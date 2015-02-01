#!/bin/sh

../../vsrpc_idl.sh \
  --input-file ../vstcpd/vstcpd.vsidl \
  --client-out-dir client

make all -j4

