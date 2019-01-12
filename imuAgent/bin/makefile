include ../../build/Makefile.global


CFLAGS=-Wall
LDFLAGS=-ljson
	
INCLUDE_PATH +=:$(ROOT_DIR)/imu/src
INCLUDE_PATH +=:$(ROOT_DIR)/utils/src


SRC=ImuAgent.cpp
	
EXE=imuAgent
TSRC=main.cpp

LIBIMU=$(ROOT_DIR)/imu/bin/imu.a
LIBUTILS=$(ROOT_DIR)/utils/bin/utils.a

$(LIBIMU):
	$(DEPEND_RULE)
	
$(LIBUTILS):
	$(DEPEND_RULE)
	
LIBS=$(LIBIMU) $(LIBUTILS)



$(EXE): $(OBJ) $(LIBS)
	$(LINK_RULE)

clean:
	rm -f $(OBJ) $(EXE) 
	
all: $(EXE)

