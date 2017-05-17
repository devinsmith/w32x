# Makefile for libw32x

.PHONY: all clean

SRCS = button.c defwnd.c graphics.c w32x.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

CC? = gcc
AR? = ar
RM = rm -f

CFLAGS = -Wall -O2
INCLUDES = -I../include

LIB = libw32x
STATIC_LIB = $(LIB).a

all: $(STATIC_LIB)

$(STATIC_LIB) : $(OBJS)
	$(RM) $(STATIC_LIB)
	$(AR) cru $(STATIC_LIB) $(OBJS)
	ranlib $(STATIC_LIB)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -MT $@ -o $@ -c $<

clean:
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) $(STATIC_LIB)

# Include automatically generated dependency files
-include $(DEPS)

