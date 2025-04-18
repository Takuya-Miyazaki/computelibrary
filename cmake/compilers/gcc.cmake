# Copyright (c) 2025 Arm Limited.
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(ARM_COMPUTE_NO_WARN_FLAG -w CACHE STRING "Compiler flags to disable warnings")
  set(ARM_COMPUTE_STANDARD_WARNINGS -Wall -Wextra -Wpedantic CACHE STRING "Compiler flags to enable warnings.")

  # * Address sanitizer.
  set(ARM_COMPUTE_ASAN_COMPILER_FLAG_INIT -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined)
  set(ARM_COMPUTE_ASAN_LINKER_FLAG_INIT -fsanitize=address -fsanitize=undefined)

  # * Code coverage.
  set(ARM_COMPUTE_CODE_COVERAGE_COMPILER_FLAG_INIT --coverage CACHE STRING "Compiler flags to enable code coverage.")
  set(ARM_COMPUTE_CODE_COVERAGE_LINKER_FLAG_INIT --coverage CACHE STRING "Linker flags to enable code coverage.")

  # * CXX flags.
  set(ARM_COMPUTE_CCXX_FLAGS_INIT "" CACHE STRING "Base C/CXX flags.")
  set(ARM_COMPUTE_CCXX_FLAGS_RELEASE -O3 CACHE STRING "Release config specific C/CXX flags.")
  set(ARM_COMPUTE_CCXX_FLAGS_DEBUG -g -O0 CACHE STRING "Debug config specific C/CXX flags.")

  # * Linker flags.
  set(ARM_COMPUTE_LINKER_FLAGS_INIT "" CACHE STRING "Base linker flags.")
  set(ARM_COMPUTE_LINKER_FLAGS_DEBUG "" CACHE STRING "Debug config specific linker flags.")
  set(ARM_COMPUTE_LINKER_FLAGS_RELEASE "" CACHE STRING "Release config specific linker flags.")

  if(ARM_COMPUTE_USE_LIBCXX)
    message(FATAL_ERROR "Libcxx is not enabled for this compiler/platform.")
  endif()

  if(ARM_COMPUTE_USE_LLD)
    message(FATAL_ERROR "LLD is not enabled for this compiler/platform.")
  endif()
endif()
