#! /bin/sh

../../vsrpc_idl.sh \
  --input-file rpc.vsidl \
  --server-out-dir server \
  --fn-prefix rpc_

test `uname` = Linux && OPT='-j4'
make ${OPT}

