CXXFLAGS =	-O3 -g0 -Wall -fmessage-length=0

OBJS_R=	bigi.o	bigr.o

LIBS =

TARGET_R =	libbign.dll

$(TARGET_R):	$(OBJS_R)
	$(CXX) -shared -o $(TARGET_R) $(OBJS_R) $(LIBS)


all:	$(TARGET_R)

clean:
	rm -f $(TARGET_R) $(OBJS_R)
