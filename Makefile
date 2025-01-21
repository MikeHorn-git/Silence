BUILD_DIR     := $(PWD)/bin
KERN_VERSION  := $(shell uname -r)
KRN_SRC       := /lib/modules/$(KERN_VERSION)/build

INCLUDE_DIRS  := $(shell find ${PWD}/include -type d)
EXTRA_CFLAGS  := $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

FILES         := '*.c' '*.h'
FORMAT        := clang-format
TARGET        := silence
DIST_FILES    := docs include Makefile README.md src test/c2 test/create_image

.DEFAULT_GOAL := all

help:
	@echo "Usage: make <target>"
	@echo "Targets:"
	@echo "  help         Display this help message"
	@echo "  all          Build LKM"
	@echo "  clean        Clean LKM"
	@echo "  dist         Create tar archive release"
	@echo "  format       Format all .c, .h files with clang-format"

all:
	make -C ${KRN_SRC} M=${PWD}/src EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules

clean:
	make -C ${KRN_SRC} M=${PWD}/src clean

dist:
	tar cvJf $(TARGET).tar.xz $(DIST_FILES)

format:
	git ls-files $(FILES) | xargs $(FORMAT) -i

.PHONY: all clean dist format
