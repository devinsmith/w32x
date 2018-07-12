# Makefile for libw32x tests

.PHONY: all clean

SRCS1 = test1.c
OBJS1 = $(SRCS1:.c=.o)
DEPS1 = $(SRCS1:.c=.d)

OBJS = $(OBJS1)
DEPS = $(DEPS1)

CC? = gcc
RM = rm -f

CFLAGS = -Wall -O2
INCLUDES = -I../include

LIB = libw32x
STATIC_LIB = $(LIB).a

EXE1 = test1

EXES = $(EXE1)

all: $(EXES)

$(EXE1): $(OBJS1) ../src/$(STATIC_LIB)
	$(CC) $(CFLAGS) -o $(EXE1) $(OBJS1) ../src/$(STATIC_LIB) -lX11

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -MT $@ -o $@ -c $<

clean:
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) $(EXES)

# Include automatically generated dependency files
-include $(DEPS)

