#pragma once

#ifdef BT_DISABLE_ASSERTS
    #define BT_ASSERT(cond, msg) 
#else
    #define BT_ASSERT(cond, msg) ::bt::assert(cond, msg)
#endif

namespace bt {
    inline void assert(const bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << "Assertion failed: " << msg << '\n';
            std::abort();
        }
    }
}