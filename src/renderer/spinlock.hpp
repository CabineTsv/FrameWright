#pragma once

#include <atomic>
#include <thread>

class Spinlock {
public:
    void wait_for(bool state) const {
        while (m_flag.load(std::memory_order_acquire) != state)
            std::this_thread::yield();
    }

    [[nodiscard]] bool read() const {
        return m_flag.load(std::memory_order_acquire);
    }

    void set(bool state) {
        m_flag.store(state, std::memory_order_release);
    }

private:
    std::atomic<bool> m_flag { false };
};