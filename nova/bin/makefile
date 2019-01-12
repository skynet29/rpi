include ../../build/Makefile.global


CFLAGS=-Wall
LDFLAGS=-lwiringPi -ljson -lpthread
	
INCLUDE_PATH +=:$(ROOT_DIR)/utils/src
INCLUDE_PATH +=:$(ROOT_DIR)/valve/src

SRC=NovaMotor.cpp \
	MonitoringVolume.cpp
	
EXE=nova.a
TSRC=main.cpp

LIBUTILS=$(ROOT_DIR)/utils/bin/utils.a
LIBVALVE=$(ROOT_DIR)/valve/bin/valve.a

$(LIBUTILS):
	$(DEPEND_RULE)

$(LIBVALVE):
	$(DEPEND_RULE)	
	
LIBS=$(LIBUTILS) $(LIBVALVE)

TOBJ=$(TSRC:.cpp=.o)
TEST=test

$(TEST): $(OBJ) $(TOBJ) $(LIBS)
	$(LINK_RULE)

$(EXE): $(OBJ) $(LIBS)
	$(ARCHIVE_RULE)

clean:
	rm -f $(OBJ) $(EXE) $(TEST) $(TOBJ)
	
all: $(EXE) $(TEST)

