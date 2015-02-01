#----------------------------------------------------------------------------
OUT_NAME    := test_vstcpd
OUT_DIR     := .
#CLEAN_FILES := "$(OUT_DIR)/$(OUT_NAME).exe"
#----------------------------------------------------------------------------
# 1-st way to select source files
#SRCS := 
#HDRS := 

# 2-nd way to select source files
SRC_DIRS := . ../.. server ../../socklib
HDR_DIRS := . ../.. server ../../socklib
#----------------------------------------------------------------------------
OPTIM := -O3
WARN  := -Wall
#----------------------------------------------------------------------------
ifndef WIN32
# Linux:
DEFS  := -DVSTCPD_DEBUG -DVSTCPS_DEBUG -DVSTHREAD_DEBUG -DVSTHREAD_LINUX_RT
#DEFS := -DVSTCPD_DEBUG -DVSTCPS_DEBUG -DVSTHREAD_DEBUG -DVSTHREAD_LINUX -DVSTHREAD_POOL
else
# Windows (MinGW):
DEFS  := -DVSTCPD_DEBUG -DVSTCPS_DEBUG -DVSTHREAD_DEBUG 
endif
#----------------------------------------------------------------------------
include ../Makefile.os
include ../Makefile.skel
#----------------------------------------------------------------------------

