#!/bin/bash

RUNTIME=$(which podman)
if [ -z "${RUNTIME}" ]; then
    RUNTIME=$(which docker)
fi
if [ -z "${RUNTIME}" ]; then
    echo "Cannot find podman or docker installed"
    exit 1
fi

${RUNTIME} run --rm -it \
    -v ${PWD}:/build:Z \
    -w /build \
    boringssl-builder:latest \
    python3 src/util/generate_build_files.py cmake

# Cleanup the go.sum file afterwards to prevent git from thinking
# there are available changes in the src directory.
cd src && git checkout go.sum
