# cfklp Makefile

.PHONY: clean test

# Pick one. Turn off -static if on OS X!
#STATIC =
STATIC = -static

CC = clang -Wall $(STATIC)
CFLAGS = -I/usr/pkg/include
LDFLAGS = -L/usr/pkg/lib
LIBS = -lps -lm -lintl

cfklp: cfklp.c
	$(CC) -o $(.TARGET) $(CFLAGS) $(.ALLSRC) $(LDFLAGS) $(LIBS)

clean:
	rm -f cfklp *.o *.core *.ps *.pdf

test:
	./cfklp test.txt out.ps
