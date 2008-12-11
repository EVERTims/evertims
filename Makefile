#
# A very simple makefile for compiling EVERT library
#

CXXFLAGS = -D__`uname` # -Wall -Wstrict-prototypes -O2 -x c 

TARGET = libevert.a

ALL:		$(TARGET)

$(TARGET):	elBSP.o elBeam.o elListener.o elOrientedPoint.o elPathSolution.o \
		elPolygon.o elRay.o elRoom.o elSource.o elVector.o material.o
		ar -r $@ $^
		ranlib $@

#		libtool -static -o $@ $^

clean:
		rm -f $(TARGET) *.o *~ 

# DO NOT DELETE
