#pragma once

#ifndef unreachable

#ifdef __GNUC__ // GCC, Clang, ICC
#define unreachable() (__builtin_unreachable())
#else
#ifdef _MSC_VER // MSVC
#define unreachable() (__assume(false))
#else

[[noreturn]] inline void unreachable_impl() {}
#define unreachable() (unreachable_impl())

#endif
#endif

#endif
