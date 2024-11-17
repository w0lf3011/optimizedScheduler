# Utiliser une image de base Ubuntu
FROM ubuntu:24.04


# Définir la version du compilateur et du SDK
ARG GCC_ARM_VERSION=10.3-2021.10
ARG PICO_SDK_VERSION=master

# Set the environment to avoid interactive prompts during apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Installer les dépendances de base
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    wget \
    build-essential \
    libusb-1.0-0-dev \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Reset DEBIAN_FRONTEND to interactive
ENV DEBIAN_FRONTEND=dialog

# Installer le compilateur ARM GCC pour le Raspberry Pi Pico
RUN wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/${GCC_ARM_VERSION}/gcc-arm-none-eabi-${GCC_ARM_VERSION}-x86_64-linux.tar.bz2 \
    && tar -xjf gcc-arm-none-eabi-${GCC_ARM_VERSION}-x86_64-linux.tar.bz2 -C /opt \
    && rm gcc-arm-none-eabi-${GCC_ARM_VERSION}-x86_64-linux.tar.bz2

# Ajouter le compilateur ARM au PATH
ENV PATH="/opt/gcc-arm-none-eabi-${GCC_ARM_VERSION}/bin:${PATH}"

# Cloner le SDK du Raspberry Pi Pico
RUN git clone -b ${PICO_SDK_VERSION} https://github.com/raspberrypi/pico-sdk.git /pico-sdk

# Définir le chemin du SDK Pico pour CMake
ENV PICO_SDK_PATH=/pico-sdk

# Configurer le SDK Pico
RUN mkdir -p /pico-sdk/build && cd /pico-sdk/build && cmake ..

# Créer un dossier de travail pour le code
WORKDIR /workspace

# Copier le code source de l'itération actuelle dans le conteneur
COPY . /workspace

# Créer un dossier de build
RUN mkdir -p build

# Configurer et compiler le code
WORKDIR /workspace/build
RUN cmake .. && make -j$(nproc)

# Commande par défaut pour exécuter un shell dans le conteneur (peut être ajusté pour exécuter le binaire si nécessaire)
CMD ["/bin/bash"]
