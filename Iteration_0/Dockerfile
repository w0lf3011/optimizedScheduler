# Utiliser une image de base Ubuntu
FROM ubuntu:24.04

# Set the environment to avoid interactive prompts during apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Installer les dépendances de base
RUN apt-get update && apt-get install -y \
    make \
    gcc \
    build-essential \
    libc-dev \
    && rm -rf /var/lib/apt/lists/*

# Reset DEBIAN_FRONTEND to interactive
ENV DEBIAN_FRONTEND=dialog



# Créer un dossier de travail pour le code
WORKDIR /workspace

# Copier le code source de l'itération actuelle dans le conteneur
COPY . .

# Compiler le code du main.c du dossier courrant
RUN gcc -o iteration0 main.c

# Launch command
#CMD ["/bin/bash"]
CMD ["./iteration0"]
