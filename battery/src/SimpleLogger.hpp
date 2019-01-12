
#ifndef SIMPLELOGGER_HPP_INCLUDED
#define SIMPLELOGGER_HPP_INCLUDED

#include <cstdio>
#include <ctime>

class Logger
{
public:
   Logger() : m_current(0), m_next(0), m_counter(1) {}
   virtual ~Logger(){}

   void log(char const * data);

protected:
   void init();
   void rotate();

private:
   FILE * m_current;
   FILE * m_next;

   int m_counter;
   time_t m_startTime;
   time_t m_prec;
};

#endif /* SIMPLELOGGER_HPP_INCLUDED */

