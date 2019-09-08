#include "bst_impl.h"

#include <bst_api.h>

namespace atto {

bool nodeFound(const bst_api::Node *node)
{
    return node != nullptr;
}

bool bst_impl::contains(const MessageFormat &msg) const
{
    bool found = contains(msg.MessageId);
    return found;
}

bool bst_impl::contains(uint64_t msg_id) const
{
    bool found = false;
    {
        bst_api::Node * node = bst_api::search(m_root, msg_id);
        found = node != nullptr;
    }

    return found;
}

void bst_impl::add(const MessageFormat &msg)
{
    insert(msg.MessageId, msg);
}

void bst_impl::insert(uint64_t msg_id, const MessageFormat &msg)
{
    if (!contains(msg_id)) {
        if (m_root) {
            bst_api::Node* prev_node = bst_api::insert(m_root, msg_id, msg);
            (void)prev_node;
        } else {
            m_root = bst_api::makeNode(msg_id, msg);
        }
        ++m_size;
    }
}

MessageFormat & bst_impl::at(uint64_t msg_id) noexcept(false)
{
    if (!contains(msg_id)) {
        throw "out-of-range";
    }

    auto* node = bst_api::search(m_root, msg_id);
    return node->value;
}

const MessageFormat & bst_impl::at(uint64_t msg_id) const noexcept(false)
{
    if (!contains(msg_id)) {
        throw "out-of-range";
    }

    const auto* node = bst_api::search(m_root, msg_id);
    return node->value;
}

uint64_t bst_impl::beginIndex() const
{
    auto node = bst_api::minValueNode(m_root);
    return node->key;
}

const MessageFormat &bst_impl::beginValue() const
{
    auto index = beginIndex();
    return at(index);
}

void bst_impl::remove(uint64_t msg_id)
{
    if (contains(msg_id)) {
        m_root = bst_api::deleteNode(m_root, msg_id);
        --m_size;
    }
}

std::string bst_impl::toString() const
{
    return bst_api::toString(m_root);
}

size_t bst_impl::size() const
{
    return m_size;
}

bool bst_impl::empty() const
{
    return size() == 0;
}

}
