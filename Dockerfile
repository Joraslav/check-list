# Не просто создаём образ, но даём ему имя build
FROM gcc:13.2 as build

RUN apt update && \
    apt install -y \
      python3-pip \
      cmake \
      git \
    && \
    pip3 install conan==2.22.2

# Запуск conan как раньше
COPY conanfile.txt /app/
RUN mkdir /app/build && cd /app/build && \
    conan install .. --build=missing

# Папка data больше не нужна
COPY ./src /app/src
COPY ./tests /app/tests
COPY CMakeLists.txt /app/
COPY tests/CMakeLists.txt /app/tests/

RUN cd /app/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON -DDEV_MODE=OFF .. && \
    cmake --build .

RUN ctest --test-dir /app/build -C Release