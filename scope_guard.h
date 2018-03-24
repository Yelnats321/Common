#pragma once

#include <type_traits>
#include <utility>

template <typename Func>
class scope_guard_t {
public:
    scope_guard_t(Func func) : func(std::move(func)) {}
    scope_guard_t(scope_guard_t&& other) : func(std::move(other.func)) {
        released = std::exchange(other.released, true);
    }
    scope_guard_t& operator=(scope_guard_t&& other) {
        func = std::move(other.func);
        released = std::exchange(other.released, true);
    }
    ~scope_guard_t() {
        if (!released)
            func();
    }

    void release() {
        released = true;
    }

private:
    Func func;
    bool released = false;
};

template <typename Func>
auto scope_guard(Func&& func) {
    return scope_guard_t<std::decay_t<Func>>(std::forward<Func>(func));
}
