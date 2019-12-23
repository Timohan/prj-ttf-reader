#/*!
#* \file Makefile
#* \brief Makefile for compiling prj-ttf-reader library
#* and generating pkgconfig file
#* and making install & uninstall
#*
#* Copyright of Timo Hannukkala. All rights reserved.
#*
#* \author Timo Hannukkala <timohannukkala@hotmail.com>
#*/
TARGET:=libprj-ttf-reader.so
VERSION:=0.1
PREFIX:=/usr
LIBDIR:= $(PREFIX)/lib
PKGCONFIGDIR:=$(LIBDIR)/pkgconfig
PKGCONFIGFILE:=prj-ttf-reader
CC:=$(if $(CC),$(CC),gcc)
CFLAGS+=-Wno-unused-parameter -Wuninitialized -Wconversion -Wshadow -Wpointer-arith \
	-Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wcast-align -Wbad-function-cast \
	-Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual -Wunreachable-code -Wfloat-equal \
	-Wredundant-decls -Wold-style-definition -Wno-unused-const-variable -Wextra -Wall
CFLAGS+=-Werror
CFLAGS+=-O2
CFLAGS+=-g -fPIC
LDFLAGS+=-shared

CURRENT_DIR=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

src_SRCDIR:=$(CURRENT_DIR)src
src_SRCS:=$(wildcard $(src_SRCDIR)/*.c)
src_OBJS:=$(src_SRCS:.c=.o)

drawfont_SRCDIR:=$(CURRENT_DIR)src/drawfont
drawfont_SRCS:=$(wildcard $(drawfont_SRCDIR)/*c)
drawfont_OBJS:=$(drawfont_SRCS:.c=.o)

reader_SRCDIR:=$(CURRENT_DIR)src/reader
reader_SRCS:=$(wildcard $(reader_SRCDIR)/*c)
reader_OBJS:=$(reader_SRCS:.c=.o)

USE_TIME_DEBUG:=

all: default

time-debug:
	make USE_TIME_DEBUG="-DTIME_DEBUG"

default: $(src_OBJS) $(drawfont_OBJS) $(reader_OBJS)
	$(CC) $(src_OBJS) $(drawfont_OBJS) $(reader_OBJS) $(LDFLAGS) -o $(TARGET)

$(src_OBJS):%.o: %.c
	$(CC) $(CFLAGS) $(USE_TIME_DEBUG) -c $< -o $@

$(drawfont_OBJS):%.o: %.c
	$(CC) $(CFLAGS) $(USE_TIME_DEBUG) -c $< -o $@

$(reader_OBJS):%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/reader/*.o
	rm -f src/drawfont/*.o
	rm -f src/*.o
	rm -f *.so

install: $(PKGCONFIGDIR)/$(PKGCONFIGFILE).pc
	mkdir -p $(LIBDIR)/prj-ttf-reader
	cp lib*.so $(LIBDIR)/prj-ttf-reader
	mkdir -p $(PREFIX)/include/prj-ttf-reader
	cp src/prj-ttf-reader.h $(PREFIX)/include/prj-ttf-reader

$(PKGCONFIGDIR)/$(PKGCONFIGFILE).pc:
	@echo "prefix=$(PREFIX)" > $@
	@echo "exec_prefix=\$${prefix}" >> $@
	@echo "libdir=\$${prefix}/lib/prj-ttf-reader" >> $@
	@echo "includedir=\$${prefix}/include/prj-ttf-reader" >> $@
	@echo "" >> $@
	@echo "Name: prj-ttf-reader" >> $@
	@echo "Description: prj-ttf-reader lib parses ttf" >> $@
	@echo "Version: $(VERSION)" >> $@
	@echo "Cflags: -I\$${includedir}" >> $@
	@echo "Libs: -L\$${libdir} -lprj-ttf-reader -Wl,--rpath=\$${libdir}" >> $@

uninstall:
	rm -rf $(LIBDIR)/prj-ttf-reader
	rm -rf $(PREFIX)/include/prj-ttf-reader
	rm -f $(PKGCONFIGDIR)/$(PKGCONFIGFILE).pc
