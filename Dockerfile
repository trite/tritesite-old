FROM ocaml/opam:alpine-3.17-ocaml-4.14

RUN sudo apk add --update nodejs npm
RUN npm update -g

# For bisect-ppx to build properly
RUN sudo npm install -g esy

# bisect-ppx again (shasum comes from perl-utils on alpine linux)
RUN sudo apk add perl-utils

WORKDIR /app
COPY . .

RUN opam init --compiler=4.14.1
# RUN make init
# Doing these steps individually so they can be cached (especially the opam switch one)
RUN opam switch create . 4.14.1 -y --deps-only
RUN eval $(opam env)
RUN npm install --legacy-peer-deps
RUN opam update
RUN opam install -y . --deps-only
RUN opam pin -y add tritesite.dev .
RUN make build

CMD ["make serve"]
