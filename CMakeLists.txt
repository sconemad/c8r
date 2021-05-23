cmake_minimum_required (VERSION 2.6)
project(c8r)

set(C8_VERSION_MAJOR 0)
set(C8_VERSION_MINOR 1)
set(C8_VERSION_PATCH 0)

configure_file("${PROJECT_SOURCE_DIR}/c8version.h.in"
               "${PROJECT_BINARY_DIR}/c8version.h")
include_directories("${PROJECT_BINARY_DIR}")
include_directories("/usr/local/include")

add_compile_options(-Wall)

#add_compile_options(-O0)
#add_compile_options(-g)

if(EMSCRIPTEN)
  add_definitions(-I${PROJECT_BINARY_DIR}/../gmp/)
  add_definitions(-I${PROJECT_BINARY_DIR}/../mpfr/src)
  add_compile_options(-Oz)
  add_definitions("-s ASSERTIONS=1")

  add_executable(c8r main-ems.c)

  target_link_libraries(c8r
    -Oz
    "--closure 1"
    "--llvm-lto 3"
    "-s NO_EXIT_RUNTIME=1"
    "-s EXPORTED_FUNCTIONS=\"['_main','_evaluate']\""
    "-s ASSERTIONS=1"
    c8
    ${PROJECT_BINARY_DIR}/../mpfr/src/.libs/libmpfr.a
    ${PROJECT_BINARY_DIR}/../gmp/.libs/libgmp.a)

else()
  add_compile_options(-fsanitize=address)
  add_executable(c8r main.c)
  target_link_libraries(c8r
    -fsanitize=address
    c8
    readline
    mpfr
    gmp)

endif()

add_library(c8
  c8bool.c
  c8buf.c
  c8cond.c
  c8ctx.c
  c8debug.c
  c8decl.c
  c8error.c
  c8eval.c
  c8expr.c
  c8flow.c
  c8func.c
  c8group.c
  c8list.c
  c8loop.c
  c8map.c
  c8mpfr.c
  c8mpz.c
  c8obj.c
  c8ops.c
  c8script.c
  c8stmt.c
  c8string.c
  c8sub.c
  c8vec.c
  )
