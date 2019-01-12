
SUBDIRS=utils nova valve sensors bite imu balancing imuAgent powerAgent

all:
	@for i in $(SUBDIRS); do (cd ../$$i/bin; make all); done
	
clean:
	@for i in $(SUBDIRS); do (cd ../$$i/bin; make clean); done	
