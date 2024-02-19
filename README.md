# A Perpetual [Fusion](https://perpetualfusion.io)

[![Apache Licensed](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](./LICENSE)

## NOTICE

This repository has been archived in favor of the [Perpetual Fusion Organization](https://github.com/perpetualfusion/fusion). Future work will continue there.

## Introduction

Fusion started as an educational project in early 2015 as I explored what C++ looked like in the Games Industry. It has evolved a lot over time but the Core mission remains the same: provide a common library for bootstrapping C/C++ based projects quickly and in any language that supports C/C++.

What makes Fusion unique from the tens of thousands of attempts at a low-level bootstrapping library? It follows these principles:

1. Provide a C and C++ interface for all functionality.
2. The C++ STL is optional, and only required for the C++ interface.
3. The C interface is POD-clean and uses only C++ compiler-intrinsic features.
4. Use the Rust ecosystem where C/C++ fail to provide reliable standards.

Overtime I will be adding more and more scaffolding to Fusion and eventually it will be too big to be a single library. The plan is already in place to split components as they grow too large. The goal is to never be a boost but provide everything an engineer may need to quickly get off the ground on a project.

# Requirements

The Fusion library is built and tested on the following:
1. Windows 10, 11
2. Fedora Workstation 35, 36
3. CentOS Stream 8

Compilers:
1. [Visual Studio](https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B) 2019, 2022
2. GCC 10, 12
3. Clang 14

Rust Toolchain:
1. Cargo - 1.61.x
2. Rust - 1.61.x

A Builder for the library may be run via Docker using the [Builder Toolchain](./build/Dockerfile).

# Low and High Level APIs

The library provides access to two main interfaces via a C-style and a modern C++{14,17,20} interface.

The following CMake variables will toggle the interfaces:

To toggle the CXX interface set the following variable when running CMake. It is `ON` by default.
> FUSION_PUBLIC_CXX_INTERFACE

To toggle the C-style interface set the following variable when running CMake. It is `ON` by default.
> FUSION_PUBLIC_C_INTERFACE

The CMake process will detect the best possible C++ version to use between 14, 17, and 20 based on the compiler.

# Building

For more information see [build/README.md](./build/README.md).

The quickest way to get started is with the [Makefile](./Makefile). On Windows you can generate a solution for Visual Studio using the `proj` target:
> make proj

The [Makefile](./Makefile) will allow you to trigger all of the different build configurations.

The following table shows the build targets and what gets built by default with each:

|Target| Asan| Debug|Public|Release|ReleaseDebug|Ubsan |
|:-----|:---:|:----:|:----:|:-----:|:----------:|:----:|
|all|X|X|X|X|X|X|
|build|X|X|X|-|-|-|
|build-all|X|X|X|X|X|X|

> Note: Windows currently only supports ASAN (Address Sanitizer) and will not support Ubsan (Undefined Behavior) sanitzer at all.

## Libraries

The following are the libraries and projects targets provided by Fusion. Some of the libraries provide their own bindings in supported languages.

|Library|CMake Target|Bindings|Description|
|:------|:----------:|:------:|:----------|
|[Core](./libraries/core/README.md)|[Fusion::Core](./libraries/core/source/CMakeLists.txt#21)|[Python](./libraries/core/bindings/python/README.md)|The Core Fusion library is the primary interface and functionality provider. All common functionally lives here and is the only required base library.|

## Examples

See the [Examples](./examples/README.md) for more information on how to use Fusion and it's available tools.

# Contributing

Contribute to the Fusion Project via Pull Requests.

# License

The [Apache 2.0](https://choosealicense.com/licenses/apache-2.0/) license was chosen to allow the most possible freedom of use while also requiring credit to be kept in place to myself and anyone who freely contributes back to the project.

This project is maintained as a side project and supported by my own infrastructure for testing. If you like it and choose to use it. Please let me know!
