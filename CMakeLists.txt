cmake_minimum_required (VERSION 2.6)
project(calcul8r)

set(C8_VERSION_MAJOR 0)
set(C8_VERSION_MINOR 2)
set(C8_VERSION_PATCH 0)

configure_file("${PROJECT_SOURCE_DIR}/calcul8/c8version.h.in"
               "${PROJECT_BINARY_DIR}/calcul8/c8version.h")
include_directories("${PROJECT_BINARY_DIR}")
include_directories("/usr/local/include")

add_compile_options(-Wall)

option(ENABLE_ASAN "Enable address sanitizer" 0)
if(ENABLE_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(EMSCRIPTEN)
  add_definitions(-I${PROJECT_BINARY_DIR}/../gmp/)
  add_definitions(-I${PROJECT_BINARY_DIR}/../mpfr/src)
  add_compile_options(-Oz)
  add_definitions("-s ASSERTIONS=1")

  add_executable(calcul8r main-ems.c)

  target_link_libraries(calcul8r
    -Oz
    "--closure 1"
    "--llvm-lto 3"
    "-s NO_EXIT_RUNTIME=1"
    "-s EXPORTED_FUNCTIONS=\"['_main','_evaluate']\""
    "-s ASSERTIONS=1"
    calcul8
    ${PROJECT_BINARY_DIR}/../mpfr/src/.libs/libmpfr.a
    ${PROJECT_BINARY_DIR}/../gmp/.libs/libgmp.a)

else()
  add_executable(calcul8r main.c)
  target_link_libraries(calcul8r
    calcul8
    readline
    mpfr
    gmp)
  
  enable_testing()

  file(GLOB TESTSCRIPTS "test/scripts/*.c8")
  foreach(SCRIPT IN LISTS TESTSCRIPTS)
    add_test(NAME "${SCRIPT}" COMMAND calcul8r -d4 "${SCRIPT}")
  endforeach()  
endif()

add_library(calcul8
  calcul8/c8bool.c
  calcul8/c8buf.c
  calcul8/c8cond.c
  calcul8/c8ctx.c
  calcul8/c8debug.c
  calcul8/c8decl.c
  calcul8/c8error.c
  calcul8/c8eval.c
  calcul8/c8expr.c
  calcul8/c8flow.c
  calcul8/c8func.c
  calcul8/c8group.c
  calcul8/c8list.c
  calcul8/c8loop.c
  calcul8/c8map.c
  calcul8/c8mpfr.c
  calcul8/c8mpz.c
  calcul8/c8obj.c
  calcul8/c8ops.c
  calcul8/c8script.c
  calcul8/c8stmt.c
  calcul8/c8string.c
  calcul8/c8sub.c
  calcul8/c8vec.c
  )
