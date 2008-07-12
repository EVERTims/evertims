#
# A very simple makefile for compiling EVERTims
#

OSX = -D__OSX
# OSX = -D__Linux

CXXFLAGS = -I$(GLT) -ggdb -D_THREAD_SAFE $(OSX) # -Wall -Wstrict-prototypes -O2 -x c 
CFLAGS = -I$(GLT) -ggdb -D_THREAD_SAFE $(OSX) # -Wall -Wstrict-prototypes -O2 -x c 

LDFLAGS= -ggdb -Levert

# for OSX
LDLIBS= -levert -lm -lpthread -framework OpenGL -framework glut 

# for Linux
# LDLIBS= -levert -lm -lpthread -lGLU -lGL -lglut

APPS=ims

ALL:		$(APPS)

ims:		main.o material.o solver.o writer.o reader.o socket.o \
		OSC/OSC-client.o OSC/OSC-timetag.o 
		g++ $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
		rm -f $(APPS) *.o evert/*.o OSC/*.o *~ */*~

# DO NOT DELETE
