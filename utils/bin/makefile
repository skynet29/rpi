include ../../build/Makefile.global


CFLAGS=-Wall -g
LDFLAGS=-lpthread -ljson
	

SRC=DThread.cpp \
	DTimer.cpp \
	DApplication.cpp \
	DSequencer.cpp \
	DSocket.cpp \
	DServer.cpp \
	DSelect.cpp \
	DServerApp.cpp \
	DLogger.cpp \
	DClient.cpp

LIBS=	

EXE=utils.a

TSRC=test_server.cpp
TOBJ=$(TSRC:.cpp=.o)
TEST=test_server

TSRC2=test_seq.cpp
TOBJ2=$(TSRC2:.cpp=.o)
TEST2=test_seq

$(EXE): $(OBJ)
	$(ARCHIVE_RULE)
	
$(TEST): $(TOBJ) $(OBJ)
	$(LINK_RULE)

$(TEST2): $(TOBJ2) $(OBJ)
	$(LINK_RULE)

clean:
	rm -f $(OBJ) $(EXE) $(TOBJ) $(TEST) $(TOBJ2) $(TEST2)
	
all: $(EXE) $(TEST) $(TEST2)


	

