#!/bin/bash
#
# Very Simple Remote Procedure Call (VSRPC) project
# Version: 0.9
#

#----------------------------------------------------------------------------
# extension of input files
vsidl_ext="vsidl"

# suffixes for output files (server side):
local_h="_local.h"
local_c="_local.c"
server_h="_server.h"
server_c="_server.c"

# suffixes for output files (client side):
remote_h="_remote.h"
remote_c="_remote.c"
client_h="_client.h"
client_c="_client.c"
client_dbg_h="_client_dbg.h"

# suffixes for output examples
client_example_c="_client_example.c"
server_example_c="_server_example.c"

# suffix for name of vsrpc_func server structure
struct_suffix="_vsrpc_func"

# prefix for wrapper functions on server side
wrap_prefix="fn_"

# defaults:
input_file=""
server_out_dir="."
client_out_dir="."
examples_out_dir="."
server="no"
client="no"
examples="no"
clean="no"
fn_prefix=""
remote_suffix=""
local_suffix=""
#----------------------------------------------------------------------------
usage()
{
  echo
  echo "usage: `basename $0` [options]"
  echo "create C code of wrappers from *.vsidl files"
  echo
  echo "input options:"
  echo "  --input-file input_file  input *.vsidl file"
  echo
  echo "output options:"
  echo "  --server                 generate code for VSRPC server part"
  echo "  --client                 generate code for VSRPC client part"
  echo "  --examples               generate code for examples"
  echo "  --server-out-dir path    output directory for server code"
  echo "  --client-out-dir path    output directory for client code"
  echo "  --examples-out-dir path  output directory for examples"
  echo "                           by default all output directory is ."
  echo "  --fn-prefix preffix      common C funcion name prefix (like myrpc_)"
  echo "  --remote-suffix suffix   remote C funcion name suffix (like _remote)"
  echo "  --local-suffix suffix    local  C funcion name suffix (like _local)"
  echo
  echo "extra options:"
  echo "  -h | --help              show this help"
  echo "  --clean                  delete all autogenerated code safely"
  echo
}
#----------------------------------------------------------------------------
no_input_file()
{
  echo "error: no input VSIDL file" >&2
  usage >&2
  exit 1
}
#----------------------------------------------------------------------------
# test number of arguments
if [ $# -lt 1 ]
then
  echo "error: no arguments" >&2
  usage >&2
  exit 2
fi
#----------------------------------------------------------------------------
# parse options
while true
do
  case $1 in
            --input-file) input_file="$2"; shift 2 ;;
        --server-out-dir) server_out_dir="$2"; mkdir -p "$2";
                          server="yes"; shift 2 ;;
        --client-out-dir) client_out_dir="$2"; mkdir -p "$2";
                          client="yes"; shift 2 ;;
      --examples-out-dir) examples_out_dir="$2"; mkdir -p "$2";
                          examples="yes"; shift 2 ;;
             --fn-prefix) fn_prefix="$2"; shift 2 ;;
         --remote-suffix) remote_suffix="$2"; shift 2 ;;
          --local-suffix) local_suffix="$2";  shift 2 ;;
                --server) server="yes"; shift ;;
                --client) client="yes"; shift ;;
              --examples) examples="yes"; shift ;;
                 --clean) clean="yes"; shift ;;
               -h|--help) usage ; exit ;;
                       *) echo "error: bad arguments" >&2 ; usage >&2;
                          exit 2 ;;
  esac
  [ -z "$1" ] && break;
done
#----------------------------------------------------------------------------
# check input file
test -f "${input_file}" || no_input_file

# set ${prj} name
prj=`basename "${input_file}" .${vsidl_ext}`
file_name=`basename "${input_file}"`

# set prefix for wrapper functions on server side with ${prj} name
wrap_prefix="${prj}_${wrap_prefix}"
#----------------------------------------------------------------------------
# clean and exit
if [ "$clean" = "yes" ]
then
  if [ "$server" == "yes" ]
  then # clean server autogenerated code
    rm -f "${server_out_dir}/${prj}${local_h}"
   #rm -f "${server_out_dir}/${prj}${local_c}" don't remove!
    rm -f "${server_out_dir}/${prj}${server_h}"
    rm -f "${server_out_dir}/${prj}${server_c}"
  fi

  if [ "$client" == "yes" ]
  then # clean client autogenerated code
    rm -f "${client_out_dir}/${prj}${remote_h}"
    rm -f "${client_out_dir}/${prj}${remote_c}"
    rm -f "${client_out_dir}/${prj}${client_h}"
    rm -f "${client_out_dir}/${prj}${client_c}"
   #rm -f "${client_out_dir}/${prj}${client_dbg_h}" don't remove!
  fi

  if [ "$examples" == "yes" ]
  then # clean examples autogenerated code
    rm -f "${examples_out_dir}/${prj}${server_example_c}"
    rm -f "${examples_out_dir}/${prj}${client_example_c}"
  fi

  exit
fi
#----------------------------------------------------------------------------
# "horisontal line"
hr=\
"//----------------------------------------------------------------------------"

# current date
date=`date "+%x %X"`
#----------------------------------------------------------------------------
load_top()
{
# pattern which place at begin of ${prj}${local_h} file
local_h_top=`cat <<EOF
/*
 * File: "${prj}${local_h}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 */
${hr}
#include "vsrpc.h"
${hr}
EOF`

# pattern which place at begin of ${prj}${local_c} file
local_c_top=`cat <<EOF
/*
 * File: "${prj}${local_c}".
 * This file is autogenerated from "${file_name}" file
 * and can be append. You may (and must) edit this file.
 * Recomented backup copy this file to another too.
 */
${hr}
#include "vsrpc.h"
${hr}
EOF`

# pattern which place to ${prj}${server_h} file
server_h_top=`cat <<EOF
/*
 * File: "${prj}${server_h}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 */
${hr}
#include "vsrpc.h"
${hr}
extern vsrpc_func_t ${prj}${struct_suffix}[];
${hr}
EOF`

# pattern which place at begin of ${prj}${server_c} file
server_c_top=`cat <<EOF
/*
 * File: "${prj}${server_c}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 */
${hr}
#include <stdlib.h> // NULL
#include "${prj}${local_h}"
${hr}
EOF`

# pattern which place at begin of ${prj}${remote_h} file
remote_h_top=`cat <<EOF
/*
 * File: "${prj}${remote_h}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 */
${hr}
#include "vsrpc.h"
${hr}
EOF`

# pattern which place at begin of ${prj}${remote_c} file
remote_c_top=`cat <<EOF
/*
 * File: "${prj}${remote_c}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 */
$hr
#include "${prj}${client_dbg_h}" // VSRPC_ERROR_DBG()
#include "${prj}${client_h}" // ${fn_prefix}*_call(), ${fn_prefix}*_wait()
${hr}
EOF`


# pattern which place at begin of ${prj}${client_h} file
client_h_top=`cat <<EOF
/*
 * File: "${prj}${client_h}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 */
${hr}
#include "vsrpc.h"
${hr}
EOF`

# pattern which place at begin of ${prj}${client_c} file
client_c_top=`cat <<EOF
/*
 * File: "${prj}${client_c}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 */
${hr}
#include "vsrpc.h"
${hr}
EOF`

# pattern which place to ${prj}${client_dbg_h} file
client_dbg_h_top=`cat <<EOF
/*
 * File: "${prj}${client_dbg_h}".
 * This file was autogenerated from "${file_name}" file.
 * This file will NOT overwriten.
 * You may (and must) edit this file safely.
 */
$hr
#include <stdio.h> // fprintf()
#include "vsrpc.h" // vsrpc_error_str()
$hr
#define VSRPC_ERROR_DBG(err, rpc) fprintf(stderr, "VSRPC error %i: '%s'\n", err, vsrpc_error_str(err));
$hr
EOF`

} # load_top
#----------------------------------------------------------------------------
# some autogenerated examples
load_examples()
{
# example of "server" use
server_example=`cat <<EOF
/*
 * File: "${prj}${server_example_c}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 *
 * This is demo module.
 * Link this module with "${prj}${server_c}", "${prj}${local_c}",
 * "vsrpc.c" and "socklib.c".
 */
${hr}
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
${hr}
#include "socklib.h"
#include "vsrpc.h"
#include "${prj}${local_h}"
#include "${prj}${server_h}" // ${prj}${struct_suffix}[]
${hr}
// global VSRPC sructure
vsrpc_t rpc;
${hr}
// main function ;-)
int main()
{
  int retv;

  // socklib init
  sl_init();

  // init VSRPC structure
  vsrpc_init(
    &rpc,
    ${prj}${struct_suffix},
    (char** (*)(vsrpc_t*, int, char *const[])) NULL,
    VSRPC_PERM_ALL,
    (void*) NULL,  // context
    STDIN_FILENO,  // stdin
    STDOUT_FILENO, // stdout
    sl_read,
    sl_write,
    sl_select,
    (void (*)(int)) NULL); // flush()

  // run server
  retv = vsrpc_run_forever(&rpc);

  vsrpc_release(&rpc);
  return retv;
}
${hr}
EOF`

# example of "client" use
client_example=`cat <<EOF
/*
 * File: "${prj}${client_example_c}".
 * Do NOT edit this file!
 * This file is autogenerated from "${file_name}" file
 * on "${date}" and may be overwriten.
 * Please edit "${file_name}" file.
 *
 * This is demo module.
 * Link this module with "${prj}${remote_c}", "${prj}${client_c}",
 * "vsrpc.c", "socklib.c" and use "-lrt" linker option.
 */
${hr}
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
${hr}
#include "socklib.h"
#include "vsrpc.h"
#include "${prj}${remote_h}"
${hr}
double get_time()
{
  struct timespec ts;
  double t;
  clock_gettime(CLOCK_REALTIME, &ts);
  t  = ((double) ts.tv_nsec) * 1e-9;
  t += ((double) ts.tv_sec);
  return t;
}
${hr}
// global VSRPC sructure
vsrpc_t rpc;
${hr}
// main function ;-)
int main()
{
  int retv, ack;
  double t;

  // socklib init
  sl_init();

  // init VSRPC structure
  vsrpc_init(
    &rpc,
    (vsrpc_func_t*) NULL,
    (char** (*)(vsrpc_t*, int, char *const[])) NULL,
    VSRPC_PERM_DEFAULT,
    (void*) NULL,  // context
    STDIN_FILENO,  // stdin
    STDOUT_FILENO, // stdout
    sl_read,
    sl_write,
    (int (*)(int, int)) NULL, // select()
    (void (*)(int)) NULL);    // flush()

  // run some function
  t = get_time();
  vsrpc_check_version(&rpc, &ack);
  t = get_time() - t;
  fprintf(stderr, "\nversion_ok = %d\n", ack);
  fprintf(stderr, "version1_dt = %.9f\n", t);

  t = get_time();
  vsrpc_check_version(&rpc, &ack);
  t = get_time() - t;
  fprintf(stderr, "version2_dt = %.9f\n", t);

  t = get_time();
  vsrpc_check_version(&rpc, &ack);
  t = get_time() - t;
  fprintf(stderr, "version3_dt = %.9f\n", t);

  t = get_time();
  vsrpc_remote_ping(&rpc, &ack);
  t = get_time() - t;
  fprintf(stderr, "ping1_ack = %d\n", ack);
  fprintf(stderr, "ping1_dt = %.9f\n", t);

  t = get_time();
  vsrpc_remote_ping(&rpc, &ack);
  t = get_time() - t;
  fprintf(stderr, "ping2_dt = %.9f\n", t);

  t = get_time();
  vsrpc_remote_ping(&rpc, &ack);
  t = get_time() - t;
  fprintf(stderr, "ping3_dt = %.9f\n", t);

  //...
  //...

  retv = vsrpc_remote_exit(&rpc, 0);

  vsrpc_release(&rpc);
  return retv;
}
${hr}
EOF`
} # load_examples()
#----------------------------------------------------------------------------
load_functions()
{
# pattern of local prototypes (place to ${prj}${local_h} file)
local_proto="${RetType} ${ProcNameC}${local_suffix}(vsrpc_t*${ArgProto});"

# pattern of local function (add to ${prj}${local_c} file)
local_func=`cat <<EOF
${RetType} ${ProcNameC}${local_suffix}(vsrpc_t* rpc${ArgDesc})
{
  //...insert your code there...
  return${DefVal};
}
${hr}
EOF`

# pattern of wrapper on server side (place to *_server.c)
server_func=`cat <<EOF
char **${wrap_prefix}${ProcName}(vsrpc_t* rpc, int argc, char * const argv[])
{ // VSRPC wrapper on "server" side
  char **retv;${RetDesc}
${ArgDesc2}
  // check number of arguments
  if (argc != ${ArgNumP1}) return NULL;

  // call procedure
${ArgFill}  ${Ret2}${ProcNameC}${local_suffix}(rpc${ArgAll});

  // fill output array
  retv = vsrpc_list2argv("${RetList}"${ArgOut2});
${FreeStr}  return retv; // return values :-D
}
${hr}
EOF`

# pattern of remote prototypes (place to ${prj}${remote_h} file)
remote_proto="${RetType} ${ProcNameC}${remote_suffix}(vsrpc_t*${ArgProto});"

# pattern of wrapper on client side (place to ${prj}${remote_c} file)
remote_func=`cat <<EOF
${RetType} ${ProcNameC}${remote_suffix}(vsrpc_t* rpc${ArgDesc})
{ // VSRPC wrapper on "client" side
  int err;${RetDesc}

  // call remote procedure
  err = ${ProcNameC}_call(rpc${ArgIn});
  if (err != VSRPC_ERR_NONE)
  { VSRPC_ERROR_DBG(err, rpc) return${DefVal}; }

  // wait then remote procedure finish
  err = ${ProcNameC}_wait(rpc${ArgOut});
  if (err != VSRPC_ERR_NONE)
  { VSRPC_ERROR_DBG(err, rpc) return${DefVal}; }

  return${Ret1};
}
${hr}
EOF`

# patterns of wrappers prototypes (place to ${prj}${client_h} file)
client_proto=`cat <<EOF
int ${ProcNameC}_call(vsrpc_t*${ArgInProto});
int ${ProcNameC}_wait(vsrpc_t*${ArgOutProto});
EOF`

# patterns of wrapper on client side (place to ${prj}${client_c} file)
client_func=`cat <<EOF
int ${ProcNameC}_call(vsrpc_t* rpc${ArgInDesc})
{ // VSRPC wrapper on "client" side (call remote procedure)
  return vsrpc_call_ex(rpc, "${CallList}", "${ProcName}"${ArgIn});
}
${hr}
int ${ProcNameC}_wait(vsrpc_t* rpc${ArgOutDesc})
{ // VSRPC wrapper on "client" side (wait when remote procedure finish)
  int retv = vsrpc_wait(rpc);
  if (retv != VSRPC_ERR_NONE)
  {
    if (rpc->retc != 0)
    { vsrpc_free_argv(rpc->retv); rpc->retc = 0; }
  }
  else if (rpc->retc != ${RetNum}) retv = VSRPC_ERR_BARG;
  if (retv != VSRPC_ERR_NONE)
  {
${DefFill}    return retv;
  }
${RetFill}#if ${RetNum} != 0
  vsrpc_free_argv(rpc->retv); rpc->retc = 0;
#endif
  return VSRPC_ERR_NONE;
}
${hr}
EOF`
} # load_functions()
#----------------------------------------------------------------------------
get_type()
{ # convert symbol of type to C-type #1
  # (type: {i, f, d, s, I, F, D, S, v})
  echo $1 | sed 's/i/int/; s/f/float/; s/d/double/; s/s/char\*/;' | \
    sed 's/I/int*/; s/F/float*/; s/D/double*/; s/S/char\*\*/; s/v/void/'
}
#----------------------------------------------------------------------------
get_type2()
{ # convert symbol of type to C-type #2
  # (type: {I, F, D, S})
  echo $1 | sed 's/I/int/; s/F/float/; s/D/double/; s/S/char\*/'
}
#----------------------------------------------------------------------------
get_fill_ret()
{ # get sorce code for fill return value(s)
  # (type, arg_num, ret_num)
  case $1 in
    i) echo "a$2 = vsrpc_str2int(rpc->retv[$3]);" ;;
    f) echo "a$2 = (float) vsrpc_str2double(rpc->retv[$3]);" ;;
    d) echo "a$2 = vsrpc_str2double(rpc->retv[$3]);" ;;
    s) echo "a$2 = vsrpc_str2str(rpc->retv[$3]);" ;;
    I) echo "*a$2 = vsrpc_str2int(rpc->retv[$3]);" ;;
    F) echo "*a$2 = (float) vsrpc_str2doable(rpc->retv[$3]);" ;;
    D) echo "*a$2 = vsrpc_str2double(rpc->retv[$3]);" ;;
    S) echo "*a$2 = vsrpc_str2str(rpc->retv[$3]);" ;;
  esac
}
#----------------------------------------------------------------------------
get_fill_arg()
{ # get sorce code for fill return value(s)
  # (type, arg_num, in_num)
  case $1 in
    i) echo "a$2 = vsrpc_str2int(argv[$3]);" ;;
    f) echo "a$2 = (float) vsrpc_str2double(argv[$3]);" ;;
    d) echo "a$2 = vsrpc_str2double(argv[$3]);" ;;
    s) echo "a$2 = argv[$3];" ;;
  esac
}
#----------------------------------------------------------------------------
get_fill_def()
{ # get sorce code for fill return value(s)
  # (type, arg_num, ret_num)
  case $1 in
    i) echo "a$2 = 0;" ;;
    f) echo "a$2 = 0.;" ;;
    d) echo "a$2 = 0.;" ;;
    s) echo "a$2 = vsrpc_str2str(\"\");" ;;
    I) echo "*a$2 = 0;" ;;
    F) echo "*a$2 = 0.;" ;;
    D) echo "*a$2 = 0.;" ;;
    S) echo "*a$2 = vsrpc_str2str(\"\");" ;;
  esac
}
#----------------------------------------------------------------------------
# prepare source file headers
load_top

# create output files
if [ "$server" = "yes" ]
then
  echo "${local_h_top}" > "${server_out_dir}/${prj}${local_h}"
  [ ! -e "${server_out_dir}/${prj}${local_c}" ] && \
    echo "${local_c_top}" > "${server_out_dir}/${prj}${local_c}"
  echo "${server_h_top}" > "${server_out_dir}/${prj}${server_h}"
  echo "${server_c_top}" > "${server_out_dir}/${prj}${server_c}"
fi

if [ "$client" = "yes" ]
then
  echo "${remote_h_top}" > "${client_out_dir}/${prj}${remote_h}"
  echo "${remote_c_top}" > "${client_out_dir}/${prj}${remote_c}"
  echo "${client_h_top}" > "${client_out_dir}/${prj}${client_h}"
  echo "${client_c_top}" > "${client_out_dir}/${prj}${client_c}"
  [ ! -e "${client_out_dir}/${prj}${client_dbg_h}" ] && \
    echo "${client_dbg_h_top}" > "${client_out_dir}/${prj}${client_dbg_h}"
fi

# begin vsrpc_func_t structure
server_struct=`mktemp || echo /tmp/vsrpc_idl_tmp_${prj}${struct_suffix}`
echo "vsrpc_func_t ${prj}${struct_suffix}[] = {" > "${server_struct}"

# remove comment and empty strings from input "*.vsidl" file
sed "s/%.*$//g;/^$/d" "${input_file}" | (
while read ret ProcName args
do
  # read every function prototype
  RetType=`get_type $ret` # type of return value
  RetTypeGrep=`echo $RetType | sed 's/\*/ \*\\\*/g'`
  ProcNameC="${fn_prefix}${ProcName}"
  RetNum=0
  Ret1=""
  Ret2=""
  RetDesc=""
  RetList=""
  ArgProto=""
  ArgDesc=""
  ArgDesc2=""
  ArgDesc2Col="  "
  ArgAll=""
  ArgNum=0
  DefFill=""
  DefFillCol="    "
  RetFillCol="  "
  RetFill=""
  ArgFillCol="  "
  ArgFill=""
  ArgInProto=""
  ArgInDesc=""
  ArgIn=""
  ArgOutProto=""
  ArgOutDesc=""
  ArgOut=""
  ArgOut2=""
  FreeStrCol="  "
  FreeStr=""
  if [ "${ret}" != "v" ]
  then
    # return value is not 'void'
    ret2=`echo ${ret} | tr 'ifds' 'IFDS'`
    RetNum=$((${RetNum}+1))
    RetDesc=" ${RetType} a0;"
    RetFill="${RetFillCol}`get_fill_ret ${ret2} 0 0`\n"
    Ret1=" a0";
    Ret2="a0 = "
    RetList="${ret}"
    ArgOutProto=", ${RetType}*"
    ArgOutDesc=", ${RetType}* a0"
    ArgOut=", &a0"
    ArgOut2=", a0"
    DefFill="${DefFill}${DefFillCol}`get_fill_def ${ret2} 0`\n"
    [ "${ret}" = "s" ] && FreeStr="${FreeStrCol}vsrpc_free(a0);\n"
  fi
  cnt=0
  for arg in ${args}
  do
    cnt=$((${cnt}+1))
    atype=`get_type $arg`

    ArgProto="${ArgProto}, ${atype}"
    ArgDesc="${ArgDesc}, ${atype} a${cnt}"
    ArgAll="${ArgAll}, a${cnt}"

    [ -n "${ArgDesc2}" ] && ArgDesc2="${ArgDesc2} "

    if [ -n "$(echo 'ifds' | grep ${arg})" ]
    then
      # this is input argument
      ArgInProto="${ArgInProto}, ${atype}"
      ArgInDesc="${ArgInDesc}, ${atype} a${cnt}"
      ArgIn="${ArgIn}, a${cnt}"
      ArgDesc2="${ArgDesc2}${atype} a${cnt};"
      ArgNum=$((${ArgNum}+1))
      ArgFill="${ArgFill}${ArgFillCol}`get_fill_arg $arg $cnt $ArgNum`\n"
    fi

    if [ -n "$(echo 'IFDS' | grep $arg)" ]
    then
      # this is output argument
      ArgOutProto="${ArgOutProto}, ${atype}"
      ArgOutDesc="${ArgOutDesc}, ${atype} a${cnt}"
      ArgOut="${ArgOut}, a${cnt}"
      ArgOut2="${ArgOut2}, *a${cnt}"
      ArgDesc2="${ArgDesc2}`get_type2 $arg` a${cnt}[1];"
      RetFill="${RetFill}${RetFillCol}`get_fill_ret $arg $cnt $RetNum`\n"
      DefFill="${DefFill}${DefFillCol}`get_fill_def $arg $cnt`\n"
      RetNum=$(($RetNum+1))
      [ "$arg" = "S" ] && \
        FreeStr="${FreeStr}${FreeStrCol}vsrpc_free(*a$cnt);\n"
    fi
  done

  [ -n "$ArgDesc2" ] && ArgDesc2="${ArgDesc2Col}${ArgDesc2}\n"

  DefVal=""
  case "$ret" in
    i) DefVal=' 0'  ;;
    f) DefVal=' 0.' ;;
    d) DefVal=' 0.' ;;
    s) DefVal=' vsrpc_str2str("")' ;;
  esac

  CallList=`echo "s${args}" | tr -d ' IFDS'`
  tmp=`echo "$args" | tr -d ' ifds' | tr 'IFDS' 'ifds'`
  RetList="${RetList}${tmp}"
  ArgNumP1=$(($ArgNum+1))

  load_functions

  if [ "$server" = "yes" ]
  then # form sorce for "server" side
    echo "${local_proto}" >> "${server_out_dir}/${prj}${local_h}"

    tmp=`grep "^ *${RetTypeGrep} *${ProcNameC}${local_suffix} *(" "${server_out_dir}/${prj}${local_c}"`
    [ -z "$tmp" ] && \
      echo "${local_func}" >> "${server_out_dir}/${prj}${local_c}"

    echo -e "$server_func" >> "${server_out_dir}/${prj}${server_c}"

    # add entry to server vsrpc_func_t structute
    echo "  { \"$ProcName\", ${wrap_prefix}${ProcName} }," \
      >> "${server_struct}"
  fi

  if [ "$client" = "yes" ]
  then # form sorce for "client" side
    echo    "${remote_proto}" >> "${client_out_dir}/${prj}${remote_h}"
    echo    "${remote_func}"  >> "${client_out_dir}/${prj}${remote_c}"
    echo    "${client_proto}" >> "${client_out_dir}/${prj}${client_h}"
    echo -e "${client_func}"  >> "${client_out_dir}/${prj}${client_c}"
  fi

done)

# terminate vsrpc_func_t structure and add to *_server.c file
if [ "$server" = "yes" ]
then
  echo "${hr}" >> "${server_out_dir}/${prj}${local_h}"
  echo -e "  { NULL, NULL }\n};" >> "${server_struct}"
  cat "${server_struct}" >> "${server_out_dir}/${prj}${server_c}"
  echo "${hr}" >> "${server_out_dir}/${prj}${server_c}"
  rm "${server_struct}"
fi

if [ "$client" = "yes" ]
then
  echo "${hr}" >> "${client_out_dir}/${prj}${remote_h}"
  echo "${hr}" >> "${client_out_dir}/${prj}${client_h}"
fi

if [ "$examples" = "yes" ]
then
  # make examples
  load_examples
  echo "$server_example" > "${examples_out_dir}/${prj}${server_example_c}"
  echo                  >> "${examples_out_dir}/${prj}${server_example_c}"
  echo "$client_example" > "${examples_out_dir}/${prj}${client_example_c}"
  echo                  >> "${examples_out_dir}/${prj}${client_example_c}"
fi
#----------------------------------------------------------------------------

### end of "vsrpc_idl.sh" file ###

