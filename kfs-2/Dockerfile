FROM debian:bookworm

# ------------------------------
# Install dependencies
# ------------------------------
RUN apt-get update && apt-get install -y \
    build-essential \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    wget \
    curl \
    xz-utils \
    git \
    && rm -rf /var/lib/apt/lists/*

# ------------------------------
# Set versions
# ------------------------------
ENV BINUTILS_VERSION=2.42
ENV GCC_VERSION=14.1.0
ENV TARGET=i686-elf
ENV PREFIX=/opt/cross
ENV PATH="$PREFIX/bin:$PATH"

# ------------------------------
# Download sources
# ------------------------------
WORKDIR /tmp

RUN wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz && \
    wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz && \
    tar -xf binutils-$BINUTILS_VERSION.tar.xz && \
    tar -xf gcc-$GCC_VERSION.tar.xz

# ------------------------------
# Build binutils
# ------------------------------
RUN mkdir build-binutils && cd build-binutils && \
    ../binutils-$BINUTILS_VERSION/configure \
        --target=$TARGET \
        --prefix=$PREFIX \
        --with-sysroot \
        --disable-nls \
        --disable-werror && \
    make -j$(nproc) && \
    make install

# ------------------------------
# Build GCC (C only)
# ------------------------------
RUN mkdir build-gcc && cd build-gcc && \
    ../gcc-$GCC_VERSION/configure \
        --target=$TARGET \
        --prefix=$PREFIX \
        --disable-nls \
        --enable-languages=c \
        --without-headers \
        --disable-shared \
        --disable-threads \
        --disable-multilib && \
    make -j$(nproc) all-gcc && \
    make -j$(nproc) all-target-libgcc && \
    make install-gcc && \
    make install-target-libgcc

# ------------------------------
# Clean up
# ------------------------------
RUN rm -rf /tmp/*

# ------------------------------
# Export PATH
# ------------------------------
ENV PATH="$PREFIX/bin:$PATH"

CMD ["/bin/bash"]
