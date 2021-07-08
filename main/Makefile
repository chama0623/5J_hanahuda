MYLIBDIR = ../mylib
MYLIB    = $(MYLIBDIR)/mylib.a
CFLAGS   = -I$(MYLIBDIR)

CC = gcc
CCFLAGS = -Wall -I/usr/include/opengl
LD = gcc
LDFLAGS =
LIBS = -g -O0 -lglpng -lglut32 -lglu32 -lopengl32 -lm #myicon.o

all:	s c

s:		server.o hanahuda.o
		$(CC) -o $@ $^ $(MYLIB)	$(LIBS)	

c:		client.o hanahuda.o
		$(CC) -o $@ $^ $(MYLIB) $(LIBS)

server.o client.o:	hanahuda.h

clean:
		 $(RM) s c *.o *~