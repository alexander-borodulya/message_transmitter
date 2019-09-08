#pragma once

#include <bst_impl.h>
#include <common/message_format.h>

#include <memory>
#include <mutex>
#include <shared_mutex>

#undef ATTO_ENABLE_SHARED_TIMED_MUTEX

namespace atto {

    class container {
    public:
        container();
        bool contains(const MessageFormat & msg) const;
        bool contains(uint64_t msg_id) const;
        void add(const MessageFormat & msg);
        void insert(uint64_t msg_id, const MessageFormat & msg);
        MessageFormat & at(uint64_t msg_id) noexcept(false);
        const MessageFormat & at(uint64_t msg_id) const noexcept(false);
        uint64_t beginIndex() const;
        const MessageFormat &beginValue() const;
        void remove(uint64_t msg_id);
        std::string toString() const;
        size_t size() const;
        bool empty() const;

    private:
        std::unique_ptr<bst_impl> m_pimpl;
#if defined(ATTO_ENABLE_SHARED_TIMED_MUTEX)
        mutable std::shared_timed_mutex m_pimplSharedMutex;
#else
        mutable std::mutex m_pimplMutex;
#endif
    };

} // End of namespace atto
