include ../../build/Makefile.global


CFLAGS=-Wall
LDFLAGS=-lwiringPi -ljson
	
INCLUDE_PATH +=:$(ROOT_DIR)/utils/src

SRC=ValveController.cpp
TSRC=main.cpp

TOBJ=$(TSRC:.cpp=.o)

LIBUTILS=$(ROOT_DIR)/utils/bin/utils.a

$(LIBUTILS):
	$(DEPEND_RULE)
	
LIBS=$(LIBUTILS)	

EXE=valve.a
TEST=test


$(EXE): $(OBJ) $(LIBS)
	$(ARCHIVE_RULE)

$(TEST): $(TOBJ) $(OBJ) $(LIBS)
	$(LINK_RULE)

clean:
	rm -f $(OBJ) $(EXE) $(TOBJ) $(TEST)
	
all: $(EXE) $(TEST)

