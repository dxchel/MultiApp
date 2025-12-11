PROGRAM=multiapp
OBJECTS=build/browser_app.o build/main_application.o
MAIN=build/main.o
TESTS=test_build/main_application_tests.o test_build/main.o

CC=g++
CFLAGS=$(shell pkg-config --cflags gtkmm-4.0 webkitgtk-6.0)
CFLAGS+= -Wall -g -std=c++23

LDFLAGS=$(shell pkg-config --libs gtkmm-4.0 webkitgtk-6.0)

multiapp: $(OBJECTS) $(MAIN)
	$(CC) $(OBJECTS) $(MAIN) $(LDFLAGS) -o $(PROGRAM).exe

tests: $(OBJECTS) $(TESTS)
	$(CC) $(OBJECTS) $(TESTS) $(LDFLAGS) -o multiapp_tests.exe

$(OBJECTS): | build
$(TESTS): | test_build

build:
	if [ ! -d build ]; then mkdir build; fi

test_build:
	if [ ! -d test_build ]; then mkdir test_build; fi

build/%.o : src/%.cc $(wildcard include/%.hpp)
	$(CC) $(CFLAGS) -c $< -o $@


test_build/%.o : tests/%.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -rf *build *.exe 