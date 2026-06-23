#!/usr/bin/env python3

##
# @file mock_esphome_server.py
# @brief Mock ESPHome Native API server (plaintext) for hardware-free development.
#
# @details
# Emulates a real ESPHome device on the loopback interface so the Andromeda
# client can be developed and tested without physical hardware or access to the
# device's network. Because all traffic stays on 127.0.0.1, it is unaffected by
# corporate VPNs, proxies or firewalls.
#
# The server speaks the ESPHome Native API plaintext wire format:
# @code
#     [0x00 preamble][varint payload-length][varint message-type][protobuf payload]
# @endcode
# It answers the handshake, the entity listing and the state subscription, then
# streams a continuously changing PM2.5 sensor value.
#
# @par Usage
# @code
#     python mock_esphome_server.py            # run the server  (127.0.0.1:6053)
#     python mock_esphome_server.py --client   # run the built-in self-test client
# @endcode
# The self-test client connects, performs the handshake, subscribes and prints the
# incoming values, allowing end-to-end verification in pure Python.

import socket
import struct
import sys
import threading
import time
import math
import random

## @brief Loopback host the server binds to (never leaves the machine).
HOST = "127.0.0.1"
## @brief Default ESPHome Native API TCP port.
PORT = 6053

## @name ESPHome message type identifiers (see api_options.proto)
## @{
HELLO_REQUEST = 1
HELLO_RESPONSE = 2
CONNECT_REQUEST = 3
CONNECT_RESPONSE = 4
DISCONNECT_REQUEST = 5
DISCONNECT_RESPONSE = 6
PING_REQUEST = 7
PING_RESPONSE = 8
DEVICE_INFO_REQUEST = 9
DEVICE_INFO_RESPONSE = 10
LIST_ENTITIES_REQUEST = 11
LIST_ENTITIES_SENSOR_RESPONSE = 16
LIST_ENTITIES_DONE_RESPONSE = 19
SUBSCRIBE_STATES_REQUEST = 20
SENSOR_STATE_RESPONSE = 25
## @}

## @brief Stable entity key of the emulated PM2.5 sensor; states reference it.
PM25_KEY = 1


def encode_varint(value: int) -> bytes:
    """!
    @brief Encode an unsigned integer as a protobuf base-128 varint.
    @param value Non-negative integer to encode.
    @return The varint-encoded bytes.
    """
    out = bytearray()
    while True:
        b = value & 0x7F
        value >>= 7
        if value:
            out.append(b | 0x80)
        else:
            out.append(b)
            return bytes(out)


def _tag(field: int, wire: int) -> bytes:
    """!
    @brief Build the protobuf key (tag) byte sequence for a field.
    @param field Protobuf field number.
    @param wire Protobuf wire type (0 = varint, 2 = length-delimited, 5 = 32-bit).
    @return The encoded field tag.
    """
    return encode_varint((field << 3) | wire)


def f_varint(field: int, value: int) -> bytes:
    """!
    @brief Encode a varint field (int / uint / bool / enum).
    @param field Protobuf field number.
    @param value Integer value.
    @return The encoded field.
    """
    return _tag(field, 0) + encode_varint(value)


def f_string(field: int, value: str) -> bytes:
    """!
    @brief Encode a length-delimited UTF-8 string field.
    @param field Protobuf field number.
    @param value String value.
    @return The encoded field.
    """
    data = value.encode("utf-8")
    return _tag(field, 2) + encode_varint(len(data)) + data


def f_fixed32(field: int, value: int) -> bytes:
    """!
    @brief Encode a 32-bit fixed unsigned integer field (e.g. an entity key).
    @param field Protobuf field number.
    @param value Unsigned 32-bit value.
    @return The encoded field.
    """
    return _tag(field, 5) + struct.pack("<I", value)


def f_float(field: int, value: float) -> bytes:
    """!
    @brief Encode a 32-bit IEEE-754 float field (e.g. a sensor value).
    @param field Protobuf field number.
    @param value Floating-point value.
    @return The encoded field.
    """
    return _tag(field, 5) + struct.pack("<f", value)


def frame(msg_type: int, payload: bytes) -> bytes:
    """!
    @brief Wrap a protobuf payload in the ESPHome plaintext frame.
    @param msg_type ESPHome message type identifier.
    @param payload Serialized protobuf payload.
    @return The complete framed message ready to send.
    """
    return b"\x00" + encode_varint(len(payload)) + encode_varint(msg_type) + payload


def read_varint(buf: bytes, pos: int):
    """!
    @brief Decode a base-128 varint from a buffer.
    @param buf Byte buffer to read from.
    @param pos Offset at which to start reading.
    @return A tuple (value, new_pos). @c value is @c None if more bytes are needed.
    @throws ValueError If the varint exceeds 32 bits (corrupt stream).
    """
    result = 0
    shift = 0
    while pos < len(buf):
        b = buf[pos]
        pos += 1
        result |= (b & 0x7F) << shift
        if not (b & 0x80):
            return result, pos
        shift += 7
        if shift >= 35:
            raise ValueError("varint too long / corrupt stream")
    return None, pos


def try_parse(buf: bytearray):
    """!
    @brief Attempt to extract one complete framed message from a stream buffer.
    @details TCP delivers an arbitrary byte stream, so a frame may be split across
             reads or several frames may arrive at once. Returns @c None while the
             buffer holds only a partial frame. On loss of synchronization a single
             byte is dropped via the sentinel @c "DROP".
    @param buf Accumulated receive buffer.
    @return A tuple (msg_type, payload, consumed_bytes), or @c None if incomplete.
    """
    if not buf:
        return None
    if buf[0] != 0x00:
        return ("DROP", b"", 1)
    length, pos = read_varint(buf, 1)
    if length is None:
        return None
    msg_type, pos = read_varint(buf, pos)
    if msg_type is None:
        return None
    if len(buf) - pos < length:
        return None
    payload = bytes(buf[pos:pos + length])
    return (msg_type, payload, pos + length)


def fake_pm25(t0: float) -> float:
    """!
    @brief Produce a believable, time-varying PM2.5 reading.
    @details Combines a slow sine drift, random noise and rare pollution spikes.
    @param t0 Reference start time (as returned by @c time.time()).
    @return A non-negative PM2.5 value in micrograms per cubic metre.
    """
    t = time.time() - t0
    base = 12.0 + 8.0 * math.sin(t / 15.0)
    noise = random.uniform(-1.5, 1.5)
    spike = 40.0 if random.random() < 0.03 else 0.0
    return max(0.0, base + noise + spike)


def handle_client(conn: socket.socket, addr):
    """!
    @brief Serve a single connected client for its whole lifetime.
    @details Reads framed requests, replies to the handshake, entity listing and
             ping, and on subscription starts a background thread that streams
             sensor states until the client disconnects.
    @param conn Connected client socket.
    @param addr Remote address tuple, used for logging.
    """
    print(f"[server] client connected: {addr}")
    send_lock = threading.Lock()
    stop = threading.Event()
    t0 = time.time()

    def send(msg_type, payload=b""):
        """!
        @brief Thread-safe send of a framed message on this connection.
        @param msg_type ESPHome message type identifier.
        @param payload Serialized protobuf payload (default empty).
        """
        with send_lock:
            conn.sendall(frame(msg_type, payload))

    def state_loop():
        """!
        @brief Background worker that emits a SensorStateResponse every 2 seconds.
        @details Runs until @c stop is set or the socket is closed.
        """
        while not stop.is_set():
            value = fake_pm25(t0)
            payload = f_fixed32(1, PM25_KEY) + f_float(2, value)
            try:
                send(SENSOR_STATE_RESPONSE, payload)
            except OSError:
                break
            print(f"[server] -> SensorState  PM2.5 = {value:6.2f} ug/m3")
            stop.wait(2.0)

    state_thread = None
    buf = bytearray()
    try:
        while True:
            data = conn.recv(4096)
            if not data:
                break
            buf += data
            while True:
                parsed = try_parse(buf)
                if parsed is None:
                    break
                msg_type, payload, consumed = parsed
                del buf[:consumed]
                if msg_type == "DROP":
                    continue

                if msg_type == HELLO_REQUEST:
                    print("[server] <- HelloRequest")
                    body = (f_varint(1, 1) + f_varint(2, 9) +
                            f_string(3, "mock-esphome 1.0") +
                            f_string(4, "andromeda-mock"))
                    send(HELLO_RESPONSE, body)

                elif msg_type == CONNECT_REQUEST:
                    print("[server] <- ConnectRequest")
                    send(CONNECT_RESPONSE, f_varint(1, 0))

                elif msg_type == DEVICE_INFO_REQUEST:
                    print("[server] <- DeviceInfoRequest")
                    body = (f_varint(1, 0) +
                            f_string(2, "andromeda-mock") +
                            f_string(6, "MockSensor"))
                    send(DEVICE_INFO_RESPONSE, body)

                elif msg_type == LIST_ENTITIES_REQUEST:
                    print("[server] <- ListEntitiesRequest")
                    sensor = (f_string(1, "pm25") +
                              f_fixed32(2, PM25_KEY) +
                              f_string(3, "Particulate Matter 2.5") +
                              f_string(4, "andromeda-mock-pm25") +
                              f_string(6, "ug/m3") +
                              f_varint(7, 1) +
                              f_string(9, "pm25"))
                    send(LIST_ENTITIES_SENSOR_RESPONSE, sensor)
                    send(LIST_ENTITIES_DONE_RESPONSE)

                elif msg_type == SUBSCRIBE_STATES_REQUEST:
                    print("[server] <- SubscribeStatesRequest (starting stream)")
                    if state_thread is None:
                        state_thread = threading.Thread(target=state_loop, daemon=True)
                        state_thread.start()

                elif msg_type == PING_REQUEST:
                    send(PING_RESPONSE)

                elif msg_type == DISCONNECT_REQUEST:
                    send(DISCONNECT_RESPONSE)
                    return
                else:
                    print(f"[server] <- (ignored message type {msg_type})")
    except ConnectionResetError:
        pass
    finally:
        stop.set()
        conn.close()
        print(f"[server] client disconnected: {addr}")


def run_server():
    """!
    @brief Start the mock server and accept clients until interrupted.
    @details Each accepted connection is served on its own daemon thread.
    """
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as srv:
        srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        srv.bind((HOST, PORT))
        srv.listen()
        print(f"[server] mock ESPHome listening on {HOST}:{PORT}  (Ctrl+C to stop)")
        while True:
            conn, addr = srv.accept()
            threading.Thread(target=handle_client, args=(conn, addr), daemon=True).start()


def run_client():
    """!
    @brief Built-in self-test client that exercises the server in pure Python.
    @details Connects, performs the handshake, requests the entity list, subscribes
             to states and prints every decoded sensor value.
    """
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        print(f"[client] connected to {HOST}:{PORT}")
        s.sendall(frame(HELLO_REQUEST, f_string(1, "andromeda-test") + f_varint(2, 1) + f_varint(3, 9)))
        s.sendall(frame(CONNECT_REQUEST, f_string(1, "")))
        s.sendall(frame(LIST_ENTITIES_REQUEST, b""))
        s.sendall(frame(SUBSCRIBE_STATES_REQUEST, b""))

        buf = bytearray()
        while True:
            data = s.recv(4096)
            if not data:
                break
            buf += data
            while True:
                parsed = try_parse(buf)
                if parsed is None:
                    break
                msg_type, payload, consumed = parsed
                del buf[:consumed]
                if msg_type == "DROP":
                    continue
                if msg_type == HELLO_RESPONSE:
                    print("[client] <- HelloResponse")
                elif msg_type == CONNECT_RESPONSE:
                    print("[client] <- ConnectResponse")
                elif msg_type == LIST_ENTITIES_SENSOR_RESPONSE:
                    print("[client] <- ListEntitiesSensorResponse (found a sensor)")
                elif msg_type == SENSOR_STATE_RESPONSE:
                    key, state = decode_sensor_state(payload)
                    print(f"[client] <- SensorState  key={key}  value={state:.2f}")


def decode_sensor_state(payload: bytes):
    """!
    @brief Decode a SensorStateResponse payload.
    @details Reads @c fixed32 key (field 1) and @c float state (field 2), skipping
             any other fields.
    @param payload Serialized SensorStateResponse protobuf payload.
    @return A tuple (key, state).
    """
    key = 0
    state = 0.0
    pos = 0
    while pos < len(payload):
        tag, pos = read_varint(payload, pos)
        field = tag >> 3
        wire = tag & 0x7
        if wire == 5:
            raw = payload[pos:pos + 4]
            pos += 4
            if field == 1:
                key = struct.unpack("<I", raw)[0]
            elif field == 2:
                state = struct.unpack("<f", raw)[0]
        elif wire == 0:
            _, pos = read_varint(payload, pos)
        else:
            break
    return key, state


if __name__ == "__main__":
    if "--client" in sys.argv:
        run_client()
    else:
        try:
            run_server()
        except KeyboardInterrupt:
            print("\n[server] stopped.")
