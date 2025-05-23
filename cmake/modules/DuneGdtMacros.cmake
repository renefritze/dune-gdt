# ~~~
# This file is part of the dune-gdt project:
# https://github.com/dune-community/dune-gdt
# Copyright 2010-2018 dune-gdt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
# with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
# Felix Schindler (2014, 2016 - 2018)
# René Fritze     (2017 - 2018)
# ~~~

set(DUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS TRUE)

include(XtCompilerSupport)
include(XtTooling)
include(DuneXTHints)

set(DXT_DONT_LINK_PYTHON_LIB
    ${DXT_DONT_LINK_PYTHON_LIB}
    CACHE STRING "wheelbuilders want to set this to 1")

# library checks  #########################################################################
find_package(PkgConfig)

find_package(
  Boost 1.48.0 REQUIRED
  COMPONENTS atomic
             chrono
             date_time
             filesystem
             system
             thread
             timer)
dune_register_package_flags(INCLUDE_DIRS ${Boost_INCLUDE_DIRS})

if(TARGET Boost::headers)
  dune_register_package_flags(LIBRARIES Boost::headers)
endif()

foreach(
  boost_lib IN
  ITEMS atomic
        chrono
        date_time
        filesystem
        system
        thread
        timer)
  if(TARGET Boost::${boost_lib})
    dune_register_package_flags(LIBRARIES Boost::${boost_lib})
  else()
    string(TOUPPER "${boost_lib}" _boost_lib)
    dune_register_package_flags(LIBRARIES ${Boost_${_boost_lib}_LIBRARY})
  endif()
endforeach()

find_package(Eigen3 3.4.0)

if(EIGEN3_FOUND)
  dune_register_package_flags(INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR} COMPILE_DEFINITIONS "ENABLE_EIGEN=1")
  set(HAVE_EIGEN 1)
else(EIGEN3_FOUND)
  dune_register_package_flags(COMPILE_DEFINITIONS "ENABLE_EIGEN=0")
  set(HAVE_EIGEN 0)
endif(EIGEN3_FOUND)

find_package(MKL)

if(MKL_FOUND)
  set(HAVE_LAPACKE 0)
else(MKL_FOUND)
  find_package(LAPACKE)
endif(MKL_FOUND)

find_package(Spe10Data)

# intel mic and likwid don't mix
if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "k1om")
  include(FindLIKWID)
  find_package(LIKWID)

  if(LIKWID_FOUND)
    dune_register_package_flags(INCLUDE_DIRS ${LIKWID_INCLUDE_DIR} LIBRARIES ${LIKWID_LIBRARY})
  endif(LIKWID_FOUND)
else()
  set(HAVE_LIKWID 0)
endif()

include(DuneTBB)

if(HAVE_MPI)
  include(FindMPI4PY)

  if(MPI4PY_FOUND)

  else()
    execute_process(
      COMMAND ${RUN_IN_ENV_SCRIPT} pip install mpi4py
      ERROR_VARIABLE shell_error
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    myfind_mpi4py()
  endif()

  if(MPI4PY_FOUND)
    # this only works in dependent modules
    dune_register_package_flags(INCLUDE_DIRS "${MPI4PY_INCLUDE_DIR}")

    # this only works in dune-gdt itself
    include_directories(SYSTEM "${MPI4PY_INCLUDE_DIR}")
  else()
    message(FATAL_ERROR "mpi4py is required, but could not be installed")
  endif()
endif()

# dune-common's FindX does not set this correctly
include_directories(SYSTEM ${PYTHON_INCLUDE_DIRS})

# end library checks  #####################################################################

# misc vars  #########################################################################
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(DS_MAX_MIC_THREADS
    120
    CACHE STRING "")
set(DUNE_XT_COMMON_TEST_DIR ${dune-gdt_SOURCE_DIR}/dune/xt/common/test)
set(ENABLE_PERFMON
    0
    CACHE STRING "enable likwid performance monitoring API usage")

if(NOT DS_HEADERCHECK_DISABLE)
  set(ENABLE_HEADERCHECK 1)
endif(NOT DS_HEADERCHECK_DISABLE)

set(DXT_TEST_TIMEOUT
    1000
    CACHE STRING "per-test timeout in seconds")
set(DXT_TEST_PROCS
    3
    CACHE STRING "run N tests in parallel")

set(DUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS TRUE)

include(DunePybindxiUtils)

# TODO there's no real way to require packages present at configure time? - jinja2,pyparsing is needed for test
# templating can't use the run-in-env script here, because it will try to use dune.common which we're trying to install
# can't use dune-common_WHEELHOUSE here, because it is not set yet can't use a wildcard here, because no shell expansion
# is done
dune_execute_process(
  COMMAND
  ${CMAKE_BINARY_DIR}/dune-env/bin/python
  -m
  pip
  install
  jinja2
  pyparsing)
execute_process(
  COMMAND ${CMAKE_BINARY_DIR}/dune-env/bin/python -m pip install
          ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/share/dune/wheelhouse/dune_common-2.10.0-py3-none-any.whl
  COMMAND ${CMAKE_BINARY_DIR}/dune-env/bin/python -m pip install
          ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/share/dune/wheelhouse/dune_testtools-2.4-py3-none-any.whl
  OUTPUT_VARIABLE pip_install_log ECHO_OUTPUT_VARIABLE ECHO_ERROR_VARIABLE
  ERROR_VARIABLE pip_install_log
  OUTPUT_STRIP_TRAILING_WHITESPACE)
