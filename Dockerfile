FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    lldb \
    gdb \
    make \
    git \
    pkg-config \
    readline-common \
    libreadline-dev \
    libncursesw5-dev \
    valgrind \
    vim \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /work
