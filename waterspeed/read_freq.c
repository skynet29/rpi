#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_LENGTH 32

void main(void) {
	FILE * fp_freq;

	fp_freq = fopen("/dev/gpiofreq0","r");
	if(fp_freq == NULL) {
		perror("/dev/gpiofreq0");
		exit(EXIT_FAILURE);
	}

	char buffer[BUFFER_LENGTH];
	int frequency;

	while(1) {
		if (fgets(buffer, BUFFER_LENGTH, fp_freq) != NULL) {
			if (sscanf(buffer, "%d", & frequency) == 1) {
				fprintf(stdout, "Frequency: %d Hz\n", frequency);
			}
		}
		usleep(500000);
	}
}
