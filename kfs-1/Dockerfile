FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install build tools (no multiarch on arm64) and GRUB utils for ISO
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        bison \
        flex \
        nasm \
        xorriso \
        grub-common \
        grub-pc-bin \
        mtools \
        qemu-system-x86 \
        wget \
        curl \
        git \
        libncurses-dev \
        libssl-dev \
        libelf-dev \
        ca-certificates \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install i386 cross toolchain (works on arm64 host)
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        binutils-i686-linux-gnu \
        gcc-i686-linux-gnu \
        g++-i686-linux-gnu \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set environment for cross-compilation
ENV CC=i686-linux-gnu-gcc
ENV CXX=i686-linux-gnu-g++
ENV LD=i686-linux-gnu-ld
ENV AS=i686-linux-gnu-as
ENV AR=i686-linux-gnu-ar
ENV NM=i686-linux-gnu-nm
ENV OBJDUMP=i686-linux-gnu-objdump
ENV OBJCOPY=i686-linux-gnu-objcopy

WORKDIR /src

CMD ["/bin/bash"]
