SRCEXT := .c
SRCDIRS := $(patsubst ./%,%,$(shell find . -type d))
# save for learn
# SRCDIRS := $(patsubst ./%,%,$(shell find . -type d | grep -vw Aregress_test))
SRCS1 := $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*, $(SRCEXT))))
# OBJS = $(SRCS:%.c=%.o)
SRCS := $(filter-out ./Aregress_test.c, $(SRCS1))

OBJS = $(patsubst %.c,%.o,$(SRCS))

.PHONY: clean install prep test

all: prep $(TARGET)

prep:
	@echo "target: $(TARGET)"
#@svn log -l 1 | sed -n 2p | sed 's/^/"&/g' | sed 's/$$/&"/g' > netman_version.svn
	@git log | sed -n 1p | awk '{print $$2}'| sed 's/^/"&/g' | sed 's/$$/&"/g' > netman_version.git

$(TARGET): $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(LDFLAGS)
	cp $(TARGET) $(OUT_BIN)/$(PLATFORM)
%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS) netman_version.svn netman_version.git

install: $(TARGET)
	@if [ -n "$(INSTALL_DIR)" ];then \
		mkdir -p $(INSTALL_DIR); \
		cp $(TARGET) $(INSTALL_DIR); \
	fi

help:
	@echo "No help now!"

test:
	@echo "test: $(OBJS)"
