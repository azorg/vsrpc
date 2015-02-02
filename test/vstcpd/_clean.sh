#! /bin/sh

make clean

../../vsrpc_idl.sh \
  --input-file rpc.vsidl \
  --server-out-dir server \
  --clean

