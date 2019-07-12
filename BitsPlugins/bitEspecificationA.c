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
#include <ISO8583.h>

const pluginRegister_t EspecA[] = {{HEADER, "PG_HEADER"}, {BIT3, "PG_bit3"}, {BIT4, "PG_b4"}, {END, NULL}};

const pluginRegister_t * registerPlugin(void)
{
	printf("DENTRO DA FUNCAO DE REGISTRO DO PLUGIN\n");
	return(EspecA);
}

/* ===========================
 * LER CAPITULO "Standard Message Header" DO MANUAL DA VISA BASE-I PARA IMPLEMENTAR VISA.
 * ===========================
 */
int PG_HEADER(bit_t *bit)
{
	printf("DENTRO DA FUNCAO DE FORMATACAO DE HEADER DO PLUGIN\n");
	return(OK);
}

int PG_bit3(bit_t *bit)
{
	fprintf(stderr, "DENTRO DA FUNCAO DE FORMATACAO DO BIT3: ");

	return(OK);
}

int PG_b4(bit_t *bit)
{
	fprintf(stderr, "DENTRO DA FUNCAO DE FORMATACAO DO BIT4: ");

	return(OK);
}
