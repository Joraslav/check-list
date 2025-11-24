# Не просто создаём образ, но даём ему имя build
FROM gcc:13.2 AS build

RUN apt-get update && \
    apt-get install -y \
      python3-pip \
      python3-full \
      pipx \
      cmake \
      git \
      apt-transport-https \
      ca-certificates \
      gnupg \
      lsb-release \
    && \
    pipx install conan==2.22.2 && \
    pipx ensurepath

# Запуск conan как раньше
COPY conanfile.txt /app/
RUN mkdir /app/build && cd /app/build && \
    conan profile detect --force && \
    conan install .. --build=missing -pr default

# Папка data больше не нужна
COPY ./src /app/src
COPY ./tests /app/tests
COPY CMakeLists.txt /app/
COPY tests/CMakeLists.txt /app/tests/

RUN cd /app/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON -DDEV_MODE=OFF .. && \
    cmake --build .

RUN ctest --test-dir /app/build -C Release