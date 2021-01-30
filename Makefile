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
#DEPS = main.h MCP9808.h device_defs.h i2c.h runMag.h cmdmgr.h config.gperf cfghash.c  
#DEPS = main.h MCP9808.h device_defs.h i2c.h runMag.h cmdmgr.h uthash/uthash.h analysis.h
DEPS = 
#SRCS = main.c runMag.c i2c.c cmdmgr.c cfghash.c
SRCS = multiMag.c
OBJS = $(subst .c,.o,$(SRCS))
#DOBJS = main.o runMag.o i2c.o cmdmgr.o cfghash.o 
DOBJS = multiMag.o
LIBS = -lm
DEBUG = -g -Wall
CFLAGS = -I.
LDFLAGS =
TARGET_ARCH =
LOADLIBES =
LDLIBS =
GPERFFLAGS = --language=ANSI-C 

TARGET = multiMag

RM = rm -f

all: release

#cfghash.c: config.gperf
#	$(GPERF) $(GPERFFLAGS) config.gperf > cfghash.c

# debug: runMag.c cfghash.c $(DEPS) 
debug: multiMag.c $(DEPS) 
	$(CC) -c $(DEBUG) $(SRCS)
#	$(CC) -c $(DEBUG) multiMag.c  
#	$(CC) -c $(DEBUG) cmdmgr.c  
#	$(CC) -c $(DEBUG) i2c.c
#	$(CC) -c $(DEBUG) jsonLogOutput.c
#	$(CC) -c $(DEBUG) csvLogOutput.c
#	$(CC) -c $(DEBUG) grapeLogOutput.c
#	$(CC) -c $(DEBUG) analysis.c
#	$(CC) -c $(DEBUG) cfghash.c
#	$(CC) -o $(TARGET) $(DEBUG) main.c runMag.o i2c.o cmdmgr.o cfghash.o $(LIBS)
#	$(CC) -o $(TARGET) $(DEBUG) main.c runMag.o i2c.o cmdmgr.o $(LIBS)
#	$(CC) -o $(TARGET) $(DEBUG) main.c runMag.o i2c.o cmdmgr.o jsonLogOutput.o csvLogOutput.o grapeLogOutput.o analysis.o $(LIBS)
#	$(CC) -o $(TARGET) $(DEBUG) main.c runMag.o i2c.o cmdmgr.o jsonLogOutput.o csvLogOutput.o grapeLogOutput.o $(LIBS)
	$(CC) -o $(TARGET) $(DEBUG) multiMag.c $(LIBS)

#release: runMag.c cfghash.c $(DEPS)
release: runMag.c $(DEPS)
	$(CC) -c $(DEBUG) $(SRCS)
#	$(CC) -c $(CFLAGS) multiMag.c
#	$(CC) -c $(CFLAGS) cmdmgr.c
#	$(CC) -c $(CFLAGS) i2c.c  
#	$(CC) -c $(CFLAGS) jsonLogOutput.c
#	$(CC) -c $(CFLAGS) csvLogOutput.c
#	$(CC) -c $(CFLAGS) grapeLogOutput.c
#	$(CC) -c $(CFLAGS) analysis.c
#	$(CC) -c $(CFLAGS) cfghash.c
#	$(CC) -o $(TARGET) $(CFLAGS) main.c runMag.o i2c.o cmdmgr.o cfghash.o $(LIBS)
#	$(CC) -o $(TARGET) $(CFLAGS) main.c runMag.o i2c.o cmdmgr.o $(LIBS)
#	$(CC) -o $(TARGET) $(CFLAGS) main.c runMag.o i2c.o cmdmgr.o jsonLogOutput.o csvLogOutput.o grapeLogOutput.o analysis.o $(LIBS)
#	$(CC) -o $(TARGET) $(CFLAGS) main.c runMag.o i2c.o cmdmgr.o jsonLogOutput.o csvLogOutput.o grapeLogOutput.o $(LIBS)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRCS) $(LIBS)

clean:
#	$(RM) $(OBJS) $(TARGET) cfghash.c config.json
	$(RM) $(OBJS) $(TARGET) config.json

distclean: clean
	
.PHONY: clean distclean all debug release