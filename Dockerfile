# Build stage
FROM ubuntu:24.04 AS build

# First RUN: Prepare environment and install basic dependencies
RUN apt-get update && \
    apt-get install -y \
    software-properties-common \
    apt-transport-https \
    ca-certificates \
    gnupg \
    lsb-release \
    wget

# Second RUN: Install Python dependencies
RUN apt-get install -y \
    python3-pip \
    python3-full \
    pipx \
    git \
    && \
    rm -rf /var/lib/apt/lists/*

# Third RUN: Install CMake and set up Kitware repository
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
    apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" && \
    apt-get update && \
    apt-get install -y cmake && \
    rm -rf /var/lib/apt/lists/*

# Fourth RUN: Install Conan
RUN pipx install conan==2.22.2 && \
    pipx ensurepath

# Set environment variable for PATH
ENV PATH="/root/.local/bin:${PATH}"

# Copy dependency files first for better caching
COPY conanfile.txt /app/
COPY CMakeLists.txt /app/
COPY profiles /app/profiles

# Set working directory
WORKDIR /app

# Install Conan dependencies
RUN conan profile detect --force && \
    conan install . --build=missing -pr profiles/Release

# Copy source code after dependencies are installed
COPY scripts /app/scripts
COPY src /app/src
COPY tests /app/tests
COPY tests/CMakeLists.txt /app/tests/
COPY LICENSE /app/LICENSE
COPY README.md /app/README.md

# Make scripts executable
RUN chmod +x scripts/create_env.sh && chmod +x scripts/build.sh

# Set up environment and build
RUN ./scripts/create_env.sh Release && \
    ./scripts/build.sh

# Test stage
FROM build AS test
WORKDIR /app
# Set entrypoint to run tests by default
ENTRYPOINT ["ctest", "--test-dir", "build/Release", "--output-on-failure"]

# Production stage
FROM ubuntu:24.04 AS production

# Install only runtime dependencies
RUN apt-get update && \
    apt-get install -y \
    ca-certificates \
    && \
    rm -rf /var/lib/apt/lists/*

# Copy the built application from the build stage
COPY --from=build /app/build/Release/todo /usr/local/bin/todo
COPY --from=build /app/build/Release/tg-bot /usr/local/bin/tg-bot

# Set the default command
CMD ["todo"]