/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#ifndef __ISO8583_METADATA_H__
#define __ISO8583_METADATA_H__

int procMetadataDATE(unsigned char *c, unsigned char *result);
int procMetadataRND(unsigned char *c, unsigned char *result);
void checkMetadata(unsigned char *c, size_t cSz);

#endif
