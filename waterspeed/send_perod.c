#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <memory.h>

static int verbose = 0; // if level = 1, traces are displayed when messsages are sent

/**
 * Init the UDP connection
 */
int init_udp(struct in_addr *localInterface, struct sockaddr_in *groupSock) {
  int sd;
  // Create a datagram socket on which to send
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sd < 0) perror("Opening datagram socket error");
  else if (verbose==1) printf("Opening the datagram socket...OK.\n");
  // Initialize the group sockaddr structure with a
  // group address of 234.2.3.4 and port 7067
  memset((char *)groupSock, 0, sizeof(groupSock));
  groupSock->sin_family = AF_INET;
  groupSock->sin_addr.s_addr = inet_addr("234.2.3.4");
  groupSock->sin_port = htons(7067);
  // Set local interface for outbound multicast datagrams.
  // The IP address specified must be associated with a local,
  // multicast capable interface.
  localInterface->s_addr = inet_addr("192.168.1.124");
  if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)localInterface, sizeof(localInterface)) < 0) {
    perror("Setting local interface error");
  }
  else if (verbose==1) printf("Setting the local interface...OK.\n");
  return sd;
}

/**
 * Send a JSON message on the UDP connection
 */
int send_udp(char *databuf, int sd, struct in_addr localInterface, struct sockaddr_in groupSock) {
  int datalen = strlen(databuf);
  //printf("%s -> %d\n", databuf, datalen);
  // Send a message to the multicast group specified by the groupSock sockaddr structure
  if (sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0) {
    perror("Sending datagram message error");
  }
  return datalen;
}

/**
 * Open the device corresponding to the given sensor id (id=1, 2, ...)
 */
FILE *open_device(int device_id) {
  FILE *cur_device ;
  char device_name[32];
  snprintf(device_name,32, "/dev/gpiofreq%d", device_id-1);
  cur_device = fopen(device_name, "r");
  if (cur_device==NULL) perror(device_name);
  else if (verbose==1) printf("Reinit device %s\n", device_name);
  return cur_device;
}

/**
 * Init the divices where the gpio will be read
 */
FILE **init_devices(int nb_devices) {
  FILE **devices;
  devices = malloc(sizeof(FILE*)*nb_devices);
  int i = 0;
  for(i=0;i<nb_devices;i++) {
    devices[i] = open_device(i+1);
  }
  return devices;
}

/**
 * Reinit the devices
 */
 void reinit_devices(int nb_devices, FILE **devices) {
   int i = 0;
   for(i=0;i<nb_devices;i++) {
     // close the device
     fclose(devices[i]);
     // reopen it
     devices[i] = open_device(i+1);
   }
 }

/**
 * Acquisition for all devices at the given recurrence
 */
void devices_acquisition(int msg_rec, int reinit_timeout, int nb_devices, FILE **devices, int sd, struct in_addr localInterface, struct sockaddr_in groupSock) {
    struct timeval prev_t;
    struct timeval now_t;
    int cpt_rec = 0;
    int msg_max = 512;
    char buffer[32];
    char period_msg[64];
    char message[msg_max];
    while(1) {
      usleep(1000);
      gettimeofday(&now_t, NULL);
      if (prev_t.tv_sec!= 0 ||  prev_t.tv_usec!= 0) {
        long long elapsed;
        elapsed = (now_t.tv_sec-prev_t.tv_sec)*1000000LL + now_t.tv_usec - prev_t.tv_usec;
        if (elapsed>msg_rec) {
          long long tref = (long long)now_t.tv_sec*1000LL + now_t.tv_usec/1000;
          // get the acquired periods on all devices
          snprintf(message, msg_max, "{\"t\":%Ld,", tref);
          int id;
          FILE *cur_device ;
          for(id=1;id<=nb_devices;id++) {
            cur_device = devices[id-1];
            int period;
            if (cur_device!=NULL) {
              if (fgets(buffer,32,cur_device)!=NULL) {
                if (sscanf(buffer, "%d", &period)==1) {
                  // period OK
                }
                else period = -2; // format error : should never happen
              }
              else period = -1;
            }
            else period = -3;
            if (verbose==1) {
              float frequency = 0;
              if (period>0) frequency = (float)1000000 / (float)period;
              snprintf(period_msg, 64, "\"v%d\":{\"T\":%d,\"f\":%f}", id, period, frequency);
            }
            else snprintf(period_msg, 64, "v%d:{\"T\":%d}", id, period);
            if (id<nb_devices) strcat(period_msg, ",");
            strcat(message, period_msg);
          }
          strcat(message, "}");
          // Send the built message
          // Example: {"tref":165235648, "speed":[{"id":1, "period":152},...,{"id":7, "period":564}]}
          prev_t = now_t;
          if (reinit_timeout>0) {
            cpt_rec++;
            if (cpt_rec>reinit_timeout) {
              reinit_devices(nb_devices, devices);
              cpt_rec = 0;
            }
          }
          int msg_length = send_udp(message, sd, localInterface, groupSock);
          if (verbose==1) printf("%s\n\n",message);
        }
      }
    }
}

/**
 * Main method
 * Compilation: cc -o send_period send_perod.c
 */
int main(int argc, char* argv[]) {
  if (argc<4) {
    printf("usage: %s <nb_sensors> <message_recurrence (ms)> <reinit_nb_recurrences> [-v]\n", argv[0]);
    exit(0);
  }
  if (argc==5 && strcmp(argv[4],"-v")==0) verbose = 1;
  // open the acquisition file (means gpio acquisition will start)
  int nb_devices;
  if (sscanf(argv[1], "%d", &nb_devices)==1) {
    if (verbose==1) printf("Number of sensors = %d\n", nb_devices);
  }
  else nb_devices = 1;
  FILE **devices = init_devices(nb_devices);
  // init the udp socket
  struct in_addr localInterface;
  struct sockaddr_in groupSock;
  int sd = init_udp(&localInterface, &groupSock);
  // scan each 100 ms the aquired periods on all init gpio
  int msg_rec;
  if (sscanf(argv[2], "%d", &msg_rec)==1) {
    msg_rec = msg_rec*1000;
    if (verbose==1) printf("Message recurrence = %d ms\n", msg_rec/1000);
  }
  else msg_rec = 100000; // message recurrence in microseconds (default = 10 Hz)
  int reinit_timeout;
  if (sscanf(argv[3], "%d", &reinit_timeout)==1) {
    if (verbose==1) printf("Nb of recurrences between each devices reinit = %d ms\n", msg_rec/1000);
  }
  else reinit_timeout = 0; // no devices is reinit by default
  devices_acquisition(msg_rec, reinit_timeout, nb_devices, devices, sd, localInterface, groupSock);
}
