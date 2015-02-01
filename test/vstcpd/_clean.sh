#! /bin/sh

make clean

../../vsrpc_idl.sh \
  --input-file vstcpd.vsidl \
  --server-out-dir server \
  --clean

