#!/usr/bin/sh

openssl req -x509 \
            -sha256 -days 356 \
            -nodes \
            -newkey rsa:2048 \
            -subj "/CN=localhost/C=RU/L=Saint Petersburg" \
            -keyout rootCA.key -out rootCA.crt

openssl genrsa -out localhost.key 2048
openssl req -new -key localhost.key -out localhost.csr
openssl x509 -req \
    -in localhost.csr \
    -CA rootCA.crt -CAkey rootCA.key \
    -CAcreateserial -out localhost.cert \
    -days 365 \
    -sha256 -extfile cert.conf