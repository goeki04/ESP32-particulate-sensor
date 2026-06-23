#pragma once
#include <cstdint>
#include "a_Primitives.hpp"
#include <functional>
namespace Andromeda::Network {
    enum class DecoderState {
        WaitPreamble,
        ReadLength,
        ReadType,
        ReadPayload
    };

    class ESPHomeDecoder {
    public:
        ESPHomeDecoder() {};
        ~ESPHomeDecoder() = default;

        void appendData(const uint8_t* data, size_t length);

        using MessageCallback = std::function<void(u32 message_type, const std::vector<u8>& payload)>;
        void addOnMessageCallback(MessageCallback callback) {
            m_MessageCallbacks.push_back(std::move(callback));
        };

    private:
        void verifyBuffer();
        bool try_decode_varint(u32& out_value);
        void dispatch_message();
        DecoderState m_CurrentState = DecoderState::WaitPreamble;
        std::vector<uint8_t> m_Buffer;
        u32 m_CurrentMsgLength = 0;
        u32 m_CurrentMsgType = 0;
        std::vector<MessageCallback> m_MessageCallbacks;
    };
}