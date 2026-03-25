# cmake/avr.cmake — included via CMAKE_PROJECT_mutiny_INCLUDE for the avr preset.
# Overrides the default CMAKE_PROJECT_mutiny_INCLUDE (warnings.cmake) so we must
# re-include it explicitly, then defer new/delete injection until after add_library().
include("${CMAKE_CURRENT_LIST_DIR}/warnings.cmake")

# The mutiny target is defined after project(), so use DEFER to call target_sources
# once the top-level CMakeLists.txt has finished processing all targets.
# CMAKE_CURRENT_LIST_DIR is captured now; at deferred-call time it would resolve
# to the top-level source dir instead of cmake/.
set(_avr_cmake_dir "${CMAKE_CURRENT_LIST_DIR}")
cmake_language(DEFER CALL target_sources mutiny
    PRIVATE "${_avr_cmake_dir}/avr-new-delete.cpp"
)

# Inject avr-platform.cpp directly into test executables (not the library).
# It must be in the exe so that:
#   (a) the .mmcu section data is unconditionally linked in, and
#   (b) the __attribute__((constructor)) stdout redirector runs before main().
# A helper function lets us guard against the target not existing (e.g. when
# MUTINY_BUILD_TESTING is OFF).
function(_mutiny_avr_add_platform_to_target target src)
    if(TARGET "${target}")
        target_sources("${target}" PRIVATE "${src}")
        # Force the linker to keep the .mmcu section.
        # On AVR, __attribute__((retain)) is not supported and --gc-sections
        # drops the .mmcu section because no live code references it.  The
        # standard simavr workaround is to name the anchor _mmcu (C linkage)
        # and pass -u,_mmcu, which forces the linker to pull in the symbol and
        # transitively keep the whole .mmcu section.
        target_link_options("${target}" PRIVATE "LINKER:-u,_mmcu")
    endif()
endfunction()

cmake_language(DEFER CALL _mutiny_avr_add_platform_to_target
    mutiny_tests
    "${_avr_cmake_dir}/avr-platform.cpp"
)

cmake_language(DEFER CALL _mutiny_avr_add_platform_to_target
    ExampleTests
    "${_avr_cmake_dir}/avr-platform.cpp"
)
