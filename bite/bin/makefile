include ../../build/Makefile.global


CFLAGS=-Wall
LDFLAGS=-ljson -lpthread
	
INCLUDE_PATH +=:$(ROOT_DIR)/utils/src
INCLUDE_PATH +=:$(ROOT_DIR)/sensors/src


SRC=BiteNode.cpp \
	BiteMessage.cpp \
	SensorBiteNode.cpp
	
EXE=bite.a

LIBUTILS=$(ROOT_DIR)/utils/bin/utils.a

$(LIBUTILS):
	$(DEPEND_RULE)
	
LIBSENSORS=$(ROOT_DIR)/sensors/bin/sensors.a

$(LIBSENSORS):
	$(DEPEND_RULE)
		
LIBS=$(LIBUTILS) $(LIBSENSORS)


test: main.o $(OBJ) $(LIBS)
	$(LINK_RULE)

$(EXE): $(OBJ) $(LIBS)
	$(ARCHIVE_RULE)

clean:
	rm -f *.o $(EXE) test
	
all: $(EXE) test

