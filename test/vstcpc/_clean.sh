#! /bin/sh

make clean

../../vsrpc_idl.sh \
  --input-file ../vstcpd/rpc.vsidl \
  --client-out-dir client \
  --clean

