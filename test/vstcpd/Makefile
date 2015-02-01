#----------------------------------------------------------------------------
OUT_NAME    := test_vstcpd
OUT_DIR     := .
CLEAN_FILES := "$(OUT_DIR)/$(OUT_NAME).exe"
#----------------------------------------------------------------------------
# 1-st way to select source files
#SRCS := 
#HDRS := 

# 2-nd way to select source files
SRC_DIRS := . ../.. server ../../socklib
HDR_DIRS := . ../.. server ../../socklib
#----------------------------------------------------------------------------
#DEFS   := -DVSTCPD_DEBUG -DVSTHREAD_DEBUG -DVSTCPS_DEBUG -DVSTHREAD_LINUX_RT
DEFS    := -DVSTHREAD_LINUX -DVSTHREAD_POOL \
           -DVSTCPS_DEBUG -DVSTHREAD_DEBUG -DVSTCPD_DEBUG 
#DEFS   := -DVSTHREAD_LINUX_RT -DVSTCPS_DEBUG 
#DEFS   := -DVSTHREAD_LINUX_RT -DVSTCPD_DEBUG 
#DEFS   := -DVSTHREAD_LINUX -DVSTHREAD_DEBUG -DVSTHREAD_POOL

OPTIM   := -O3
WARN    := -Wall
CFLAGS  := $(DEFS) $(OPTIM) $(WARN) -pipe $(CFLAGS)
LDFLAGS := -lpthread -lrt
#----------------------------------------------------------------------------
include ../Makefile.skel
#----------------------------------------------------------------------------

