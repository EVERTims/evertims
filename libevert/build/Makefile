#
# A very simple makefile for compiling EVERT library
#

CXXFLAGS = -D__`uname` 

TARGET = libevert.a

ALL:		$(TARGET)

$(TARGET):	elBSP.o elBeam.o elListener.o elOrientedPoint.o elPathSolution.o \
		elPolygon.o elRay.o elRoom.o elSource.o elVector.o material.o
		ar -r $@ $^
		ranlib $@

clean:
		rm -f $(TARGET) *.o *~ 

