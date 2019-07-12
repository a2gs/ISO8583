/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#include <ISO8583.h>
#include <iso8583Utils.h>

int bitDefaultFormat(bit_t *bit)
{
#define UNDCNSTBUFF_SZ	(10000)

	unsigned char underconstructionOutputBuff[UNDCNSTBUFF_SZ] = {0};
	unsigned char *walkerUndCnstBf = NULL;
	unsigned char bitSzStr[15] = {0};
	size_t bitSzStrConvertedSz = 0;

	walkerUndCnstBf = underconstructionOutputBuff;
	memset(underconstructionOutputBuff, 0, UNDCNSTBUFF_SZ);
	memset(bitSzStr, 0, sizeof(bitSzStr));

	fprintf(stderr, "Formatando bit [%d]:\n", bit->bitId);
	fprintf(stderr, "\nData (%zu) [%.*s]:\n", bit->szRaw, (int)bit->szRaw, bit->data);
	fprintf(stderr, "\nType [%d]:\n", bit->type);
	fprintf(stderr, "\nFormat [%d]:\n", bit->format);

	/* Formatando o tamanho. Campos de tamanho fixo nao possuem tamanho */
	if((bit->type != FIXED) && (bit->type != UNKNOWN_TYPE)){
		snprintf(bitSzStr, sizeof(bitSzStr), "%zu", bit->szRaw);

		format(bitSzStr, bit->lenSz, walkerUndCnstBf, &bitSzStrConvertedSz, (bit->type == LxVAR || bit->type == LLxVAR || bit->type == LLLxVAR) ? BINARY : ASCII);
		walkerUndCnstBf += bitSzStrConvertedSz;
	}

	/* Formatando o dado especificado com o formato correto */
	if(format(bit->data, bit->szRaw, walkerUndCnstBf, &bit->szConverted, bit->format) == ERRO){
		return(ERRO);
	}

	/* ALOCAR MEMORIA EM bit->converted */
	/* COPIAR DE walkerUndCnstBf PARA bit->converted */

	return(OK);
}


#if 0
int bitMTI(bit_t *bit)
{
	return(OK);
}

int bit1(bit_t *bit)
{
	return(OK);
}

/* DE 2-Primary Account Number (PAN) */
int bit2(bit_t *bit)
{
	/*
	-* DUMMY DATA *-
	fprintf(stderr, "Dados lidos para bit2: data[%s] szRaw[%zu] type[AINDA NAO SETADO]\n", bit->data, bit->szRaw);

	bit->szConverted = 8;
	bit->converted = malloc(8); -* fazer free dessaporra *-
	bit->type = FIXED;
	bit->format = ASCII;
	memcpy(bit->converted, "88888888", 8);
	*/

	/* Este codigo pode ser correto .... depois acertalo para formatar o DE2
	uint16_t LL;

	if(bit->szRaw > 19)
		return(ERRO);

	fprintf(stderr, "Formatando BIT2 padrao.\n");

	bit->converted = malloc(bit->szRaw + 2);
	if(bit->converted == NULL){
		fprintf(stderr, "Erro alocando memoria para formatacao do DE2.\n");
		return(ERRO);
	}

	LL = (uint16_t)bit->szRaw;
	memcpy(bit->converted, &LL, 2);
	memcpy(&bit->converted[2], bit->data, bit->szRaw);
	*/

	return(OK);
}

/* DE 3-Processing Code */
int bit3(bit_t *bit)
{
	return(OK);
}

/* DE 4-Amount, Transaction */
int bit4(bit_t *bit)
{
	return(OK);
}

/* DE 5-Amount, Settlement */
int bit5(bit_t *bit)
{
	return(OK);
}
#endif
