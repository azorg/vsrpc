#!/bin/sh

../../vsrpc_idl.sh \
  --input-file ../vstcpd/rpc.vsidl \
  --client-out-dir client \
  --fn-prefix      rpc_ \
  --remote-suffix  _remote

../../vsrpc_idl.sh \
  --input-file lrpc.vsidl \
  --server-out-dir server \
  --fn-prefix      lrpc_ \
  --remote-suffix  _remote \
  --remote-suffix  _local

test `uname` = Linux && OPT='-j4' || OPT='WIN32=1'
make ${OPT}

