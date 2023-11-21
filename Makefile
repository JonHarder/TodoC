CC = clang

CFILES = $(wildcard src/*.c)
CFLAGS = -Wall

OBJFILES = $(CFILES:.c=.o)
OUT = src/main


$(OUT): $(OBJFILES)


.PHONY: clean install
clean:
	rm -f $(OBJFILES) $(OUT)
install:
	sudo cp ./src/main /usr/local/bin/todo

