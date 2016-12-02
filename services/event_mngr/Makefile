include ../../build/build_head.mk
CFLAGS = -D$(CONFIG_PLATFORM) -D$(LOG_LEVEL) -g -Wall -I$(INCLUDE_PATH) -I$(INCLUDE_PATH_PLATFORM)
LDFLAGS = -L$(OUT_LIBS_SHARE)/$(PLATFORM) $(LDFLAGS_PLATFORM) -lpthread -levent_mngr_utils -llogd_utils -lsys_utils
include ../../build/build_executable.mk
