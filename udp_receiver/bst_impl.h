#pragma once

#include <common/message_format.h>

namespace atto {

    namespace bst_api {
        struct Node;
    }

    class bst_impl {
    public:
        bool contains(const MessageFormat & msg) const;
        bool contains(uint64_t msg_id) const;
        void add(const MessageFormat & msg);
        void insert(uint64_t msg_id, const MessageFormat & msg);
        MessageFormat & at(uint64_t msg_id) noexcept(false);
        const MessageFormat & at(uint64_t msg_id) const noexcept(false);
        uint64_t beginIndex() const;
        const MessageFormat & beginValue() const;
        void remove(uint64_t msg_id);
        std::string toString() const;
        size_t size() const;
        bool empty() const;

    private:
        bst_api::Node * m_root = nullptr;
        size_t m_size = 0;
    };
}
