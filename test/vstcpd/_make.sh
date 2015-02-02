#! /bin/sh

../../vsrpc_idl.sh \
  --input-file rpc.vsidl \
  --fn-prefix  rpc_ \
	--server-out-dir server

make all -j4

