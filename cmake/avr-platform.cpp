// cmake/avr-platform.cpp
// AVR/simavr platform support: routes stdout to the simavr console register
// so that test output is visible when running under simavr.
//
// Injected directly into test executables (not the library) via cmake/avr.cmake
// so that:
//  - the .mmcu ELF section (which simavr reads for configuration) is always
//    linked in unconditionally, and
//  - the __attribute__((constructor)) startup code runs before main().
//
// .mmcu section layout (fixed by the simavr ABI):
//   Every entry is a tag-length-value record (packed).
//   Tag  0 = anchor (mandatory root entry, MUST be named _mmcu for -u,_mmcu)
//   Tag  1 = MCU name (NUL-padded string, 64 bytes)
//   Tag  2 = clock frequency in Hz (uint32_t)
//   Tag 11 = console I/O register address (void*)
//
// We reproduce the struct layout from <simavr/avr/avr_mcu_section.h> here to
// avoid a hard build-time dependency on simavr headers.
//
// .mmcu section retention:
//   On modern avr-gcc with -flto + --gc-sections the .mmcu section gets
//   stripped because no live code references it.  __attribute__((retain))
//   is not supported on AVR.  The workaround is the standard simavr pattern:
//   name the anchor _mmcu (C linkage) and pass -Wl,-u,_mmcu to the linker.
//   cmake/avr.cmake adds this flag to every test executable target.
//
// stdout redirection:
//   avr-libc initialises stdout to a do-nothing stream.  We replace it with a
//   stream whose put() callback writes each character to GPIOR0.  simavr
//   intercepts those I/O writes (because mmcu_console points to GPIOR0) and
//   forwards the characters to the host process's stdout.

#ifndef __AVR__
#error "avr-platform.cpp must only be compiled for AVR targets (__AVR__ not defined)"
#endif

#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>

// ── .mmcu section data ────────────────────────────────────────────────────────

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define _MMCU_ __attribute__((section(".mmcu"), used))

namespace {

struct MmcuLong {
    uint8_t tag;
    uint8_t len;
    uint32_t val;
} __attribute__((__packed__));

struct MmcuString {
    uint8_t tag;
    uint8_t len;
    char str[64];
} __attribute__((__packed__));

struct MmcuAddr {
    uint8_t tag;
    uint8_t len;
    void* what;
} __attribute__((__packed__));

const MmcuString mmcu_name _MMCU_ = {
    1,
    static_cast<uint8_t>(sizeof(MmcuString) - 2),
    {'a', 't', 'm', 'e', 'g', 'a', '2', '5', '6', '0'}
};

const MmcuLong mmcu_freq _MMCU_ = {
    2,
    static_cast<uint8_t>(sizeof(MmcuLong) - 2),
    16000000UL
};

// simavr will intercept I/O writes to this register and print them as console
// output.  GPIOR0 (General Purpose I/O Register 0) is unused by avr-libc and
// safe to repurpose for this role.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
const MmcuAddr mmcu_console _MMCU_ = {
    11,
    static_cast<uint8_t>(sizeof(MmcuAddr) - 2),
    reinterpret_cast<void*>(const_cast<uint8_t*>(&GPIOR0))
};

} // namespace

// The anchor MUST have C linkage and be named _mmcu so that the linker flag
// -Wl,-u,_mmcu can force-include the .mmcu section even with --gc-sections.
// The anchor is placed last so simavr walks the preceding entries first.
// NOLINTNEXTLINE(readability-identifier-naming,cppcoreguidelines-avoid-non-const-global-variables)
extern "C" const uint8_t _mmcu[2] _MMCU_ = {0, 0};

// ── stdout redirection ───────────────────────────────────────────────────────

namespace {

int simavr_putchar(char c, FILE*)
{
    GPIOR0 = static_cast<uint8_t>(c);
    return 0;
}

// FDEV_SETUP_STREAM expands to an aggregate initialiser for avr-libc's FILE.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
FILE simavr_stdout = FDEV_SETUP_STREAM(simavr_putchar, nullptr, _FDEV_SETUP_WRITE);

// Runs before main() via the .init_array mechanism.
// Replaces the default (no-op) stdout/stderr with the simavr console stream.
__attribute__((constructor)) void avr_platform_init()
{
    stdout = &simavr_stdout;
    stderr = &simavr_stdout;
}

} // namespace

// ── simavr exit signalling ────────────────────────────────────────────────────
//
// avr-gcc 15 / avr-libc's __stop_program uses only `cli; rjmp .-2` — no SLEEP
// instruction.  simavr's graceful-quit detection fires on SLEEP with interrupts
// disabled, so without SLEEP the simulator spins forever.
//
// A destructor (`.fini_array` entry) runs just before the CRT reaches
// __stop_program, giving us a chance to execute `cli; sleep`.  The `sleep`
// is a NOP on real hardware when SE (SMCR.SE) is clear, so this is safe for
// both simulation and deployment.
//
// `used` prevents --gc-sections from discarding the function body.
__attribute__((destructor, used)) void simavr_quit()
{
    __asm__ __volatile__("cli" ::: "memory");
    __asm__ __volatile__("sleep");
}
