TARGET_EXEC := PLATOTerm
BUILD_DIR := ./build
SRC_DIRS := ./src

CC := m68k-amigaos-gcc

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS ?= -Os -DVERSION='"0.5"' -D__AMIGADATE__='"("__DATE__")"'
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
LDFLAGS := -lamiga

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(SRC_DIRS)/*.dep
	rm -rf dist/1.x/PLATOTerm
	rm -rf dist/1.x/PLATOTerm.info
	rm -rf dist/1.x/PLATOTerm.adf
	rm -rf dist/1.x/PLATOTermPrefs.info
	rm -rf dist/2.x/PLATOTerm
	rm -rf dist/2.x/PLATOTerm.info
	rm -rf dist/2.x/PLATOTerm.adf
	rm -rf dist/2.x/PLATOTermPrefs.info

dist: clean build/PLATOTerm
	cp dist/1.x/files/template.adf dist/1.x/PLATOTerm.adf
	cp build/PLATOTerm dist/1.x/PLATOTerm
	cp dist/1.x/files/PLATOTerm.info dist/1.x/PLATOTerm.info
	cp icons/1.x/PLATOTermPrefs.info dist/1.x/PLATOTermPrefs.info
	xdftool dist/1.x/PLATOTerm.adf write build/PLATOTerm /
	xdftool dist/1.x/PLATOTerm.adf write "dist/docs/Read Me" /
	cp dist/2.x/files/template.adf dist/2.x/PLATOTerm.adf
	cp build/PLATOTerm dist/2.x/PLATOTerm
	cp icons/NewIcon/PLATOTerm.info dist/2.x/PLATOTerm.info
	xdftool dist/2.x/PLATOTerm.adf write build/PLATOTerm /
	xdftool dist/2.x/PLATOTerm.adf write "dist/docs/Read Me" /

-include $(DEPS)

MKDIR_P ?= mkdir -p
