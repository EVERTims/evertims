#
# A very simple makefile for compiling EVERT library
#

OSX = -D__OSX

CXXFLAGS = -I$(GLT) -ggdb $(OSX) # -Wall -Wstrict-prototypes -O2 -x c 
# CFLAGS = -I$(GLT) -ggdb $(OSX) # -Wall -Wstrict-prototypes -O2 -x c 

TARGET = libevert.a

ALL:		$(TARGET)

$(TARGET):	elBSP.o elBeam.o elListener.o elOrientedPoint.o elPathSolution.o \
		elPolygon.o elRay.o elRoom.o elSource.o elVector.o material.o
		libtool -static -o $@ $^

clean:
		rm -f $(TARGET) *.o *~ 

# DO NOT DELETE
