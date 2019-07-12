/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#ifndef __ISO8583_H__
#define __ISO8583_H__

#include <stddef.h>

#define OK		(1)
#define ERRO	(0)

/* -------------------------------------------- */

typedef enum{
	HEADER = 999,
	MTI = 0,
	BIT1 = 1,
	BIT2 = 2,
	BIT3 = 3,
	BIT4 = 4,
	BIT5 = 5,
	BIT6 = 6,
	BIT7 = 7,
	END = -1
}bitId_t;

typedef enum{
	FIXED, /* UTILIZADO PARA CAMPOS DE TAMANHOS E BITs DEFINIDOS PELO USUARIO/PLUGIN */
	LxVAR,
	LdVAR,
	LLxVAR,
	LLdVAR,
	LLLxVAR,
	LLLdVAR,
	UNKNOWN_TYPE
}bitType_t;

typedef enum{
	EBCDIC,
	ASCII,	/* String */
	BINARY,	/* ex: "123456789" (size = 5) = (12345)b */
	HEX,		/* ex: "313233343536373839" (size = 5) = (31)b (32)b (33)b (34)b (35)b */
	UNKNOWN_FORMAT
}bitFormat_t;

typedef struct _bit_t{
	bitId_t bitId;			/* Numero do bit conforme ISO8583 */
	bitType_t type;		/* Tipo do bit conforme ISO8583 */
	bitFormat_t format;	/* Formato da representadcao do dado dendo do bit */

	size_t szRaw; /* Tamanho do dado lido no arquivo de mensagem (plugins nao precisam definir esta variavel) */
	unsigned char *data; /* Dado lido direto do arquivo de mensagem (plugins nao precisam definir esta variavel) */

	size_t szConverted; /* Tamanho do dado convertido */
	unsigned char *converted; /* Dado convertido */
	size_t lenSz; /* Tamanho do campo de tamanho (usado para LVAR) */

	struct _bit_t *next;
}bit_t;

typedef struct _bitFuncFmt_t{
	bitId_t bitId;
	int (*func)(bit_t *bit);
}bitFuncFmt_t;

typedef struct _pluginRegister_t{
	bitId_t bitId;
	char *funcName;
}pluginRegister_t;

#endif
