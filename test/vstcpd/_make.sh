#! /bin/sh

../../vsrpc_idl.sh \
  --input-file vstcpd.vsidl \
  --server-out-dir server

make all -j4

