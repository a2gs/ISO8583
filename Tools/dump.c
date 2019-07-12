/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#ifndef __ISO8583_DUMP_H__
#define __ISO8583_DUMP_H__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int dump(unsigned char *buf, size_t szbuf, unsigned char *out, size_t szout)
{
	size_t i = 0;
	unsigned int j = 0;
	unsigned  char *p1 = NULL, *p2 = NULL, *end = out;
	unsigned char fmtAux[3] = {0};

	/*
		          1         2         3         4         5         6    
		0123456789012345678901234567890123456789012345678901234567890123456789
		xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xxTaaaaaaaaaaaaaaaaN
	 */

#define MASK		("                                               \t                \n")
#define MASK_SZ	(sizeof(MASK))

	if(ceil((float)szbuf/16.0) > ((float)szout/(float)MASK_SZ))
		return(-1);

	p1 = &out[0];
	p2 = &out[48];

	for(i = 0; i < szbuf; ){

		memcpy(p1, MASK, MASK_SZ);

		for(j = 0; j < 16 && i < szbuf; j++, i++){
			sprintf(fmtAux, "%02x", buf[i]);
			*p1 = fmtAux[0];
			*(p1+1) = fmtAux[1];
			p1 += 3;

			if(isprint(buf[i])) *p2 = buf[i];
			else                *p2 = '.';
			p2++;
		}

		end = p2 + 1;
		p1 += sizeof("aaaaaaaaaaaaaaaa");
		p2 += sizeof("Nxx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx");
	}

	*end = '\0';

	return(0);
}

#if 0
int main(int argc, char *argv[]){
	unsigned char x[] = "abcdefghjlmnopqrstuvxz1234567890" "\xff" "\x32" "\x01" "abcdef";
	unsigned char y[270] = {0};

	int i;

	for(i=0; i<sizeof(x); i++)
		printf("%02x ", x[i]);

	printf("\n\n");

	dump((unsigned char *)x, sizeof(x), y, sizeof(y));

	printf("%s\n",y);

	return(0);
}
#endif

#endif
