FROM ubuntu:18.04
COPY scripts/linux /tmp
RUN DEBIAN_FRONTEND=noninteractive /tmp/bootstrap.sh \
    && rm -rf /root/packages \
    && apt-get -y autoremove \
    && apt-get -y autoclean \
    && rm -rf /var/lib/apt/lists/*
VOLUME /root/orb-slam2
