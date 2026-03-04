#pragma once
#include <cstdint>
#include <functional>
namespace Andromeda {
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

        using MessageCallback = std::function<void(uint32_t message_type, const std::vector<uint8_t>& payload)>;
        void addOnMessageCallback(MessageCallback callback) {
            m_MessageCallbacks.push_back(std::move(callback));
        };

    private:
        void verifyBuffer();
        bool try_decode_varint(uint32_t& out_value);
        void dispatch_message();
        DecoderState m_CurrentState = DecoderState::WaitPreamble;
        std::vector<uint8_t> m_Buffer;
        uint32_t m_CurrentMsgLength = 0;
        uint32_t m_CurrentMsgType = 0;
        std::vector<MessageCallback> m_MessageCallbacks;
    };
}