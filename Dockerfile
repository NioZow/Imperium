FROM nixos/nix:latest

RUN echo "experimental-features = nix-command flakes" >> /etc/nix/nix.conf

WORKDIR /workspace

COPY flake.nix flake.lock ./

COPY . .

RUN nix develop --command true

CMD ["nix", "develop"]

