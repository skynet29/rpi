include ../../build/Makefile.global


CFLAGS=-Wall -g
LDFLAGS=-lwiringPi -ljson -lpthread
	

INCLUDE_PATH +=:../src/rear
INCLUDE_PATH +=:../src/front
INCLUDE_PATH +=:../src/common

INCLUDE_PATH +=:$(ROOT_DIR)/utils/src
INCLUDE_PATH +=:$(ROOT_DIR)/nova/src
INCLUDE_PATH +=:$(ROOT_DIR)/valve/src
INCLUDE_PATH +=:$(ROOT_DIR)/sensors/src
INCLUDE_PATH +=:$(ROOT_DIR)/bite/src


SRC=RearAgent.cpp \
	StartMotorAction.cpp \
	ValveAction.cpp \
	ValveFillAlternateAction.cpp \
	ValveEmptyAlternateAction.cpp \
	BiteServerApp.cpp \
	NovaVoltageSensor.cpp \
	NovaCurrentSensor.cpp \
	NovaTemperatureSensor.cpp \
	NovaKeepAliveSensor.cpp \
	ValveStateAction.cpp \
	ConsPowerSensor.cpp \
	StopMotorAction.cpp \
	IncMotorPowerAction.cpp

	


LIBUTILS=$(ROOT_DIR)/utils/bin/utils.a
LIBNOVA=$(ROOT_DIR)/nova/bin/nova.a
LIBVALVE=$(ROOT_DIR)/valve/bin/valve.a
LIBSENSORS=$(ROOT_DIR)/sensors/bin/sensors.a
LIBBITE=$(ROOT_DIR)/bite/bin/bite.a

$(LIBUTILS):
	$(DEPEND_RULE)
	
$(LIBNOVA):
	$(DEPEND_RULE)

$(LIBVALVE):
	$(DEPEND_RULE)
	
$(LIBSENSORS):
	$(DEPEND_RULE)	
	
$(LIBBITE):
	$(DEPEND_RULE)		

LIBS= $(LIBNOVA) $(LIBSENSORS) $(LIBUTILS) $(LIBVALVE) $(LIBBITE)


EXE=rearAgent frontAgent


rearAgent: $(OBJ) $(LIBS)
	$(LINK_RULE)
	
frontAgent: FrontAgent.o BiteServerApp.o $(LIBS)
	$(LINK_RULE)	

clean:
	rm -f *.o $(EXE)
	
all: $(EXE)


