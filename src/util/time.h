#ifndef SCANR_TIME_H
#define SCANR_TIME_H

#include <time.h>

#define MS_IN_S  1000
#define MS_IN_NS 1000000
#define BILLION  1000000000

/**
 * @brief Convert timespec structure to milliseconds.
 *
 * @param[in] tp - Pointer to a timespec structure
 *
 * @return Milliseconds or 0 on bad input
 */
unsigned long timespec_milliseconds(const struct timespec *tp);

/**
 * @brief Computes tp3 = tp1 - tp2.
 *
 * @param[in] tp1 - Reference timespec
 * @param[in] tp2 - Timespec to subtract
 * @param[out] tp3 - Timespec result
 */
void timespec_diff(const struct timespec *tp1,
                   const struct timespec *tp2,
                   struct timespec *tp3);

#endif
