CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -Iinclude
LDFLAGS = -lcurl
SRC     = source/main.c source/engine.c
BIN     = bin/icrawl

.PHONY: all build clean install

all: build

build: $(BIN)

$(BIN): $(SRC) include/engine.h
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

clean:
	rm -rf bin

install: build
	install -m 755 $(BIN) /usr/local/bin/icrawl
