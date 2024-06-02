#!/usr/bin/sh

openssl req -x509 -nodes -newkey rsa:4096 -keyout localhost.key -out localhost.cert -sha512
