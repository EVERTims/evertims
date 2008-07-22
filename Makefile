#
# A very simple makefile for compiling EVERTims
#

EVERTDIR = ../EVERT
OSCDIR = ../OSC

OSX = -D__OSX
# OSX = -D__Linux

CXXFLAGS = -I$(EVERTDIR) -I$(OSCDIR) -ggdb -D_THREAD_SAFE $(OSX) # -Wall -Wstrict-prototypes -O2 -x c 
CFLAGS = -I$(EVERTDIR) -I$(OSCDIR) -ggdb -D_THREAD_SAFE $(OSX) # -Wall -Wstrict-prototypes -O2 -x c 

LDFLAGS= -L$(EVERTDIR) -ggdb 

# for OSX
LDLIBS= -levert -lm -lpthread -framework OpenGL -framework glut 

# for Linux
# LDLIBS= -levert -lm -lpthread -lGLU -lGL -lglut

APPS=ims

ALL:		$(APPS)

ims:		main.o solver.o writer.o reader.o socket.o \
		$(OSCDIR)/OSC-client.o $(OSCDIR)/OSC-timetag.o 
		g++ $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
		rm -f $(APPS) *.o evert/*.o OSC/*.o *~ */*~

# DO NOT DELETE
