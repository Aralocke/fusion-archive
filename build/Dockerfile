FROM fedora:36

LABEL maintainer="Daniel Weiner <info@phantomnet.net>"

RUN set -ex; \
    mkdir -p /usr/src; \
    dnf update -y;

RUN set -ex; \
    dnf install --setopt install_weak_deps=false -y \
        cargo clang cmake cpp gdb gdb-headless llvm make \
        python3 python3-devel python3-pip rust rust-std-static

RUN set -ex; \
    dnf clean all;

USER nobody
WORKDIR /usr/src
