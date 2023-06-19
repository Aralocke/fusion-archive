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

.DEFAULT_TARGET := all

CMAKE := $(shell command -v cmake 2>/dev/null)
PODMAN := $(shell command -v podman 2>/dev/null)
TOOLCHAIN := clang

ifeq ($(OS),Windows_NT)
    OS_NAME = windows
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        ARCH = X64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            ARCH = X64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            ARCH = X32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OS_NAME = linux
    endif
    ifeq ($(UNAME_S),Darwin)
        OS_NAME = osx
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        ARCH = X64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        ARCH = X32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        ARCH = ARM
    endif
endif

# Default make target, make everything
all: $(OS_NAME)-build-all
build: $(OS_NAME)-build
clean: $(OS_NAME)-clean-all

debug: $(OS_NAME)-build-debug
public: $(OS_NAME)-build-public

builder:
	$(PODMAN) build -t fusion-builder:latest -f Build/Dockerfile .

proj: $(OS_NAME)-proj

# Global targets
check-dir:
	@if [ ! -d tmp ]; then \
		mkdir tmp || exit 1; \
	fi

check-prereqs:
ifndef CMAKE
	@echo "CMAKE does not exist on the path" && exit 1
endif

check-toolchain:
	@if [ ! -f build/CMake/Toolchains/$(TOOLCHAIN).cmake ]; then \
		echo >&2 "Toolchain does not exist for '$(TOOLCHAIN)'"; \
		exit 1; \
	fi

# Utility Support
boringssl-update:
	@if [ ! -d external/libraries/boringssl/src ]; then \
		@echo >&2 "BoringSSL submodule not initialized"; \
		exit 1; \
	fi
	@echo "Building BoringSSL toolchain container"
	cd external/libraries/boringssl && \
		$(PODMAN) build -t boringssl-builder:latest -f Dockerfile .
	@echo "Running BoringSSL toolchain update"
	cd external/libraries/boringssl && \
		./generate_files.sh
#
# Linux specific build targets
#
linux-build: linux-build-debug linux-build-public linux-build-asan linux-build-ubsan

linux-build-all: linux-build-debug linux-build-release linux-build-releasedbg \
	linux-build-public linux-build-asan linux-build-ubsan

linux-build-asan: linux-cmake-asan
	cd tmp/Linux/$(ARCH)-Asan-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config ASAN \
			--parallel $(NPROC)

linux-build-debug: linux-cmake-debug
	cd tmp/Linux/$(ARCH)-Debug-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config DEBUG \
			--parallel $(NPROC)

linux-build-public: linux-cmake-public
	cd tmp/Linux/$(ARCH)-Public-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config MINSIZEREL \
			--parallel $(NPROC)

linux-build-release: linux-cmake-release
	cd tmp/Linux/$(ARCH)-Release-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config RELEASE \
			--parallel $(NPROC)

linux-build-releasedbg: linux-cmake-releasedbg
	cd tmp/Linux/$(ARCH)-ReleaseDbg-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config RELWITHDEBINFO \
			--parallel $(NPROC)

linux-build-ubsan: linux-cmake-ubsan
	cd tmp/Linux/$(ARCH)-Ubsan-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config UBSAN \
			--parallel $(NPROC)

linux-clean-all: check-dir linux-clean-debug linux-clean-release linux-clean-releasedbg \
	linux-clean-public linux-clean-asan linux-clean-ubsan

linux-clean-asan: linux-cmake-asan
	cd tmp/Linux/$(ARCH)-Asan-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config ASAN \
			--target clean

linux-clean-debug: linux-cmake-debug
	cd tmp/Linux/$(ARCH)-Debug-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config DEBUG \
			--target clean

linux-clean-public: linux-cmake-public
	cd tmp/Linux/$(ARCH)-Public-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config MINSIZEREL \
			--target clean

linux-clean-release: linux-cmake-release
	cd tmp/Linux/$(ARCH)-Release-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config RELEASE \
			--target clean

linux-clean-releasedbg: linux-cmake-releasedbg
	cd tmp/Linux/$(ARCH)-ReleaseDbg-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config RELWITHDEBINFO \
			--target clean

linux-clean-ubsan: linux-cmake-ubsan
	cd tmp/Linux/$(ARCH)-Ubsan-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config UBSAN \
			--target clean

linux-cmake-asan: check-toolchain
	@mkdir -p tmp/Linux/$(ARCH)-Asan-$(TOOLCHAIN)
	cd tmp/Linux/$(ARCH)-Asan-$(TOOLCHAIN) && \
		$(CMAKE) -G "Unix Makefiles" \
			-DCMAKE_EXPORT_COMPILE_COMMANDS:STRING=ON \
			-DCMAKE_BUILD_TYPE:STRING=ASAN \
			-DCMAKE_TOOLCHAIN_FILE:STRING=../../../build/CMake/Toolchains/$(TOOLCHAIN).cmake \
			-DFUSION_BUILD_ROOT:STRING=$(PWD)/tmp \
			../../../

linux-cmake-debug: check-toolchain
	@mkdir -p tmp/Linux/$(ARCH)-Debug-$(TOOLCHAIN)
	cd tmp/Linux/$(ARCH)-Debug-$(TOOLCHAIN) && \
		$(CMAKE) -G "Unix Makefiles" \
			-DCMAKE_EXPORT_COMPILE_COMMANDS:STRING=ON \
			-DCMAKE_BUILD_TYPE:STRING=Debug \
			-DCMAKE_TOOLCHAIN_FILE:STRING=../../../build/CMake/Toolchains/$(TOOLCHAIN).cmake \
			-DFUSION_BUILD_ROOT:STRING=$(PWD)/tmp \
			../../../

linux-cmake-public: check-toolchain
	@mkdir -p tmp/Linux/$(ARCH)-Public-$(TOOLCHAIN)
	cd tmp/Linux/$(ARCH)-Public-$(TOOLCHAIN) && \
		$(CMAKE) -G "Unix Makefiles" \
			-DCMAKE_EXPORT_COMPILE_COMMANDS:STRING=ON \
			-DCMAKE_BUILD_TYPE:STRING=MINSIZEREL \
			-DCMAKE_TOOLCHAIN_FILE:STRING=../../../build/CMake/Toolchains/$(TOOLCHAIN).cmake \
			-DFUSION_BUILD_ROOT:STRING=$(PWD)/tmp \
			../../../

linux-cmake-release: check-toolchain
	@mkdir -p tmp/Linux/$(ARCH)-Release-$(TOOLCHAIN)
	cd tmp/Linux/$(ARCH)-Release-$(TOOLCHAIN) && \
		$(CMAKE) -G "Unix Makefiles" \
			-DCMAKE_EXPORT_COMPILE_COMMANDS:STRING=ON \
			-DCMAKE_BUILD_TYPE:STRING=RELEASE \
			-DCMAKE_TOOLCHAIN_FILE:STRING=../../../build/CMake/Toolchains/$(TOOLCHAIN).cmake \
			-DFUSION_BUILD_ROOT:STRING=$(PWD)/tmp \
			../../../

linux-cmake-releasedbg: check-toolchain
	@mkdir -p tmp/Linux/$(ARCH)-ReleaseDbg-$(TOOLCHAIN)
	cd tmp/Linux/$(ARCH)-ReleaseDbg-$(TOOLCHAIN) && \
		$(CMAKE) -G "Unix Makefiles" \
			-DCMAKE_EXPORT_COMPILE_COMMANDS:STRING=ON \
			-DCMAKE_BUILD_TYPE:STRING=RELWITHDEBINFO \
			-DCMAKE_TOOLCHAIN_FILE:STRING=../../../build/CMake/Toolchains/$(TOOLCHAIN).cmake \
			-DFUSION_BUILD_ROOT:STRING=$(PWD)/tmp \
			../../../

linux-cmake-ubsan: check-toolchain
	@mkdir -p tmp/Linux/$(ARCH)-Ubsan-$(TOOLCHAIN)
	cd tmp/Linux/$(ARCH)-Ubsan-$(TOOLCHAIN) && \
		$(CMAKE) -G "Unix Makefiles" \
			-DCMAKE_EXPORT_COMPILE_COMMANDS:STRING=ON \
			-DCMAKE_BUILD_TYPE:STRING=UBSAN \
			-DCMAKE_TOOLCHAIN_FILE:STRING=../../../build/CMake/Toolchains/$(TOOLCHAIN).cmake \
			-DFUSION_BUILD_ROOT:STRING=$(PWD)/tmp \
			../../../

#
# OSX Build Targets
#
osx-build-all: linux-build-debug

osx-build-debug: osx-cmake-debug
	cd tmp/OSX/$(ARCH)-Debug-$(TOOLCHAIN) && \
		$(CMAKE) \
			--build . \
			--config DEBUG \
			--parallel $(NPROC)

osx-cmake-debug: check-toolchain
	@mkdir -p tmp/OSX/$(ARCH)-Debug-$(TOOLCHAIN)
	cd tmp/OSX/$(ARCH)-Debug-$(TOOLCHAIN) && \
		$(CMAKE) -G "Unix Makefiles" \
			-DCMAKE_EXPORT_COMPILE_COMMANDS:STRING=ON \
			-DCMAKE_BUILD_TYPE:STRING=Debug \
			-DCMAKE_TOOLCHAIN_FILE:STRING=../../../build/CMake/Toolchains/$(TOOLCHAIN).cmake \
			-DFUSION_BUILD_ROOT:STRING=$(PWD)/tmp \
			../../../

#
# Windows specific build targets
#
windows-build: windows-build-debug windows-build-public windows-build-asan

windows-build-all: windows-build-debug windows-build-release windows-build-asan \
	windows-build-public windows-build-releasedbg

windows-build-asan: windows-proj
	@echo "Building CMake project for 'asan' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config ASAN \
			--parallel $(NPROC)

windows-build-debug: windows-proj
	@echo "Building CMake project for 'debug' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config DEBUG \
			--parallel $(NPROC)

windows-build-public: windows-proj
	@echo "Building CMake project for 'public' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config MINSIZEREL \
			--parallel $(NPROC)

windows-build-release: windows-proj
	@echo "Building CMake project for 'release' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config RELEASE \
			--parallel $(NPROC)

windows-build-releasedbg: windows-proj
	@echo "Building CMake project for 'release-dbg' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config RELWITHDEBINFO \
			--parallel $(NPROC)

windows-check-dir: check-prereqs check-dir
	@if [ ! -d tmp/VisualStudio ]; then \
		mkdir tmp/VisualStudio || exit 1; \
	fi

windows-clean-all: windows-clean-asan windows-clean-debug windows-clean-release \
	windows-clean-public windows-clean-releasedbg

windows-clean-asan: windows-proj
	@echo "Cleaning generated files for 'asan' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config ASAN \
			--target clean

windows-clean-debug: windows-proj
	@echo "Cleaning generated files for 'debug' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config DEBUG \
			--target clean

windows-clean-public: windows-proj
	@echo "Cleaning generated files for 'public' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config MINSIZEREL \
			--target clean

windows-clean-release: windows-proj
	@echo "Cleaning generated files for 'release' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config RELEASE \
			--target clean

windows-clean-releasedbg: windows-proj
	@echo "Cleaning generated files for 'release-dbg' on $(OS_NAME)"
	cd tmp/VisualStudio && \
		$(CMAKE) \
			--build . \
			--config RELWITHDEBINFO \
			--target clean

windows-proj: windows-check-dir
	cd tmp/VisualStudio && \
		$(CMAKE) -G "Visual Studio 17 2022" \
			-DFUSION_BUILD_ROOT:STRING=$(PWD)/tmp \
			../../

