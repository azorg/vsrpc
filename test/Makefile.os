#----------------------------------------------------------------------------
#WIN32 := 1
#----------------------------------------------------------------------------
ifndef WIN32
# Linux:
CFLAGS   := $(DEFS) $(OPTIM) $(WARN) $(CFLAGS) -pipe
LDFLAGS  := -lpthread
else
# Windows (MinGW):
EXEC_EXT := .exe
CFLAGS   := $(DEFS) $(OPTIM) $(WARN) $(CFLAGS)
LDFLAGS  := /bin/msys-1.0.dll -lwsock32 ../../NtQuerySemaphore.lib
endif
#----------------------------------------------------------------------------
