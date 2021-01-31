#=========================================================================
# Simple Makefile for multiMag
#
# Author:      David Witten, KD0EAG
# Date:        January 30, 2021
# License:     GPL 3.0
#=========================================================================
CC = gcc
LD = gcc
GPERF = gperf
CXX = g++
#DEPS = main.h MCP9808.h device_defs.h i2c.h multiMag.h cmdmgr.h config.gperf cfghash.c  
#DEPS = main.h MCP9808.h device_defs.h i2c.h multiMag.h cmdmgr.h uthash/uthash.h analysis.h
DEPS = main.h logFiles.h mcp9808.h rm3100.h
#SRCS = main.c multiMag.c i2c.c cmdmgr.c cfghash.c
SRCS = multiMag.c logFiles.c
OBJS = $(subst .c,.o,$(SRCS))
DOBJS = multiMag.o
LIBS = -lm
DEBUG = -g -Wall
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS =
GPERFFLAGS = --language=ANSI-C
PTHREAD = -pthread

TARGET = multiMag

RM = rm -f

all: release

#@echo $(TARGET)
#@echo $(SRCS)
#@echo $(OBJS)
#@echo $(DEPS)

#cfghash.c: config.gperf
#	$(GPERF) $(GPERFFLAGS) config.gperf > cfghash.c

debug: $(TARGET) $(DEPS) $(OBJS)
	$(CC) -c $(DEBUG) $(PTHREAD) multiMag.c  
	$(CC) -o $(TARGET) $(PTHREAD) $(DEBUG) multiMag.c $(LIBS)

#release: $(TARGET) $(SRCS) $(OBJS) $(DEPS)

release: $(OBJS)
	$(CC) -o $(TARGET) $(PTHREAD) $(CFLAGS) $(OBJS) $(LIBS)
    
multiMag.o : multiMag.c
	$(CC) -c $(CFLAGS) $(PTHREAD) multiMag.c

clean:
	$(RM) $(OBJS) $(TARGET) config.json

distclean: clean
	
.PHONY: clean distclean all debug release