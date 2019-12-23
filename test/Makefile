#/*!
#* \file Makefile
#* \brief Makefile for compiling test
#*
#* Copyright of Timo hannukkala, Inc. All rights reserved.
#*
#* \author Timo Hannukkala <timohannukkala@hotmail.com>
#*/
TARGET:=unit_tests
CXX:=$(if $(CXX),$(CXX),g++)
GOOGLETESTFOLDER=/mnt/prj/googletest
CXXFLAGS+=-g -Wall -pedantic -c -pipe -std=gnu++11 -W -D_REENTRANT -fPIC
CXXFLAGS+=-I./src
CXXFLAGS+=-I$(GOOGLETESTFOLDER)/googletest -I$(GOOGLETESTFOLDER)/googletest/include
LDFLAGS:=-lpthread
# set current make dir
CURRENT_DIR=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

src_SRCDIR:=$(CURRENT_DIR)src
src_SRCS:=$(wildcard $(src_SRCDIR)/*.cpp)
src_OBJS:=$(src_SRCS:.cpp=.o)

TESTIMAGEFOLDERS:=$(CURRENT_DIR)img_data

all: default

default: $(src_OBJS)
	$(CXX) $(CXXFLAGS) $(GOOGLETESTFOLDER)/googletest/src/gtest-all.cc -o $(CURRENT_DIR)gtest-all.o
	$(CXX) $(CXXFLAGS) $(CURRENT_DIR)../lib/src/drawfont/glyph_image_positions.c -DTEST_CASE -o $(CURRENT_DIR)glyph_image_positions.o
	$(CXX) $(CXXFLAGS) $(CURRENT_DIR)../lib/src/drawfont/glyph_graph_generator.c -DTEST_CASE -o $(CURRENT_DIR)glyph_graph_generator.o
	$(CXX) $(CXXFLAGS) $(CURRENT_DIR)../lib/src/drawfont/glyph_drawer.c -DTEST_CASE -o $(CURRENT_DIR)glyph_drawer.o
	$(CXX) $(CXXFLAGS) $(CURRENT_DIR)../lib/src/drawfont/glyph_filler.c -DTEST_CASE -o $(CURRENT_DIR)glyph_filler.o
	$(CXX) $(CXXFLAGS) $(CURRENT_DIR)../lib/src/reader/parse_text.c -DTEST_CASE -o $(CURRENT_DIR)parse_text.o
	$(CXX) $(src_OBJS) $(CURRENT_DIR)gtest-all.o $(drawfont_OBJS) $(CURRENT_DIR)glyph_image_positions.o $(CURRENT_DIR)glyph_graph_generator.o $(CURRENT_DIR)glyph_drawer.o $(CURRENT_DIR)glyph_filler.o $(CURRENT_DIR)parse_text.o $(LDFLAGS) -o $(TARGET)

$(src_OBJS):%.o: %.cpp
	$(CXX) -DTEST_IMAGE_FOLDERS="\"$(TESTIMAGEFOLDERS)\"" $(CXXFLAGS) -DTEST_CASE -c $< -o $@

clean:
	rm -f $(TARGET)
	rm -f src/*.o
	rm -f *.o
