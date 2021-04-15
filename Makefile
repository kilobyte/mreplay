all: simple libmlog.so mreplay
clean:
	rm -f simple libmlog.so mreplay

simple: simple.c
	gcc -Wall -o $@ $^

libmlog.so: libmlog.c
	gcc -Wall -shared -o $@ $^

mreplay: mreplay.c
	gcc -Wall -g -o $@ $^ -pthread
