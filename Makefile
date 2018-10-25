TARGET_EXEC := PLATOTerm
BUILD_DIR := ./build
SRC_DIRS := ./src

CC := vc +aos68k

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(SRCS:.c=.dep)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I$(NDK_INC)

CPPFLAGS ?= $(INC_FLAGS) -deps
LDFLAGS := -L$(NDK_LIB) -lamiga

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

-include $(DEPS)

MKDIR_P ?= mkdir -p
