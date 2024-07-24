#pragma once

#ifdef __GNUC__

#define BREW_API __attribute__((sysv_abi))

#define BREW_EXPORT __attribute__((visibility("default")))

#define BREW_NORETURN __attribute__((noreturn))

#else

#error Unsupported compiler.

#endif
