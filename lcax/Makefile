# Makefile template for shared library
IDIR = .
CC = gcc # C compiler
CFLAGS = -fPIC -Wall -Wextra -O2 -g -I$(IDIR) -D_LCA3_MEMS# C flags
LDFLAGS = -shared  # linking flags
RM = rm -f  # rm command
TARGET_LIB = LCA_math.so # target lib

SRCS = mathematics.c # source files
OBJS = $(SRCS:.c=.o)


_DEPS = mathematics.h PlatformLCA3_MEMS_Mathematics.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c $(DEPS)
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)