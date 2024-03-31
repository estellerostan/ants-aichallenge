#ifndef TIMER_H_
#define TIMER_H_

/*
    struct for checking how long it has been since the start of the turn.
*/
#ifdef _WIN32 //Windows timer (DON'T USE THIS TIMER UNLESS YOU'RE ON WINDOWS!)
    #include <io.h>
    #include <windows.h>

    struct Timer
    {
        clock_t startTime, currentTime;

        Timer()
        {

        };

        void Start()
        {
            startTime = clock();
        };

        double GetTime()
        {
            currentTime = clock();

            return (double)(currentTime - startTime);
        };
    };

#else //Mac/Linux Timer
#include <sys/time.h>

    struct Timer
    {
        timeval timer;
        double startTime, currentTime;

        Timer()
        {

        };

        //starts the timer
        void Start()
        {
            gettimeofday(&timer, NULL);
            startTime = timer.tv_sec+(timer.tv_usec/1000000.0);
        };

        //returns how long it has been since the timer was last started in milliseconds
        double GetTime()
        {
            gettimeofday(&timer, NULL);
            currentTime = timer.tv_sec+(timer.tv_usec/1000000.0);
            return (currentTime-startTime)*1000.0;
        };
    };
#endif


#endif //TIMER_H_
