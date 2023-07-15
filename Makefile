SRC_DIR = src
BIN_DIR = bin
DEP_DIR = .deps

CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-language-extension-token -pedantic -std=gnu2x $(shell sdl2-config --cflags) -I$(SRC_DIR)
LDFLAGS = $(shell sdl2-config --libs) -lm
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

TARGET = game
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRCS))
DEPS = $(patsubst $(SRC_DIR)/%.c, $(DEP_DIR)/%.d, $(SRCS))

.PHONY: clean all

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BIN_DIR) $(DEP_DIR)
