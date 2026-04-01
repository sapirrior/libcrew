CC = clang
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lcurl
SRC = $(wildcard source/*.c)
TARGET = crew

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)
