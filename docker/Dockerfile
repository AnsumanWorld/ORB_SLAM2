FROM ubuntu:17.10
COPY scripts/linux /tmp
RUN /tmp/bootstrap.sh \
    && rm -rf /root/packages \
    && apt-get -y autoremove \
    && apt-get -y autoclean \
    && rm -rf /var/lib/apt/lists/*
VOLUME /root/orb-slam2
