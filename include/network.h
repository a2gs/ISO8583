/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#ifndef __ISO8583_NETWORK_H__
#define __ISO8583_NETWORK_H__

int sendTrx(char *serverAddr, int port, unsigned char *trx, size_t szTrx);
int receiveTrx(unsigned char *Msg);

#endif
