# syntax=docker/dockerfile:1.7

FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG CMAKE_VERSION=4.2.1
ARG NODE_VERSION=26.7.0
ARG PNPM_VERSION=11.20.0
ARG PYTHON_VERSION=3.13
ARG JAVA_VERSION=21

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

LABEL org.opencontainers.image.title="Stationeers IC10 CI Linux"
LABEL org.opencontainers.image.description="Linux CI environment for C++/Node/Python/Java/VSCode builds"

ENV HOME=/root

# 基础工具，对应 setup-cpp 的 Linux 部分
RUN apt-get update && apt-get install -y --no-install-recommends \
        software-properties-common \
        ca-certificates \
        curl \
        wget \
        git \
        xz-utils \
        unzip \
        zip \
        build-essential \
        pkg-config \
        ninja-build \
        ccache \
        lld \
    && rm -rf /var/lib/apt/lists/*

# 工具链、Python、Java
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y \
    && add-apt-repository ppa:deadsnakes/ppa -y \
    && apt-get update && apt-get install -y --no-install-recommends \
        g++-15 \
        gcc-15 \
        python${PYTHON_VERSION} \
        python${PYTHON_VERSION}-venv \
        python${PYTHON_VERSION}-dev \
        openjdk-${JAVA_VERSION}-jdk \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 100 \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-15 100 \
    && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-15 100 \
    && update-alternatives --install /usr/bin/python3 python3 /usr/bin/python${PYTHON_VERSION} 100 \
    && rm -rf /var/lib/apt/lists/*

# pip
RUN curl -fsSL https://bootstrap.pypa.io/get-pip.py -o /tmp/get-pip.py \
    && python3 /tmp/get-pip.py \
    && rm -f /tmp/get-pip.py

# CMake，对应 lukka/get-cmake 的 cmake-version
RUN curl -fsSL "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz" -o /tmp/cmake.tar.gz \
    && tar -xzf /tmp/cmake.tar.gz -C /opt \
    && ln -sf /opt/cmake-${CMAKE_VERSION}-linux-x86_64/bin/cmake /usr/local/bin/cmake \
    && ln -sf /opt/cmake-${CMAKE_VERSION}-linux-x86_64/bin/ctest /usr/local/bin/ctest \
    && ln -sf /opt/cmake-${CMAKE_VERSION}-linux-x86_64/bin/cpack /usr/local/bin/cpack \
    && rm -f /tmp/cmake.tar.gz

# Node.js + pnpm + VSCE，对应 setup-node 和 VSCode 扩展打包
RUN curl -fsSL "https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz" -o /tmp/node.tar.xz \
    && tar -xJf /tmp/node.tar.xz -C /usr/local --strip-components=1 \
    && rm -f /tmp/node.tar.xz \
    && npm install -g pnpm@${PNPM_VERSION} @vscode/vsce

# 全局安装 node-gyp，并预下载当前 Node 版本的头文件
RUN npm install -g node-gyp \
   && node-gyp install \
   && ls -la /root/.cache/node-gyp/${NODE_VERSION}/include/node/node.h

# Java 环境
ENV JAVA_HOME=/usr/lib/jvm/java-21-openjdk-amd64
ENV PATH="${JAVA_HOME}/bin:${PATH}"

# Python
ENV PYTHONUNBUFFERED=1

# ccache，对应 setup-cpp 中的缓存配置
ENV CCACHE_DIR=/root/.ccache
ENV CCACHE_MAXSIZE=2G
ENV CCACHE_COMPRESS=true

RUN ccache --set-config=max_size=2G \
    && ccache --set-config=compression=true \
    && ccache --zero-stats

WORKDIR /workspace

# 对应你 workflow 中的 vars，可按实际仓库变量调整
ENV STATIONEERS_CODE_DIR=code \
    STATIONEERS_ARTIFACTS_DIR=artifacts \
    STATIONEERS_DEP_CACHE_DIR=.deps

# 验证工具链
RUN cmake --version \
    && ninja --version \
    && gcc --version | head -1 \
    && g++ --version | head -1 \
    && ccache --version | head -1 \
    && node --version \
    && npm --version \
    && pnpm --version \
    && python3 --version \
    && java -version \
    && javac -version \
    && vsce --version

CMD ["/bin/bash"]