/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#ifndef __ISO8583_TOOLS_H__
#define __ISO8583_TOOLS_H__

int dump(unsigned char *buf, size_t szbuf, unsigned char *out, size_t szout);
void * ebcdic2ascii(void *dest, const void *srce, size_t count);
void * ascii2ebcdic(void *dest, const void *srce, size_t count);

#endif
