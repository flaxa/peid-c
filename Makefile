
OUT_DIR = bin
SRC = src/main.c src/pe.c src/signature_tree.c lib/cJSON.c
CFLAGS = -Wall -Wextra -Iinclude

ifeq ($(OS),Windows_NT)
    CC = gcc
    OUT = $(OUT_DIR)/peid-c.exe
    MKDIR = if not exist $(OUT_DIR) mkdir $(OUT_DIR)
    RM = rmdir /s /q $(OUT_DIR)
else
    CC = gcc 
    OUT = $(OUT_DIR)/peid-c
    MKDIR = mkdir -p $(OUT_DIR)
    RM = rm -rf $(OUT_DIR)
endif

all: $(OUT)

$(OUT): $(SRC)
	$(MKDIR)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	$(RM)
