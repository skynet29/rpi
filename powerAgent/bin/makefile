include ../../build/Makefile.global


CFLAGS=-Wall
LDFLAGS=-lwiringPi -ljson
	
INCLUDE_PATH +=:$(ROOT_DIR)/utils/src


SRC=powerAgent.cpp
	
EXE=powerAgent

LIBUTILS=$(ROOT_DIR)/utils/bin/utils.a

	
$(LIBUTILS):
	$(DEPEND_RULE)
	
LIBS=$(LIBUTILS)



$(EXE): $(OBJ) $(LIBS)
	$(LINK_RULE)

clean:
	rm -f $(OBJ) $(EXE) 
	
all: $(EXE)

