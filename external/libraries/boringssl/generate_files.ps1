docker run --rm -it `
    -v ${PWD}:/build `
    -w /build `
    boringssl-builder:latest `
    python3 src/util/generate_build_files.py cmake
