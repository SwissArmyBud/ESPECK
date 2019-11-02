
# 1024 or 512.  512 saves memory...
$BITS=512

echo "
[ req ]
distinguished_name = req_distinguished_name
prompt = no
[ req_distinguished_name ]
O = especk.com
CN = 127.0.0.1
" >> ./certs.conf

openssl genrsa -out ./tls.ca_key.pem $BITS
openssl genrsa -out ./tls.key_$BITS.pem $BITS
openssl rsa -in ./tls.key_$BITS.pem -out ./tls.key_$BITS -outform DER

openssl req -out ./tls.ca_x509.req -key ./tls.ca_key.pem -new -config ./certs.conf
openssl req -out ./tls.x509_$BITS.req -key ./tls.key_$BITS.pem -new -config ./certs.conf
openssl x509 -req -in ./tls.ca_x509.req -out ./tls.ca_x509.pem -sha256 -days 5000 -signkey ./tls.ca_key.pem
openssl x509 -req -in ./tls.x509_$BITS.req -out ./tls.x509_$BITS.pem -sha256 -CAcreateserial -days 5000 -CA ./tls.ca_x509.pem -CAkey ./tls.ca_key.pem
openssl x509 -in ./tls.ca_x509.pem -outform DER -out ./tls.ca_x509.cer
openssl x509 -in ./tls.x509_$BITS.pem -outform DER -out ./tls.x509_$BITS.cer

echo -e -n "\n" > ./x509keys.h
echo -e -n "#ifndef __ESP_509_KEYS_H\n" >> ./x509keys.h
echo -e -n "#define __ESP_509_KEYS_H\n" >> ./x509keys.h
echo -e -n "\n" >> ./x509keys.h
echo -e -n "#pragma once\n" >> ./x509keys.h
echo -e -n "\n" >> ./x509keys.h
echo -e -n "static const uint8_t x509[] PROGMEM = {\n" >> ./x509keys.h

hexdump -v -e '10/1 "0x%02X, " "\n"' ./tls.key_$BITS | sed 's/, 0x .*/\r/' | sed ':a;$!{N;s/,\n/,\n/g;ba;}' >> ./x509keys.h

echo -e -n "};\n" >> ./x509keys.h
echo -e -n "\n" >> ./x509keys.h
echo -e -n "static const uint8_t rsakey[] PROGMEM = {\n" >> ./x509keys.h

hexdump -v -e '10/1 "0x%02X, " "\n"' ./tls.x509_$BITS.cer  | sed 's/, 0x .*/\r/' | sed ':a;$!{N;s/,\n/,\n/g;ba;}' >> ./x509keys.h

echo -e -n "};\n" >> ./x509keys.h
echo -e -n "\n" >> ./x509keys.h
echo -e -n "#endif" >> ./x509keys.h

rm ./tls.*
rm ./certs.conf
