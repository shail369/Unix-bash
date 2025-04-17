# Makefile for compiling the C files in the final folder

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = final_program
SRC_DIR = codes
OBJ_DIR = obj

# List of source files
SRCS = main.c helper.c hop.c reveal.c log.c background.c proclore.c seek.c myshrc.c IOredirection.c activities.c ping.c fgbg.c neonate.c iman.c
SRCS := $(addprefix $(SRC_DIR)/, $(SRCS))

# List of object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Rule to compile all object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Rule to clean up the build directory
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

