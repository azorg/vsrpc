#!/bin/sh

../../vsrpc_idl.sh \
  --input-file ../vstcpd/rpc.vsidl \
  --client-out-dir client \
  --fn-prefix      rpc_ \
  --remote-suffix  _remote

test `uname` = Linux && OPT='-j4'
make ${OPT}

