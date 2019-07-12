/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#ifndef __ISO8583_UTILS_H__
#define __ISO8583_UTILS_H__

int format(unsigned char *rawData, size_t szRawData, unsigned char *convertedDataOutput, size_t *convertedDataOutputSz, bitFormat_t formatTo);

/*
int setBit(unsigned char *bitmap, unsigned int toalMaxBits, unsigned int bitId, unsinged int newState);
int getBit(unsigned char *bitmap, unsigned int toalMaxBits, unsigned int bitId, unsinged int *state);
*/

#endif
