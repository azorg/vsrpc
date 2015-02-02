#! /bin/sh

../../vsrpc_idl.sh \
  --input-file rpc.vsidl \
  --fn-prefix  rpc_ \
	--server-out-dir server

test `uname` = Linux && OPT='-j4'
make ${OPT}

