/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#include <time.h>
#include <ISO8583.h>
#include <time_measurement.h>

int getTime(tictime_t *t)
{
	if(clock_gettime(CLOCK_REALTIME, t) == -1)
		return(ERRO);

	return(OK);
}

double diffTime(tictime_t *start, tictime_t *stop)
{
	return((stop->tv_sec - start->tv_sec) + (double)(stop->tv_nsec - start->tv_nsec) / (double)1000000000L);
}
