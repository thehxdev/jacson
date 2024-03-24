CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Wno-unused-result
LDFLAGS :=
LIBS :=

BIN := build/test


$(BIN): src/jacson.o test/test.o
	@mkdir -p build
	$(CC) $(LDFLAGS) -o $(BIN) src/jacson.o test/test.o

src/jacson.o: src/jacson.c
	$(CC) $(CFLAGS) -c -o src/jacson.o $<

test/test.o: test/test.c src/jacson.o
	$(CC) $(CFLAGS) -c -o test/test.o test/test.c

clean:
	rm -rf src/jacson.o test/test.o build
