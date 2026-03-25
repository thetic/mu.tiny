#!/usr/bin/env bash
# cmake/avr-simavr.sh — CTest emulator wrapper for AVR cross-compiled tests.
#
# CMake sets CMAKE_CROSSCOMPILING_EMULATOR to this script, so CTest invokes:
#
#   avr-simavr.sh <firmware.elf> [args...]
#
# simavr exits 0 regardless of firmware exit status (there is no OS-level exit
# code on bare metal).  This wrapper captures the test output, re-prints it,
# then checks whether the mutiny summary line starts with "Errors" and returns
# 1 in that case so CTest records a failure.
#
# simavr's own diagnostic messages go to stderr and are suppressed here to
# avoid polluting the test output that CTest captures.

set -euo pipefail

output=$(simavr -m atmega2560 "$@" 2>/dev/null)
printf '%s\n' "$output"

if printf '%s\n' "$output" | grep -qE '^Errors[[:space:]]'; then
    exit 1
fi

exit 0
