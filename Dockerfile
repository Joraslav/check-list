FROM ubuntu:24.04 AS build

RUN apt-get update && \
    apt-get install -y \
    software-properties-common \
    apt-transport-https \
    ca-certificates \
    gnupg \
    lsb-release \
    wget \
    && \
    apt-get install -y \
    python3-pip \
    python3-full \
    pipx \
    git \
    && \
    # Установка последней версии CMake
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
    apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" && \
    apt-get update && \
    apt-get install -y cmake && \
    pipx install conan==2.22.2 && \
    pipx ensurepath && \
    # Проверка установленной версии CMake
    cmake --version

ENV PATH="/root/.local/bin:${PATH}"

RUN conan profile detect --force && \
    conan profile update settings.compiler.cppstd=23 default

COPY conanfile.txt /app/
RUN mkdir /app/build && cd /app/build && \
    conan install .. --build=missing -pr default
    
# Папка data больше не нужна
COPY ./src /app/src
COPY ./tests /app/tests
COPY CMakeLists.txt /app/
COPY tests/CMakeLists.txt /app/tests/

RUN cd /app/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON -DDEV_MODE=OFF -DCMAKE_TOOLCHAIN_FILE=/app/build/Release/conan_toolchain.cmake .. && \
    cmake --build .

RUN ctest --test-dir /app/build -C Release