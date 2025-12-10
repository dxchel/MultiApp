PROGRAM=multiapp
OBJECTS=build/browser_app.o build/main_application.o build/main.o

CC=g++
CFLAGS=$(shell pkg-config --cflags gtkmm-4.0 webkitgtk-6.0)
CFLAGS+= -Wall -g -std=c++23

LDFLAGS=$(shell pkg-config --libs gtkmm-4.0 webkitgtk-6.0)

multiapp: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(PROGRAM).exe

build/%.o : src/%.cc
	if [ ! -d build ]; then mkdir build; fi
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -rf build $(PROGRAM).exe 