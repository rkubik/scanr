#include "util/time.h"

unsigned long timespec_milliseconds(const struct timespec *tp)
{
    unsigned long ms = 0;

    if (!tp) {
        goto done;
    }

    ms = (tp->tv_sec*MS_IN_S) + (tp->tv_nsec/MS_IN_NS);      
done:
    return ms;
}

void timespec_diff(const struct timespec *tp1,
                   const struct timespec *tp2,
                   struct timespec *tp3)
{
    if (tp1 && tp2 && tp3) {
        if ((tp1->tv_nsec - tp2->tv_nsec) < 0) {
            tp3->tv_sec = tp1->tv_sec - tp2->tv_sec - 1;
            tp3->tv_nsec = tp1->tv_nsec - tp2->tv_nsec + BILLION;
        } else {
            tp3->tv_sec = tp1->tv_sec - tp2->tv_sec;
            tp3->tv_nsec = tp1->tv_nsec - tp2->tv_nsec;
        }
    }
}
