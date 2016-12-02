ifeq ($(PLATFORM),silan)
CC = csky-linux-gcc
AR = csky-linux-ar
CONFIG_PLATFORM := CONFIG_SILAN
#LOG_LEVEL := LOG_LEVEL=3
INCLUDE_PATH_PLATFORM := $(TOPDIR)/sc6138_sdk/sdk/dev/include
LIBS_SHARE_PLATFORM := $(TOPDIR)/sc6138_sdk/sdk/dev/lib
LIBS_PRIV := -lslpadmux -lslpwm -lslgpio -ljson-c
endif

ifeq ($(PLATFORM),montage)
CC = mips-openwrt-linux-gcc
AR = mips-openwrt-linux-ar
CONFIG_PLATFORM := CONFIG_MONTAGE
INCLUDE_PATH_PLATFORM := .
LIBS_SHARE_PLATFORM := .
LIBS_PRIV :=
endif

ifeq ($(PLATFORM),x86)
CC = gcc
AR = ar
CONFIG_PLATFORM := CONFIG_X86
INCLUDE_PATH_PLATFORM := .
LIBS_SHARE_PLATFORM := .
#LOG_LEVEL := LOG_LEVEL=3
endif

ifeq ($(BUILD_TYPE),release)
LOG_LEVEL := LOG_LEVEL=1
endif

ifeq ($(BUILD_TYPE),debug)
LOG_LEVEL := LOG_LEVEL=4
endif
