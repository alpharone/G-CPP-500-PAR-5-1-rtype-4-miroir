FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential cmake git \
    libx11-dev libxrandr-dev libxi-dev libxcursor-dev libudev-dev \
    libgl1-mesa-dev libopenal-dev libvorbis-dev libflac-dev \
    x11-apps mesa-utils && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -S . -B build -DBUILD_TESTS=OFF -DBUILD_PLUGINS=ON && \
    cmake --build build -j$(nproc)
