#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include <cerrno>
#include <cstdio>

#include <cstring>

class FileWatcher {
public:
	FileWatcher();
        void addFile(char const * pathname);
        void start();
        void stop();
        virtual ~FileWatcher();
private:
	std::string eventsToStr(uint32_t mask);
	int fdInotify;
        int fd;
        bool isRunning;
};

FileWatcher::FileWatcher() : isRunning(false){
	fdInotify = inotify_init();
}

FileWatcher::~FileWatcher() {
    close(fdInotify);
}

void FileWatcher::addFile(char const * pathname) {
   fd = inotify_add_watch(fdInotify, pathname, IN_ALL_EVENTS);
   
   std::perror ("Hou!");

   std::cout << " : "<< fdInotify << " : "<< pathname << " : " << fd << "\n";
}

std::string FileWatcher::eventsToStr(uint32_t mask) {
	std::string str;
	if (mask & IN_ACCESS)
        {
           str.assign("IN_ACCESS");
        }     
	else if (mask & IN_ATTRIB)
        {
           str.assign("IN_ATTRIB");
        }     
	else if (mask & IN_CLOSE_WRITE)
        {
           str.assign("IN_CLOSE_WRITE");
        }     
	else if (mask & IN_CREATE)
        {
           str.assign("IN_CREATE");
        }     
	else if (mask & IN_CLOSE_NOWRITE)
        {
           str.assign("IN_CLOSE_NOWRITE");
        } 
	else if (mask & IN_DELETE)
        {
           str.assign("IN_DELETE");
        }    
        else if (mask & IN_DELETE_SELF)
        {
           str.assign("IN_DELETE_SELF");
        }
        else if (mask & IN_MODIFY)
        {
           str.assign("IN_MODIFY");
        }
	else if (mask & IN_MOVE_SELF)
        {
           str.assign("IN_MOVE_SELF");
        }
	else if (mask & IN_MOVED_FROM)
        {
           str.assign("IN_MOVED_FROM");
        }
	else if (mask & IN_MOVED_TO)
        {
           str.assign("IN_MOVED_TO");
        }
	else if (mask & IN_OPEN)
        {
           str.assign("IN_OPEN");
        }
 
	return str;
}

void FileWatcher::start() {
   isRunning = true;   
   inotify_event * pEvent = 0;
  char buffer[1000*(sizeof(inotify_event) + 1)];  
 
	

  while (isRunning) {
	//std::cout << "In loop \n";

	fd_set rfds;
        struct timeval tv;
        int retval;

        /* Surveiller stdin (fd 0) en attente d'entrees */
        FD_ZERO(&rfds);
        FD_SET(fdInotify, &rfds);

        /* Pendant 5 secondes maxi */
        tv.tv_sec = 2;
        tv.tv_usec = 0;
	
	
	retval = select(fdInotify+1, &rfds, NULL, NULL, &tv);
        /* Considerer tv comme indefini maintenant ! */

        if (retval == -1) 
	{
            perror("select()");
	}        
	else if (retval) // && FD_ISSET(fdInotify, &rfds))
	{
          // std::cout << "Donnees disponibles maintenant\n";
               /* FD_ISSET(0, &rfds) est vrai */
		int nbBytesRead = read(fdInotify, buffer, 1000*(sizeof(inotify_event) + 1));
		//std::cout << "Read " << nbBytesRead << " bytes. " << "\n";
        	int i = 0;
       		 while (i<nbBytesRead) {
	   		pEvent = (inotify_event *) &buffer[i];
			std::string evt = eventsToStr(pEvent->mask);
			if (pEvent->len > 0 && strcmp("IN_CLOSE_WRITE", evt.c_str()) == 0 &&
                    strcmp("Changeant.txt", pEvent->name) == 0)
			{
			std::cout << "Event : " 
				<< pEvent->wd
				<< " | "
				<< eventsToStr(pEvent->mask)
				<< " | "
				<< pEvent->cookie
		         << " | " << pEvent->name << "\n";
			}
			i += sizeof(inotify_event) + pEvent->len;   
		//	std::cout << i << "\n";  
        	}

	}
	else
	{
             // std::cout << "Pas de donnees depuis 2 secondes\n";
	 }     
	
	FD_CLR(fdInotify, &rfds);
        usleep(5000);
   }
}

void FileWatcher::stop() {
   isRunning = false;
}

/*
int main() {
   FileWatcher fw;
   fw.addFile("/home/regis");
   fw.start();
}*/

