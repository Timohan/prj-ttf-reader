#/*!
#* \file Makefile
#* \brief Makefile for compiling
#*
#* Copyright of Timo hannukkala, Inc. All rights reserved.
#*
#* \author Timo Hannukkala <timohannukkala@hotmail.com>
#*/
TARGET:=opengl-example
CXX:=$(if $(CXX),$(CXX),g++)
PKGFLAGS=`pkg-config --cflags --libs prj-ttf-reader`
CXXFLAGS+=-g -Wall -pedantic -c -pipe -std=gnu++11 -W -D_REENTRANT -fPIC
CXXFLAGS+=-I./src
CXXFLAGS+=$(PKGFLAGS)
CXXFLAGS+=-Wno-unused-parameter -Wuninitialized -Wconversion -Wshadow -Wpointer-arith \
	 -Wswitch-default -Wswitch-enum -Wcast-align \
	 -Winline -Wundef -Wcast-qual -Wunreachable-code -Wlogical-op -Wfloat-equal \
	 -Wredundant-decls -Werror \
	 -Wno-unused-const-variable
LDFLAGS:=-lGL -lGLEW -lglfw $(PKGFLAGS)
# set current make dir
CURRENT_DIR=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

src_SRCDIR:=$(CURRENT_DIR)src
src_SRCS:=$(wildcard $(src_SRCDIR)/*.cpp)
src_OBJS:=$(src_SRCS:.cpp=.o)

all: default

default: $(src_OBJS)
	$(CXX) $(src_OBJS) $(LDFLAGS) -o $(TARGET)

$(src_OBJS):%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)
	rm -f src/*.o
