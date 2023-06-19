FROM debian:stable-slim

RUN set -ex; \
    apt-get update; \
    apt-get install --no-install-recommends -y \
        perl golang python3 cmake; \
    apt-get clean autoclean; \
    apt-get autoremove -y; \
    rm -rf /var/lib/{apt,dpkg,cache,log};

RUN set -ex; \
    mkdir -p /build;
