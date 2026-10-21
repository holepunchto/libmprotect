cmake_minimum_required(VERSION 3.25)

find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)

project(mprotect C)

fetch_package("github:jedisct1/libsodium#e18eee6" SOURCE_DIR sodium)

file(COPY_FILE "${sodium}/builds/msvc/version.h" "${sodium}/src/libsodium/include/sodium/version.h")

file(GLOB_RECURSE sodium_headers CONFIGURE_DEPENDS "${sodium}/src/libsodium/**/*.h")
file(GLOB_RECURSE sodium_sources CONFIGURE_DEPENDS "${sodium}/src/libsodium/**/*.c")

# Only the guarded allocator is used, so libsodium is built with its portable
# reference implementations and no assembly or CPU feature flags. Every
# platform must still get the defines that enable page protection, as without
# them sodium_malloc() silently falls back to malloc().
add_library(sodium OBJECT)

set_target_properties(
  sodium
  PROPERTIES
  C_VISIBILITY_PRESET hidden
  POSITION_INDEPENDENT_CODE ON
)

target_sources(
  sodium
  INTERFACE
    ${sodium_headers}
  PRIVATE
    ${sodium_sources}
)

target_include_directories(
  sodium
  INTERFACE
    "${sodium}/src/libsodium/include"
  PRIVATE
    "${sodium}/src/libsodium/include/sodium"
)

target_compile_definitions(
  sodium
  PUBLIC
    SODIUM_STATIC=1
  PRIVATE
    CONFIGURED=1
    HAVE_INTTYPES_H=1
    HAVE_STDINT_H=1
)

if(NOT MSVC)
  target_compile_options(
    sodium
    PRIVATE
      -fno-strict-aliasing
      -fwrapv
  )
endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
  find_package(Threads REQUIRED)

  target_link_libraries(
    sodium
    INTERFACE
      Threads::Threads
  )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux|Android")
  target_compile_definitions(
    sodium
    PRIVATE
      _GNU_SOURCE=1
      HAVE_MADVISE=1
      HAVE_MLOCK=1
      HAVE_MMAP=1
      HAVE_MPROTECT=1
      HAVE_POSIX_MEMALIGN=1
      HAVE_PTHREAD=1
      HAVE_SYSCONF=1
      HAVE_SYS_MMAN_H=1
      HAVE_SYS_PARAM_H=1
  )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_compile_definitions(
    sodium
    PRIVATE
      HAVE_SYS_RANDOM_H=1
  )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin|iOS")
  target_compile_definitions(
    sodium
    PRIVATE
      HAVE_ARC4RANDOM=1
      HAVE_ARC4RANDOM_BUF=1
      HAVE_MADVISE=1
      HAVE_MLOCK=1
      HAVE_MMAP=1
      HAVE_MPROTECT=1
      HAVE_POSIX_MEMALIGN=1
      HAVE_PTHREAD=1
      HAVE_SYSCONF=1
      HAVE_SYS_MMAN_H=1
      HAVE_SYS_PARAM_H=1
  )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_compile_definitions(
    sodium
    PRIVATE
      _CRT_SECURE_NO_WARNINGS=1
  )
endif()

if(CMAKE_C_BYTE_ORDER MATCHES "BIG_ENDIAN")
  target_compile_definitions(
    sodium
    PRIVATE
      NATIVE_BIG_ENDIAN=1
  )
else()
  target_compile_definitions(
    sodium
    PRIVATE
      NATIVE_LITTLE_ENDIAN=1
  )
endif()

add_library(mprotect OBJECT)

set_target_properties(
  mprotect
  PROPERTIES
  C_STANDARD 99
  POSITION_INDEPENDENT_CODE ON
)

target_sources(
  mprotect
  INTERFACE
    include/mprotect.h
  PRIVATE
    src/mprotect.c
)

target_include_directories(
  mprotect
  PUBLIC
    include
  PRIVATE
    $<TARGET_PROPERTY:sodium,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(
  mprotect
  PRIVATE
    SODIUM_STATIC=1
)

add_library(mprotect_shared SHARED)

set_target_properties(
  mprotect_shared
  PROPERTIES
  OUTPUT_NAME mprotect
  WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_link_libraries(
  mprotect_shared
  PUBLIC
    mprotect
  PRIVATE
    sodium
)

add_library(mprotect_static STATIC)

set_target_properties(
  mprotect_static
  PROPERTIES
  OUTPUT_NAME mprotect
  PREFIX lib
)

target_link_libraries(
  mprotect_static
  PUBLIC
    mprotect
  PRIVATE
    sodium
)

install(TARGETS mprotect_shared mprotect_static)

install(FILES include/mprotect.h DESTINATION include)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()

  add_subdirectory(test)
endif()
