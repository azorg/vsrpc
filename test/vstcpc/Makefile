OUT_NAME     := test_vstcpc
OUT_DIR      := .
CLEAN_FILES := "$(OUT_DIR)/$(OUT_NAME).exe"
#----------------------------------------------------------------------------
# 1-st way to select source files
#SRCS := 
#HDRS := 

# 2-nd way to select source files
SRC_DIRS := . ../.. client ../../socklib
HDR_DIRS := . ../.. client ../../socklib
#----------------------------------------------------------------------------
DEFS    := -DVSTCPC_DEBUG
OPTIM   := -O3
WARN    := -Wall
CFLAGS  := $(DEFS) $(OPTIM) $(WARN) -pipe $(CFLAGS)
LDFLAGS := -lpthread -lrt
#----------------------------------------------------------------------------
include ../Makefile.skel
#----------------------------------------------------------------------------

