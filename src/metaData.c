/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ISO8583.h>

int procMetadataDATE(unsigned char *c, unsigned char *result)
{
	char mask[100] = {0};
	char *cWalker = NULL;
	struct tm *tmTime = NULL;
	time_t rawtime;

	c += 8; /* to jump "<<<DATE " */

	for(cWalker = (char *)c; cWalker != NULL; ){
		if(strncmp(cWalker, "DD", 2) == 0){
			strcat(mask, "%d");
			cWalker += 2;
		}else if(strncmp(cWalker, "MM", 2) == 0){
			strcat(mask, "%m");
			cWalker += 2;
		}else if(strncmp(cWalker, "YYYY", 4) == 0){
			strcat(mask, "%Y");
			cWalker += 4;
		}else if(strncmp(cWalker, "YY", 2) == 0){
			strcat(mask, "%y");
			cWalker += 2;
		}else if(strncmp(cWalker, "HH", 2) == 0){
			strcat(mask, "%H");
			cWalker += 2;
		}else if(strncmp(cWalker, "mm", 2) == 0){
			strcat(mask, "%M");
			cWalker += 2;
		}else if(strncmp(cWalker, "SS", 2) == 0){
			strcat(mask, "%S");
			cWalker += 2;
		}else if(strncmp(cWalker, ">>>", 3) == 0)
			break;
	}

	time(&rawtime);
	tmTime = localtime(&rawtime);
	strftime((char *)result, 100, mask, tmTime);

	return(OK);
}

int procMetadataRND(unsigned char *c, unsigned char *result)
{
	long mod = 0;
	char *cut = NULL;

	c += 7; /* "<<<RND " */

	cut = strchr((char *)c, '>');
	if(cut != NULL) *cut = '\0';
	else            return(ERRO);

	srand(time(0));
	mod = (long)pow(10.0, (double)atoi((char *)c));

	sprintf((char *)result, "%0*d", atoi((char *)c), (int)(rand() % mod));

	return(OK);
}

void checkMetadata(unsigned char *c, size_t cSz)
{
	unsigned char *end = NULL;
	unsigned char buffResult[100] = {0};

	if(*c == '<' && *(c+1) == '<' && *(c+2) == '<'){ /* Provavel metadata */

		end = c + strlen((const char*) c); /* Verificando o final */

		if(*(end-1) == '>' && *(end-2) == '>' && *(end-3) == '>'){

			/* PROCESSAR METADATA */
			if (strncmp((const char *)c+3, "DATE", 4) == 0){
				procMetadataDATE(c, buffResult);
			}else if(strncmp((const char *)c+3, "RND", 3) == 0){
				procMetadataRND(c, buffResult);
			}

			strcpy((char *)c, (const char *)buffResult);
		}
	}
}
