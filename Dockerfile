# Не просто создаём образ, но даём ему имя build
FROM gcc:13.2 as build

RUN apt update && \
    apt install -y \
      python3-pip \
      cmake \
    && \
    pip3 install conan==2.10.1

# Запуск conan как раньше
COPY conanfile.txt /app/
RUN mkdir /app/build && cd /app/build && \
    conan install .. --build=missing

# Папка data больше не нужна
COPY ./include /app/include
COPY ./src /app/src
COPY CMakeLists.txt /app/

RUN cd /app/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON -DDEV_MODE=OFF .. && \
    cmake --build .

RUN ctest --test-dir /app/build -C Release