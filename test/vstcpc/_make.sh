#!/bin/sh

../../vsrpc_idl.sh \
  --input-file ../vstcpd/rpc.vsidl \
  --client-out-dir client \
  --fn-prefix rpc_

test `uname` = Linux && OPT='-j4'
make ${OPT}

