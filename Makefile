CC = clang

CFILES = $(wildcard src/*.c)
CFLAGS = -Wall

OBJFILES = $(CFILES:.c=.o)
OUT = src/main


$(OUT): $(OBJFILES)


.PHONY: clean
clean:
	rm -f $(OBJFILES) $(OUT)
