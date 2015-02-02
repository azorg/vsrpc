#!/bin/sh

../../vsrpc_idl.sh \
  --input-file ../vstcpd/rpc.vsidl \
  --client-out-dir client \
  --fn-prefix rpc_

make all -j4

