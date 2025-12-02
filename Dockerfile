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

WORKDIR /app

COPY conanfile.txt /app/
COPY /profiles /app/profiles
COPY ./scripts /app/scripts
RUN ./app/scripts/create_env.sh Release

COPY ./src /app/src
COPY ./tests /app/tests
COPY CMakeLists.txt /app/
COPY tests/CMakeLists.txt /app/tests/

RUN /app/scripts/build.sh

RUN ctest --test-dir build/Release