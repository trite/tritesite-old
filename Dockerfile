# FROM ocaml/opam
# # WORKDIR /app
# # COPY . .
# RUN make init
# CMD ["make", "serve"]

# FROM ocaml/opam
FROM ocaml/opam:alpine-3.17-ocaml-4.14

# RUN sudo curl -fsSL https://deb.nodesource.com/setup_18.x | sudo bash - && \
#   sudo apt-get install -y nodejs


RUN sudo apk add --update nodejs npm

WORKDIR /app
COPY . .
RUN opam init --compiler=4.14.0
RUN make init
RUN make build
CMD ["make serve"]

# CMD ["node", "--version"]
# RUN npm --version

# RUN cat /etc/os-release

# RUN lsb_release -a

# RUN hostnamectl

# RUN uname -r

# RUN apt-get update && \
#   apt-get -y install sudo

# RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo

# USER docker

# RUN sudo apt-get update && sudo apt-get list

# ENV NODE_ENV=development
# ENV NODE_VERSION=18.12.0
# RUN apt install -y curl
# RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.3/install.sh | bash
# ENV NVM_DIR=/root/.nvm
# RUN . "$NVM_DIR/nvm.sh" && nvm install ${NODE_VERSION}
# RUN . "$NVM_DIR/nvm.sh" && nvm use v${NODE_VERSION}
# RUN . "$NVM_DIR/nvm.sh" && nvm alias default v${NODE_VERSION}
# ENV PATH="/root/.nvm/versions/node/v${NODE_VERSION}/bin/:${PATH}"
# RUN node --version
# RUN npm --version



# WORKDIR /usr/src/app
# COPY ["package.json", "package-lock.json*", "npm-shrinkwrap.json*", "./"]
# RUN npm install --production --silent && mv node_modules ../
# COPY . .
# EXPOSE 8080
# RUN chown -R node /usr/src/app
# USER node
# CMD ["npm", "start"]
