OUT_NAME     := test_vstcpc
OUT_DIR      := .
#CLEAN_FILES := "$(OUT_DIR)/$(OUT_NAME).exe"
#----------------------------------------------------------------------------
# 1-st way to select source files
#SRCS := 
#HDRS := 

# 2-nd way to select source files
SRC_DIRS := . ../.. client ../../socklib
HDR_DIRS := . ../.. client ../../socklib
#----------------------------------------------------------------------------
OPTIM := -O3
WARN  := -Wall
#----------------------------------------------------------------------------
ifndef WIN32
# Linux:
DEFS  := -DVSTCPC_DEBUG -DVSTHREAD_DEBUG -DVSTHREAD_LINUX_RT
#DEFS := -DVSTCPC_DEBUG -DVSTHREAD_DEBUG -DVSTHREAD_LINUX -DVSTHREAD_POOL 
else
# Windows (MinGW):
DEFS  := -DVSTCPC_DEBUG -DVSTHREAD_DEBUG 
endif
#----------------------------------------------------------------------------
include ../Makefile.os
include ../Makefile.skel
#----------------------------------------------------------------------------

