FROM nixos/nix:latest

# Enable flakes and nix-command
RUN echo "experimental-features = nix-command flakes" >> /etc/nix/nix.conf

# Set working directory
WORKDIR /workspace

# Copy your flake files
COPY flake.nix flake.lock ./

# Optional: Copy any other necessary files
COPY . .

# Pre-build the development environment to cache dependencies
RUN nix develop --command true

# Default command drops you into the nix develop shell
CMD ["nix", "develop"]

