FROM alpine:3.15.10
RUN apk update && apk add linux-headers linux-virt-dev tpm2-tss-dev gcc g++ cmake make protobuf-dev abseil-cpp protobuf pkgconf

RUN mkdir -p /rmasmoke
WORKDIR /rmasmoke
