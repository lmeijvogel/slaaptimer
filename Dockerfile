FROM ubuntu:focal-20210609

RUN apt-get update
RUN apt-get install -y arduino-core \
                       build-essential \
                       python3 \
                       python3-serial

WORKDIR /app/arduino

CMD make
