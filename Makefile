PROGRAM=multiapp
OBJECTS=main.o

CC=g++
CFLAGS=$(shell pkg-config --cflags gtkmm-4.0 webkitgtk-6.0)
CFLAGS+= -Wall -g -std=c++23

LDFLAGS=$(shell pkg-config --libs gtkmm-4.0 webkitgtk-6.0)


multiapp: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(PROGRAM).exe

%.o : src/%.cc
	$(CC) $(CFLAGS) -c $<

clean: 
	rm -rf $(OBJECTS) $(PROGRAM) 