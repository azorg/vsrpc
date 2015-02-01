#----------------------------------------------------------------------------
OUT_NAME    := test_vsfifo
OUT_DIR     := .
CLEAN_FILES := "$(OUT_DIR)/$(OUT_NAME).exe"
#----------------------------------------------------------------------------
# 1-st way to select source files
SRCS := \
	../../vsthread.c \
	../../vsfifo.c \
	test_vsfifo.c

HDRS := \
	../../vsthread.h \
	../../vsfifo.h

# 2-nd way to select source files
#SRC_DIRS := . ../..
#HDR_DIRS := . ../..
#----------------------------------------------------------------------------
#DEFS   := -DVSTHREAD_LINUX -DVSTHREAD_POOL -DVSTHREAD_DEBUG
DEFS    := -DVSTHREAD_LINUX -DVSTHREAD_DEBUG
OPTIM   := -Os
WARN    := -Wall
CFLAGS  := $(DEFS) $(OPTIM) $(WARN) -pipe $(CFLAGS)
LDFLAGS := -lpthread
#----------------------------------------------------------------------------
include ../Makefile.skel
#----------------------------------------------------------------------------

