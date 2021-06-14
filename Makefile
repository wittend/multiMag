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
DEPS = main.h logFiles.h mcp9808.h rm3100.h utilRoutines.h uthash/uthash.h i2c.h cmdline.h
SRCS = multiMag.c logFiles.c utilRoutines.c config.c i2c.c cmdline.c
OBJS = $(subst .c,.o,$(SRCS))
DOBJS = multiMag.o logFiles.o utilRoutines.o config.o i2c.o cmdline.o
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

debug: $(OBJS)
	$(CC) -o $(TARGET) $(DEBUG) $(PTHREAD) $(CFLAGS) $(OBJS) $(LIBS)

release: $(OBJS)
	$(CC) -o $(TARGET) $(PTHREAD) $(CFLAGS) $(OBJS) $(LIBS)
    
clean:
	$(RM) $(OBJS) $(TARGET) config.json

distclean: clean
	
.PHONY: clean distclean all debug release

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
#	@echo "... websockets"
#	@echo "... websockets_shared"
.PHONY : help

