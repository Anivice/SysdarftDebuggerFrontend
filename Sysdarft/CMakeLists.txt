cmake_minimum_required(VERSION 3.30)

project(SysdarftDebuggerFrontend VERSION 0.0.1 LANGUAGES CXX)

# Enable Ccache support
find_program(CCACHE_PROGRAM ccache)
if(NOT CCACHE_PROGRAM)
    message(FATAL_ERROR "ccache not found! Please install ccache or adjust the configuration.")
endif()

# Set ccache as the compiler launcher before setting the compiler
set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")

# Optionally, set the C and C++ compilers if you need specific versions
# set(CMAKE_C_COMPILER "/usr/bin/gcc" CACHE STRING "C Compiler" FORCE)
# set(CMAKE_CXX_COMPILER "/usr/bin/g++" CACHE STRING "C++ Compiler" FORCE)

# Set C and C++ standards
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable position-independent code if needed
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set visibility properties
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# WARNING:  Standard C++14 library can cause thread sanitizers to report data race in certain areas,
#           which is considered a false positive, but will cause the unit test to fail. In C++23 this
#           issue simply does not exist

# Universal compiler and linker flags
set(compiler_options
        # Warnings and diagnostics
        -Wall                                   # Enable common warnings
        -Wextra                                 # Enable extra warnings
        -Wpedantic                              # Strict compliance with the standard
        -Wunused                                # Warn about unused variables, functions, etc.
        -Wuninitialized                         # Warn if variables are used uninitialized
        -fdiagnostics-show-option               # Show which option triggered the warning
        -fdiagnostics-color=always              # Enable colored diagnostics for better readability

        # Debugging and stack protection
        -g3                                     # Maximum debug information, including macro expansions
        -O0
        -fstack-usage                           # Generate stack usage info for each function
        -fstack-protector-all                   # Protect all functions with a stack canary to prevent stack overflow attacks
        # -D_FORTIFY_SOURCE=2                   # Buffer overflow detection on safer libc functions (e.g., memcpy).
        # You need to enable optimization for _FORTIFY_SOURCE to work!
        -gdwarf-4                               # Generate DWARF version 4 debug information

        -fno-eliminate-unused-debug-types
        -fno-omit-frame-pointer
        -lasan -lubsan -fPIC -fpie
)

set(linker_options
        # Linker options for memory safety, thread safety, and verbose debugging
        -Wl,--no-omagic                         # Prevent the generation of object files in memory; useful for debugging
        -Wl,--as-needed                         # Only link libraries that are actually needed to reduce binary size
        -Wl,--fatal-warnings                    # Treat all linker warnings as errors to catch issues early
        -Wl,-z,relro                            # Read-only relocations to prevent certain memory exploits (optional)
        -Wl,-z,now                              # Fully resolve all symbols during the link time for extra safety
        -Wl,-z,noexecstack                      # Prevent execution of code on the stack (security hardening)
        -Wl,-z,defs                             # Ensure all symbols are defined, and prevent undefined symbols
        -Wl,-O0

        -gdwarf-4                               # Generate detailed debug information for the linker
        -fno-eliminate-unused-debug-types
        -fno-omit-frame-pointer

        # Stack protection
        -fstack-protector-all                   # Link with stack protection for all functions
        -lasan -lubsan -fPIC -fpie --whole-file
)

if ("${COMPILE_WITH_MEMORY_SANITIZERS}" STREQUAL "True")
    message(STATUS "Sanitizers for memory enabled")
    list(APPEND compiler_options
            -fsanitize=address                  # Detect illegal memory access such as buffer overflows and use-after-free
            -fsanitize=undefined                # Detect undefined behavior like integer overflows and null dereferencing
            # Code coverage options
            -fprofile-arcs                      # Enable code coverage instrumentation
            -ftest-coverage                     # Generate coverage test data
    )

    list(APPEND linker_options
            -fsanitize=address                  # Link the AddressSanitizer runtime for memory integrity
            -fsanitize=undefined                # Link the UndefinedBehaviorSanitizer for detecting undefined behavior
            # Code coverage options
            -fprofile-arcs                      # Enable code coverage instrumentation
            -ftest-coverage                     # Generate coverage test data
    )
endif ()

if ("${COMPILE_WITH_THREAD_SANITIZERS}" STREQUAL "True")
    message(STATUS "Sanitizers for thread enabled")
    list(APPEND compiler_options
            -fsanitize=undefined                    # Detect undefined behavior like integer overflows and null dereferencing
            -fsanitize=thread                       # Ensure thread safety by detecting data races
    )
    list(APPEND linker_options
            -fsanitize=undefined                    # Link the UndefinedBehaviorSanitizer for detecting undefined behavior
            -fsanitize=thread                       # Link the ThreadSanitizer runtime for thread safety
    )
endif ()

# Sanity check
if (("${COMPILE_WITH_MEMORY_SANITIZERS}" STREQUAL "True") AND ("${COMPILE_WITH_THREAD_SANITIZERS}" STREQUAL "True"))
    message(FATAL_ERROR "Memory sanitizers and thread sanitizers are mutually exclusive!")
endif ()

set(optimization_flags
        -O3
        -march=native
        -mtune=native
        -flto
        -fomit-frame-pointer
        -ffast-math
        -fstrict-aliasing
        -fdata-sections
        -ffunction-sections
        -s
)

set(optimization_link_flags
        -O3
        -march=native
        -mtune=native
        -flto
        -fomit-frame-pointer
        -ffast-math
        -fstrict-aliasing
        -fdata-sections
        -ffunction-sections
        -Wl,--gc-sections
        -s
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_options(${compiler_options})
    add_link_options(${linker_options})
    message(STATUS "Debug mode enabled")
else ()
    add_compile_options(${optimization_flags})
    add_compile_options(${optimization_link_flags})
endif ()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES Sysdarft_en_US.ts)

set(PROJECT_SOURCES
        main.cpp
        sysdarft.cpp
        sysdarft.h
        sysdarft.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Sysdarft
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        dialogconnect.h dialogconnect.cpp dialogconnect.ui
        dialogwarning.h dialogwarning.cpp dialogwarning.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Sysdarft APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Sysdarft SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Sysdarft
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Sysdarft PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Sysdarft)
endif()
set_target_properties(Sysdarft PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Sysdarft
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Sysdarft)
endif()
