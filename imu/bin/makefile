include ../../build/Makefile.global


CFLAGS=-Wall
LDFLAGS=
	

SRC=RTFusion.cpp \
	RTFusionKalman4.cpp \
	RTFusionRTQF.cpp \
	RTIMU.cpp \
	RTIMUGD20HM303D.cpp \
	RTIMUGD20M303DLHC.cpp \
	RTIMUHal.cpp \
	RTIMULSM9DS0.cpp \
	RTIMUMPU9150.cpp \
	RTIMUNull.cpp \
	RTIMUSettings.cpp \
	RTMath.cpp
	
EXE=imu.a

LIBS=

$(EXE): $(OBJ) $(LIBS)
	$(ARCHIVE_RULE)

clean:
	rm -f $(OBJ) $(EXE) $(TOBJ)
	
all: $(EXE)

