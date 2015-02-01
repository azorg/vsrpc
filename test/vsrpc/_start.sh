#!/bin/sh

if true
then
  bash ../../vsrpc_idl.sh \
    --input-file prj.vsidl \
    --server-out-dir server \
    --client-out-dir client \
    --examples-out-dir examples
fi

CFLAGS="-O0 -g -fomit-frame-pointer -I../../ -I../../socklib -Iserver -Iclient -Wall -pipe"
LDFLAGS="-lrt"
GCC=gcc
LD=gcc

${GCC} ${CFLAGS} -c ../../vsrpc.c || exit
${GCC} ${CFLAGS} -c ../../socklib/socklib.c || exit

${GCC} ${CFLAGS} -c examples/prj_server_example.c || exit
${GCC} ${CFLAGS} -c server/prj_server.c || exit
${GCC} ${CFLAGS} -c server/prj_common.c || exit
${LD} ${LDFLAGS} prj_server_example.o prj_common.o prj_server.o vsrpc.o socklib.o \
              -o prj_server_example || exit


${GCC} ${CFLAGS} -c examples/prj_client_example.c || exit
${GCC} ${CFLAGS} -c client/prj_client.c || exit
${GCC} ${CFLAGS} -c client/prj_wrap.c || exit
${LD} ${LDFLAGS} prj_client_example.o prj_client.o prj_wrap.o vsrpc.o \
                 socklib.o -o prj_client_example || exit

[ ! -e fifo ] && mkfifo fifo

cat fifo | ./prj_server_example | tee log2 | ./prj_client_example | tee log1 > fifo

#gprof prj_server_example > gprof_server.rpt
#gprof prj_client_example > gprof_client.rpt


