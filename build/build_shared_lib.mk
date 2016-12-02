SRCS = $(wildcard *.c) $(wildcard ../*.c)
OBJS = $(SRCS:%.c=%.o)

LIB_SRCS = $(wildcard *.c)
LIB_OBJS = $(LIB_SRCS:%.c=%.o)
LIB_NAME_STATIC = lib$(notdir $(CURDIR)).a
LIB_NAME_SHARD = lib$(notdir $(CURDIR)).so

APP_NAME = $(notdir $(CURDIR)).out

PLATFORM := silan
#PLATFORM := montage

include ../../build/build.mk

CFLAG = -D$(CONFIG_PLATFORM) -g -Wall -I$(INCLUDE_PATH) -I$(INCLUDE_PATH_PLATFORM) -fPIC
LDFLAG = -L$(OUT_LIBS_SHARE)/$(PLATFORM)

.PHONY: clean install lib

all: share_lib

$(APP_NAME): $(SRCS)
	@$(CC) $(CFLAG) -o $@ $^ $(LDFLAG)

%.o: %.c
	@$(CC) $(CFLAG) -c -o $@ $<

clean:
	rm -f $(APP_NAME) $(OBJS) $(LIB_NAME_STATIC) $(LIB_NAME_SHARD) $(LIB_OBJS)

install:
	cp $(LIB_NAME_SHARD) $(OUT_LIBS_SHARE)/$(PLATFORM)

static_lib: $(LIB_SRCS) $(LIB_OBJS)
	$(AR) cqs $(LIB_NAME_STATIC) $(LIB_OBJS)

share_lib: $(LIB_SRCS) $(LIB_OBJS)
	$(CC) -shared -o $(LIB_NAME_SHARD) $(LIB_OBJS)
