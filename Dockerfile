FROM ubuntu:latest AS upstream

ARG DEBIAN_FRONTEND=noninteractive

ARG CORE_VERSION=3.3.11

# arduino-cli warnings: none default more all
ARG WARNINGS=default

# Get curl and git
RUN apt-get update \
    && apt-get install -y curl git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Setup Arduino CLI
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Start config file
RUN arduino-cli config init --additional-urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json,https://espressif.github.io/arduino-esp32/package_esp32_dev_index.json

# Update core index
RUN arduino-cli core update-index

# Update library index
RUN arduino-cli lib update-index

# Install platform
RUN arduino-cli core install "esp32:esp32@${CORE_VERSION}"

# Enable external libs
RUN arduino-cli config set library.enable_unsafe_install true

# ===========================================================================================

# Copy source and build deployment image
FROM upstream AS deployment

# Create a folder for the library source
RUN cd /root \
    && mkdir Arduino \
    && cd Arduino \
    && mkdir libraries \
    && cd libraries \
    && mkdir SparkFun_u-blox_GNSS_v4 \
    && cd SparkFun_u-blox_GNSS_v4 \
    && mkdir src

# Copy the library source
COPY src /root/Arduino/libraries/SparkFun_u-blox_GNSS_v4/src

# Copy the example source file
COPY examples .

# Compile Sketch
RUN cd Example1_PositionVelocityTime \
    && arduino-cli compile --fqbn "esp32:esp32:esp32" \
    --warnings ${WARNINGS} \
    Example1_PositionVelocityTime.ino \
    --export-binaries

# ===========================================================================================

# Copy the compile output. List the files
FROM deployment AS output
# COPY --from=deployment ./build/esp32.esp32.esp32 /
CMD echo $(ls /*.*)
