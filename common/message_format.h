#pragma once

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <string>

namespace atto {

struct MessageFormat {
    MessageFormat()
        : MessageFormat(0, 0, 0, 0)
    {
    }

    MessageFormat(uint16_t s, uint8_t t, uint64_t id, uint64_t d)
        : MessageSize(s)
        , MessageType(t)
        , MessageId(id)
        , MessageData(d)
    {
    }
    MessageFormat(const MessageFormat &) = default;
    MessageFormat(MessageFormat &&) = default;
    MessageFormat& operator= (const MessageFormat &) = default;
    MessageFormat& operator= (MessageFormat &&) = default;

    uint16_t MessageSize;
    uint8_t MessageType;
    uint64_t MessageId;
    uint64_t MessageData;
};

inline bool operator == (const MessageFormat & l, const MessageFormat & r) {
    return l.MessageSize == r.MessageSize
        && l.MessageType == r.MessageType
        && l.MessageId == r.MessageId
        && l.MessageData == r.MessageData;
}

const size_t MessageFormatSize = sizeof(MessageFormat);
const size_t CommandSize = sizeof(int);
const uint64_t TCPResendID = 10;

inline std::string toString(const MessageFormat & msg)
{
    std::string s ("MessageFormat { ");
    s += "Size: ";
    s += std::to_string(msg.MessageSize);
    s += ", ";
    s += "Type: ";
    s += std::to_string(msg.MessageType);
    s += ", ";
    s += "Id: ";
    s += std::to_string(msg.MessageId);
    s += ", ";
    s += "Data: ";
    s += std::to_string(msg.MessageData);
    s += " }";
    return s;
}

inline std::string toStringCompact(const MessageFormat & msg)
{
    std::string s ("msg { ");
    s += std::to_string(msg.MessageSize);
    s += ", ";
    s += std::to_string(msg.MessageType);
    s += ", ";
    s += std::to_string(msg.MessageId);
    s += ", ";
    s += std::to_string(msg.MessageData);
    s += " }";
    return s;
}

inline MessageFormat MakeRandMessage()
{
    const uint64_t mID = rand() % 20;
    const uint64_t mData = static_cast<uint64_t>(rand());
    MessageFormat msg(0, 0, mID, mData);
    return msg;
}

inline MessageFormat MakeMessageDiv20(uint64_t seed)
{
    uint64_t id = seed % 20;
    uint64_t data = seed;
    atto::MessageFormat msg(0, 0, id, data);
    return msg;
}

inline MessageFormat MakeMessageWithId(uint64_t id)
{
    atto::MessageFormat msg(0, 0, id, id);
    return msg;
}

}
