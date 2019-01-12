include ../../build/Makefile.global


CFLAGS=-Wall
LDFLAGS=-lwiringPi -ljson -lpthread
	
INCLUDE_PATH +=:$(ROOT_DIR)/utils/src	

SRC=TMP102Sensor.cpp \
	ads1015.cpp \
	Sensor.cpp \
	I2CSensor.cpp \
	SensorManager.cpp
	
EXE=sensors.a

LIBUTILS=$(ROOT_DIR)/utils/bin/utils.a

$(LIBUTILS):
	$(DEPEND_RULE)
	
LIBS=$(LIBUTILS)

test_temp: $(OBJ) $(LIBS) test_temp.o
	$(LINK_RULE)
	
test_water: $(OBJ) $(LIBS) test_water.o
	$(LINK_RULE)

test_mngr: $(OBJ) $(LIBS) test_mngr.o
	$(LINK_RULE)
	
TEST=test_temp test_water test_mngr	

$(EXE): $(OBJ) $(LIBS)
	$(ARCHIVE_RULE)

clean:
	rm -f *.o $(EXE) $(TEST)
	
all: $(EXE) $(TEST)

