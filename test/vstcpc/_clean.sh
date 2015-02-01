#! /bin/sh

make clean

../../vsrpc_idl.sh \
  --input-file ../vstcpd/vstcpd.vsidl \
  --client-out-dir client \
  --clean

rm -rf client

