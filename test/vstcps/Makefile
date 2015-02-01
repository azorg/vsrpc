#----------------------------------------------------------------------------
OUT_NAME     := vstcps_test
OUT_DIR      := .
CLEAN_FILES := "$(OUT_DIR)/$(OUT_NAME).exe"
#----------------------------------------------------------------------------
# 1-st way to select source files
SRCS := \
	../../socklib/socklib.c \
	../../vssync.c \
	../../vsthread.c \
	../../vstcps.c \
	test_vstcps.c

HDRS := \
	../../socklib/socklib.h \
	../../vssync.h \
	../../vsthread.h \
	../../vstcps.h

# 2-nd way to select source files
#SRC_DIRS := . ../..
#HDR_DIRS := . ../..
#----------------------------------------------------------------------------
#DEFS   := -DVSTHREAD_LINUX -DVSTCPS_DEBUG -DVSTHREAD_DEBUG
DEFS    := -DVSTHREAD_LINUX -DVSTCPS_DEBUG -DVSTHREAD_DEBUG -DVSTHREAD_POOL 
OPTIM   := -O3
WARN    := -Wall
CFLAGS  := $(DEFS) $(OPTIM) $(WARN) -pipe $(CFLAGS)
LDFLAGS := -lpthread
#----------------------------------------------------------------------------
include ../Makefile.skel
#----------------------------------------------------------------------------

