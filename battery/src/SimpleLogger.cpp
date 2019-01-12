
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "SimpleLogger.hpp"


void Logger::init()
{
   char buf[64];
   sprintf(buf,"BatteryMsg-%04d.log", m_counter);
   m_current = fopen(buf, "w");
}

void Logger::log(char const * str)
{
   if (m_current == 0) { init(); }
   char *p = const_cast<char*>(str);
   while (*p != '\0')
   {
      fwrite(p, 1, 1, m_current); ++p;
   }
   fflush(m_current);
	      
   time_t now; time(&now);
   if (m_prec != (time_t)-1 && (now - m_prec) > 15)
   {
       rotate();
       m_prec = now;
   }

}

void Logger::rotate()
{
    char buf[64];
    int rc = 0;    
    struct stat sta = { 0 };
    ++m_counter;

    if (m_counter == 12) { m_counter = 1; }
    sprintf(buf,"BatteryMsg-%04d.log", m_counter);

    rc = stat(buf, &sta);
    if (rc == 0) 
    {
       remove(buf);
    }
 
    m_next = fopen(buf, "w");
    fclose(m_current);
    m_current = m_next;
}


