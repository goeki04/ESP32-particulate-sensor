#include "pch.h"
#include "EspHomeDecoder.h"

namespace Andromeda {
    void ESPHomeDecoder::appendData(const uint8_t* data, size_t length)
    {
        m_Buffer.insert(m_Buffer.end(), data, data + length);
        verifyBuffer();
    }

    void ESPHomeDecoder::verifyBuffer()
    {
        /* [Preamble 1B][Length 1-5B][Type 1-5B][Payload]
         * **Note** Preamble is always 0x00 this is the start of the package.
         */
        while (!m_Buffer.empty())
        {
            switch (m_CurrentState)
            {
            case DecoderState::WaitPreamble:
                if (m_Buffer[0] == 0x00) {
                    m_Buffer.erase(m_Buffer.begin());
                    m_CurrentState = DecoderState::ReadLength;
                }
                else {
                    m_Buffer.erase(m_Buffer.begin());
                }
                break;

            case DecoderState::ReadLength:
                if (try_decode_varint(m_CurrentMsgLength)) {
                    m_CurrentState = DecoderState::ReadType;
                }
                else {
                    return;
                }
                break;

            case DecoderState::ReadType:
                if (try_decode_varint(m_CurrentMsgType)) {
                    m_CurrentState = DecoderState::ReadPayload;
                }
                else {
                    return;
                }
                break;

            case DecoderState::ReadPayload:
                if (m_Buffer.size() >= m_CurrentMsgLength) {
                    dispatch_message();

                    m_CurrentState = DecoderState::WaitPreamble;
                }
                else {
                    return;
                }
                break;
            }
        }
    }

    bool ESPHomeDecoder::try_decode_varint(uint32_t& outValue)
    {
        uint32_t result = 0;
        uint32_t shift = 0;
        size_t consumed = 0;

        for (size_t i = 0; i < m_Buffer.size(); ++i) {
            uint8_t b = m_Buffer[i];
            consumed++;

            result |= (static_cast<uint32_t>(b & 0x7F) << shift);

            if ((b & 0x80) == 0) {
                outValue = result;
                m_Buffer.erase(m_Buffer.begin(), m_Buffer.begin() + consumed);
                return true;
            }
            shift += 7;
            if (shift >= 35) return false;
        }
        return false;
    }

    void ESPHomeDecoder::dispatch_message()
    {
        std::vector<uint8_t> payload(m_Buffer.begin(), m_Buffer.begin() + m_CurrentMsgLength);
        m_Buffer.erase(m_Buffer.begin(), m_Buffer.begin() + m_CurrentMsgLength);

        for (const auto& cb : m_MessageCallbacks) {
            if (cb) {
                cb(m_CurrentMsgType, payload);
            }
        }
    }
}