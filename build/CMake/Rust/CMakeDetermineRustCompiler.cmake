# Copyright 2015-2022 Daniel Weiner
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if(NOT CMAKE_Rust_COMPILER)
	find_package(Rust)
	if(RUST_FOUND)
		set(CMAKE_Rust_COMPILER "${RUSTC_EXECUTABLE}")
		set(CMAKE_Rust_COMPILER_ID "Rust")
		set(CMAKE_Rust_COMPILER_VERSION "${RUST_VERSION}")
		set(CMAKE_Rust_PLATFORM_ID "Rust")
	endif()
endif()

message(STATUS "Cargo Home: ${CARGO_HOME}")
message(STATUS "Rust Compiler Version: ${RUSTC_VERSION}")

mark_as_advanced(CMAKE_Rust_COMPILER)

if(CMAKE_Rust_COMPILER)
	set(CMAKE_Rust_COMPILER_LOADED 1)
endif(CMAKE_Rust_COMPILER)

configure_file(${CMAKE_CURRENT_LIST_DIR}/CMakeRustCompiler.cmake.in
	${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${CMAKE_VERSION}/CMakeRustCompiler.cmake IMMEDIATE @ONLY)

set(CMAKE_Rust_COMPILER_ENV_VAR "RUSTC")
