#include "container.h"
#include <bst_impl.h>

namespace atto {

    container::container()
        : m_pimpl(std::make_unique<bst_impl>())
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        , m_pimplSharedMutex()
#else
        , m_pimplMutex()
#endif
    {
    }

    bool container::contains(const MessageFormat &msg) const
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::shared_lock<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        bool found = m_pimpl->contains(msg.MessageId);
        return found;
    }

    bool container::contains(uint64_t msg_id) const
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::shared_lock<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        bool found = m_pimpl->contains(msg_id);
        return found;
    }

    void container::add(const MessageFormat &msg)
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::lock_guard<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        m_pimpl->add(msg);
    }

    void container::insert(uint64_t msg_id, const MessageFormat &msg)
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::lock_guard<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        m_pimpl->insert(msg_id, msg);
    }

    MessageFormat & container::at(uint64_t msg_id)
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::lock_guard<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        return m_pimpl->at(msg_id);
    }

    const MessageFormat & container::at(uint64_t msg_id) const
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::shared_lock<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        return m_pimpl->at(msg_id);
    }

    uint64_t container::beginIndex() const
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::lock_guard<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        return m_pimpl->beginIndex();
    }

    const MessageFormat &container::beginValue() const
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::lock_guard<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        return m_pimpl->beginValue();
    }

    void container::remove(uint64_t msg_id)
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::lock_guard<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        m_pimpl->remove(msg_id);
    }

    std::string container::toString() const
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::shared_lock<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        return m_pimpl->toString();
    }

    size_t container::size() const
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::shared_lock<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        return m_pimpl->size();
    }

    bool container::empty() const
    {
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        std::shared_lock<std::shared_timed_mutex> lock_read(m_pimplSharedMutex);
#else
        std::lock_guard<std::mutex> lock(m_pimplMutex);
#endif
        return m_pimpl->empty();
    }

} // End of namespace atto
