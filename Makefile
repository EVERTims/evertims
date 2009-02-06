#
# A very simple makefile for compiling EVERTims
#

<<<<<<< .mine
EVERTDIR = ../EVERT/trunk
OSCDIR = ../OSC
=======
EVERTDIR = ../../EVERT/trunk
OSCDIR = ../../OSC
>>>>>>> .r13

# for OSX
LDLIBS= -levert -lm -lpthread -framework OpenGL -framework glut 

# for Linux
# LDLIBS= -levert -lm -lpthread -lGLU -lGL -lglut

#
# No more edits should be needed below this line
#

OSX = -D__`uname`

CXXFLAGS = -I$(EVERTDIR) -I$(OSCDIR) -ggdb -D_THREAD_SAFE $(OSX) 

LDFLAGS= -L$(EVERTDIR) -ggdb 

APPS=ims

ALL:		$(APPS)

ims:		main.o solver.o writer.o reader.o socket.o \
		$(OSCDIR)/OSC-client.o $(OSCDIR)/OSC-timetag.o 
		g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
		rm -f $(APPS) *.o evert/*.o OSC/*.o *~ */*~

# DO NOT DELETE
