NAME = AStruct

ifeq ($(OS), Windows_NT) # Windows
    LIB = dll
    EXE = .exe
    DOXYGEN = "$(subst \,/,$(PROGRAMFILES)/doxygen/bin/doxygen.exe)"
else # Linux and others
    LIB = so
    PIC = -fPIC
    DOXYGEN = doxygen
endif

CFLAGS += -Wall -g -fmessage-length=0 -Isrc
PREFIX ?= /usr

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=obj/%.o)
DEPS = $(patsubst %.o, %.d, $(OBJECTS))
HEADERS = $(wildcard src/*.h)
TESTS_SRC = $(wildcard tests/*_test.c)
TESTS = $(patsubst %.c, %$(EXE), $(TESTS_SRC))

TARGET = build/lib$(NAME).$(LIB)

all: CFLAGS += -DNDEBUG -O3
all: $(TARGET)

-include $(DEPS)

obj/%.o: CFLAGS += -MMD
obj/%.o: src/%.c
	gcc $(CFLAGS) -c $< -o $@

debug: CFLAGS += -O0
debug: $(TARGET) tests

$(TARGET): CFLAGS += $(PIC)
$(TARGET): obj build $(OBJECTS)
	gcc -shared -o $@ $(OBJECTS)
	
obj:
	@mkdir -p obj

build:
	@mkdir -p build

tests/%_test$(EXE): tests/%_test.c obj/%.o
	gcc $(CFLAGS) $< -o $@
	@sh ./tests/runtest.sh $@ $(VALGRIND)

ifeq ($(OS), Windows_NT) # Windows - copy DLL
    TESTS_LIB = $(TARGET:build/%=tests/%)
    $(TESTS_LIB): $(TARGET)
	    cp $(TARGET) $(TESTS_LIB)
    tests_lib: $(TESTS_LIB)
else
    tests_lib: $(TARGET)
endif

.PHONY: tests
tests: CFLAGS := $(filter-out -O3, $(CFLAGS))
tests: CFLAGS := $(filter-out -DNDEBUG, $(CFLAGS))
tests: CFLAGS += -O0 -Wl,-rpath=build -Lbuild -l$(NAME)
tests: tests_lib $(TESTS)

.PHONY: valgrind
valgrind: VALGRIND = "valgrind -v --leak-check=full --show-reachable=yes --error-exitcode=1 --log-file=tests/valgrind.log"

.PHONY: doc
doc:
	$(DOXYGEN)

clean-doc:
	rm -rf doc

clean:
	rm -rf build obj $(TESTS) tests/tests.log tests/valgrind.log

install:
	install -d $(PREFIX)/lib/
	install $(TARGET) $(PREFIX)/lib/
	install -d $(PREFIX)/include/
	install $(HEADERS) $(PREFIX)/include/

install-doc:
	install -d $(PREFIX)/share/doc/AStruct
	cp -r doc/* $(PREFIX)/share/doc/AStruct

