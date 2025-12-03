FROM ubuntu:24.04 AS build

RUN apt-get update && \
    apt-get install -y \
    software-properties-common \
    apt-transport-https \
    ca-certificates \
    gnupg \
    lsb-release \
    wget

RUN apt-get install -y \
    python3-pip \
    python3-full \
    pipx \
    git \
    && \
    rm -rf /var/lib/apt/lists/*

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
    apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" && \
    apt-get update && \
    apt-get install -y cmake && \
    rm -rf /var/lib/apt/lists/*

RUN pipx install conan==2.22.2 && \
    pipx ensurepath

ENV PATH="/root/.local/bin:${PATH}"

COPY conanfile.txt /app/
COPY CMakeLists.txt /app/
COPY /profiles /app/profiles
COPY ./scripts /app/scripts
COPY ./src /app/src
COPY ./tests /app/tests
COPY tests/CMakeLists.txt /app/tests/
COPY LICENSE /app/LICENSE
COPY README.md /app/README.md

WORKDIR /app

RUN chmod +x scripts/create_env.sh && chmod +x scripts/build.sh
RUN ./scripts/create_env.sh Release
RUN ./scripts/build.sh

FROM build AS test
RUN ctest --test-dir build/Release --output-on-failure

FROM ubuntu:24.04 AS production

RUN apt-get update && \
    apt-get install -y \
    ca-certificates \
    && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /app/build/Release/todo /usr/local/bin/todo
COPY --from=build /app/build/Release/tg-bot /usr/local/bin/tg-bot

CMD ["todo"]