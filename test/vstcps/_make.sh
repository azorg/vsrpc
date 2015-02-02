#! /bin/sh
test `uname` = Linux && OPT='-j4'
make ${OPT}

