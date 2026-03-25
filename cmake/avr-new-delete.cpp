// Minimal C++ runtime stubs for bare-metal AVR (no libstdc++ / libsupc++).
// Injected into the mutiny target via cmake/avr.cmake + CMakePresets.json.
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>  // FILE, fopen declaration
#include <time.h>   // clock_t, clock, _CLOCKS_PER_SEC_ declaration

// ---------------------------------------------------------------------------
// operator new / delete
// ---------------------------------------------------------------------------
void* operator new(size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)
        __builtin_trap();
    return ptr;
}

void* operator new[](size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)
        __builtin_trap();
    return ptr;
}

void operator delete(void* ptr) noexcept { free(ptr); }
void operator delete[](void* ptr) noexcept { free(ptr); }
// Sized-deallocation overloads (C++14, avr-g++ may warn without them)
void operator delete(void* ptr, size_t) noexcept { free(ptr); }
void operator delete[](void* ptr, size_t) noexcept { free(ptr); }

// ---------------------------------------------------------------------------
// Itanium C++ ABI guard stubs for static-local initialization.
// AVR is single-threaded; the guard object's first byte is the init flag
// (0 = not done, 1 = done). __cxa_guard_acquire returns 1 if initialization
// should proceed, 0 if it has already completed.
// ---------------------------------------------------------------------------
extern "C" {
// NOLINTNEXTLINE(bugprone-reserved-identifier,readability-identifier-naming)
int __cxa_guard_acquire(unsigned long long* g)
{
    return !*reinterpret_cast<unsigned char*>(g);
}
// NOLINTNEXTLINE(bugprone-reserved-identifier,readability-identifier-naming)
void __cxa_guard_release(unsigned long long* g)
{
    *reinterpret_cast<unsigned char*>(g) = 1;
}
// NOLINTNEXTLINE(bugprone-reserved-identifier,readability-identifier-naming)
void __cxa_guard_abort(unsigned long long*) {}
}

// ---------------------------------------------------------------------------
// POSIX stubs unavailable on bare-metal AVR (no filesystem, no POSIX clock).
// ---------------------------------------------------------------------------
extern "C" {
// fopen: no filesystem on AVR; callers must handle a nullptr return.
FILE* fopen(const char*, const char*) { return nullptr; }

// clock: no POSIX process clock on AVR; time measurements return zero.
clock_t clock() { return 0; }

// _CLOCKS_PER_SEC_ is avr-libc's backing symbol for the CLOCKS_PER_SEC
// macro.  Declared as `extern char*`; its bit-pattern is cast to clock_t
// to give the tick rate — the pointer address IS the constant value.
// Provide non-zero so the division in time.cpp does not trap.
// NOLINTNEXTLINE(bugprone-reserved-identifier,readability-identifier-naming)
char* _CLOCKS_PER_SEC_ = reinterpret_cast<char*>(1);
}
