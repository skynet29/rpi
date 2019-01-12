include ../../build/Makefile.global


CFLAGS=-Wall
LDFLAGS=-ljson
	
INCLUDE_PATH +=:$(ROOT_DIR)/utils/src


SRC=CanMsg.cpp \
	CANReceiver.cpp \
	FileWatcher.cpp \
	SimpleLogger.cpp \
        CANBatteryCodec.cpp
	
EXE=battery.a

LIBUTILS=$(ROOT_DIR)/utils/bin/utils.a

$(LIBUTILS):
	$(DEPEND_RULE)
	
LIBS=$(LIBUTILS)


test: main.o $(OBJ) $(LIBS)
	$(LINK_RULE)

$(EXE): $(OBJ) $(LIBS)
	$(ARCHIVE_RULE)

clean:
	rm -f *.o $(EXE) test
	
all: $(EXE) test

