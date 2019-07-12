/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#ifndef __ISO8583_TIME_MEASUREMENT_H__
#define __ISO8583_TIME_MEASUREMENT_H__

#include <time.h>

typedef struct timespec tictime_t;

int getTime(tictime_t *t);
double diffTime(tictime_t *start, tictime_t *stop);

#endif
