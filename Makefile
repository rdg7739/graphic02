#!smake
#
#

MACHINE= $(shell uname -s)

ifeq ($(MACHINE),Darwin)
	OPENGL_INC= -FOpenGL
	OPENGL_LIB= -framework OpenGL -framework GLUT -framework Cocoa -lm
else
	OPENGL_INC= -I/usr/include
	OPENGL_LIB= -L/usr/lib64 
	LLDLIBS = $(GLUT) -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm
endif

CXX=g++
COMPILER_FLAGS= -g

INCLUDE= $(OPENGL_INC)
LIBS= $(OPENGL_LIB) $(LLDLIBS)

TARGETS =  main 

SRCS = texture.c terrain.c main.c 

default : $(TARGETS)

OBJS = $(SRCS:.cpp=.o)

%.o: %.c
	$(CXX) -c $(COMPILER_FLAGS) -o $@ $< $(INCLUDE)

main: main.o terrain.o texture.o 
	$(CXX) $(COMPILER_FLAGS) $(LIBS) main.o terrain.o texture.o -o  main

clean:
	rm -f  *.o $(TARGETS) *~ .*.swp ._*
