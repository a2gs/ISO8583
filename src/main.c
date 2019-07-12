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

#include <ISO8583.h>
#include <time_measurement.h>
#include <plugins.h>
#include <Tools.h>
#include <network.h>
#include <defaultBitFmt.h>

/* -------------------------------------------- */

int bitMTI(bit_t *bit);
int bit1(bit_t *bit);
int bit2(bit_t *bit);
int bit3(bit_t *bit);
int bit4(bit_t *bit);
int bit5(bit_t *bit);

/* -------------------------------------------- */

bitFuncFmt_t fmtBit[] = {{HEADER, NULL},
                         {MTI, bitDefaultFormat},
                         {BIT1, bitDefaultFormat},
                         {BIT2, bitDefaultFormat},
                         {BIT3, bitDefaultFormat},
                         {BIT4, bitDefaultFormat},
                         {BIT5, bitDefaultFormat},
                         {BIT6, NULL},
                         {END, NULL}};

/* -------------------------------------------- */

void usage(char *exec)
{
	printf("Usage: %s [-w x] -i msg_to_send_file [-o received_msg_file] [-b plugins_path] -s server_ip -p server_port [-c msg_to_send_templeta_file]\n", exec);

	printf("\t-w: (optional) Define the wait time. 0=perpetual, x=x seconds, omitted=no wait respose (just send msg).\n");
	printf("\t-i: Path to message file.\n");
	printf("\t-o: (optional) Path to response message file (this file will be created). There is no efect if -w doesn't defined.\n");
	printf("\t-b: (optional) Path to plugins.\n");
	printf("\t-s: OLTP server IP or DNS.\n");
	printf("\t-p: OLTP server port.\n");
	printf("\t-c: (optional) Create a empty template of input msg file (this file will be created).\n\n");
}

bit_t * addNewSlot(bit_t **MsgHeadBitList)
{
	bit_t *new = NULL;
	bit_t *walker = NULL;

	new = malloc(sizeof(bit_t));
	if(new == NULL){
		fprintf(stderr, "Erro em alocar memoria para novo slot.\n");
		return(NULL);
	}

	memset(new, 0, sizeof(bit_t));
	new->next = NULL;

	/* Caso seja o primeiro elemento da lista */
	if(*MsgHeadBitList == NULL)
		*MsgHeadBitList = new;
	else{
		for(walker = *MsgHeadBitList; walker->next != NULL; walker = walker->next);
		walker->next = new;
	}

	return(new);
}

int assemblyMsg(bit_t *MsgHeadBitList, unsigned char *trx, size_t trxMaxSz, size_t *trxSzFormatted)
{
	bit_t *walker = NULL;
	size_t szTot = 0;
	unsigned char *trxWalker = NULL;

	for(walker = MsgHeadBitList, trxWalker = trx, *trxSzFormatted = 0; walker != NULL; walker = walker->next){
		szTot = walker->szConverted + walker->lenSz;
		memcpy(trxWalker, walker->converted, szTot);
		trxWalker += szTot;
		*trxSzFormatted += szTot;
	}

	return(OK);
}

int insertBit(bit_t **MsgHeadBitList, unsigned char *lineData, unsigned char *lineType, unsigned char *lineFormat)
{
	unsigned char *equal;
	unsigned char bitId[50] = {0};
	bit_t *newBit;

	/* Allocating a new bit_t slot */
	newBit = addNewSlot(MsgHeadBitList);
	if(newBit == NULL){
		fprintf(stderr, "Nao foi possivel adicionar um novo elemento para a linha: [%s]\n", lineData);
		return(ERRO);
	}

	/* Parsing Data line (bit Id and valeu) */
	equal = strchr(lineData, '=');
	if(equal == NULL){
		fprintf(stderr, "Nao localizado sinal de igual na linha de dado [%s]!\n", lineData);
		return(ERRO);
	}

	strncpy(bitId, lineData, equal-lineData);
	bitId[equal-lineData] = '\0';

	if(strcmp(bitId, "HEADER") == 0)   newBit->bitId = HEADER;
	else if(strcmp(bitId, "MTI") == 0) newBit->bitId = MTI;
	else                               newBit->bitId = atoi(bitId);

	newBit->szRaw = strlen(lineData) - (equal-lineData+1);
	newBit->data = malloc(newBit->szRaw);
	memcpy(newBit->data, equal+1, newBit->szRaw);

	/* Parsing Type line */
	equal = strchr(lineType, '=');
	if(equal == NULL){
		fprintf(stderr, "Nao localizado sinal de igual na linha de tipo [%s] do bit [%s]!\n", lineType, bitId);
		return(ERRO);
	}

	if(strncmp(equal+1, "FIXED", 5) == 0){
		newBit->type = FIXED;
		newBit->szRaw = atoi(equal+1+5+1); /* Redefinindo szRaw para um tamanho fixo (sample: TYPE=FIXED 1234) */
		newBit->lenSz = 0;
	}else if(strcmp(equal+1, "LxVAR") == 0){
		newBit->type = LxVAR;
		newBit->lenSz = 1;
	}else if(strcmp(equal+1, "LdVAR") == 0){
		newBit->type = LdVAR;
		newBit->lenSz = 1;
	}else if(strcmp(equal+1, "LLxVAR") == 0){
		newBit->type = LLxVAR;
		newBit->lenSz = 2;
	}else if(strcmp(equal+1, "LLdVAR") == 0){
		newBit->type = LLdVAR;
		newBit->lenSz = 2;
	}else if(strcmp(equal+1, "LLLxVAR") == 0){
		newBit->type = LLLxVAR;
		newBit->lenSz = 3;
	}else if(strcmp(equal+1, "LLLdVAR") == 0){
		newBit->type = LLLdVAR;
		newBit->lenSz = 3;
	}else{
		newBit->type = UNKNOWN_TYPE;
		newBit->lenSz = 0;
	}

	/* Parsing Format line */
	equal = strchr(lineFormat, '=');
	if(equal == NULL){
		fprintf(stderr, "Nao localizado sinal de igual na linha de formato [%s] do bit [%s]!\n", lineFormat, bitId);
		return(ERRO);
	}

	if(strcmp(equal+1, "EBCDIC") == 0)      newBit->format = EBCDIC;
	else if(strcmp(equal+1, "ASCII") == 0)  newBit->format = ASCII;
	else if(strcmp(equal+1, "BINARY") == 0) newBit->format = BINARY;
	else if(strcmp(equal+1, "HEX") == 0)    newBit->format = HEX;
	else                                    newBit->format = UNKNOWN_FORMAT;

	return(OK);
}

int loadMsg(FILE *fMsg, bit_t **MsgHeadBitList)
{
	unsigned char lineBuffData[2000] = {0};
	unsigned char lineBuffType[70] = {0};
	unsigned char lineBuffFormat[70] = {0};
	char *c = NULL;

	*MsgHeadBitList = NULL;

	while(!feof(fMsg)){
		memset(lineBuffData, 0, sizeof(lineBuffData));
		memset(lineBuffType, 0, sizeof(lineBuffType));
		memset(lineBuffFormat, 0, sizeof(lineBuffFormat));

		/* Reading the data */
		fgets(lineBuffData, sizeof(lineBuffData), fMsg);
		if(lineBuffData[0] == '#' || lineBuffData[0] == '\n' || lineBuffData[0] == '\0')
			continue;

		c = strchr(lineBuffData, '\n');
		if(c != NULL)
			*c = '\0';

		/* Reading the type */
		fgets(lineBuffType, sizeof(lineBuffType), fMsg);
		if(lineBuffType[0] == '#' || lineBuffType[0] == '\n' || lineBuffType[0] == '\0')
			continue;

		c = strchr(lineBuffType, '\n');
		if(c != NULL)
			*c = '\0';

		/* Reading the format */
		fgets(lineBuffFormat, sizeof(lineBuffFormat), fMsg);
		if(lineBuffFormat[0] == '#' || lineBuffFormat[0] == '\n' || lineBuffFormat[0] == '\0')
			continue;

		c = strchr(lineBuffFormat, '\n');
		if(c != NULL)
			*c = '\0';

		if(insertBit(MsgHeadBitList, lineBuffData, lineBuffType, lineBuffFormat) == ERRO){
			fprintf(stderr, "Erro em inserir line: [%s][%s][%s].\n", lineBuffData, lineBuffType, lineBuffFormat);
			return(ERRO);
		}
	}

#if 1
	{
		bit_t *w = NULL;

		printf("Dump da lista:\n");
		for(w = *MsgHeadBitList; w != NULL; w = w->next){
			printf("[%d][%s][%zu]\n", w->bitId, w->data, w->szRaw);
		}
	}

	//exit(0);
#endif

	return(OK);
}

int formatBits(bit_t *MsgHeadBitList)
{
	bit_t *walkerMsgHeadBitList = NULL;
	bitFuncFmt_t *walkerFmtBit = NULL;

	for(walkerMsgHeadBitList = MsgHeadBitList; walkerMsgHeadBitList != NULL; walkerMsgHeadBitList = walkerMsgHeadBitList->next){
		fprintf(stderr, "-------------------------------------------\nprocurando fmt para: [%d].\n", walkerMsgHeadBitList->bitId);

		for(walkerFmtBit = fmtBit; walkerFmtBit->bitId != END; walkerFmtBit++){
			fprintf(stderr, "   comparando fmt para: [%d].\n", walkerFmtBit->bitId);
			if(walkerFmtBit->bitId == walkerMsgHeadBitList->bitId){
				if(walkerFmtBit->func == NULL)
					fprintf(stderr, "Nao existe funcao de formatacao para [%d] definida!\n", walkerFmtBit->bitId);
				else{
					walkerFmtBit->func(walkerMsgHeadBitList);
					break;
				}
			}
		}
	}

	return(OK);
}

int createMessageFile(char *msgFileFullPath)
{
	FILE *msgFile = NULL;

	msgFile = fopen(msgFileFullPath, "w");
	if(msgFile == NULL){
		fprintf(stderr, "Erro opening file [%s]: [%s].\n", msgFileFullPath, strerror(errno));
		return(ERRO);
	}

	fprintf(msgFile, "# ISO8583 Template Message File\n\n");

	fprintf(msgFile, "# \n");

	fprintf(msgFile, "# [[TIME_MMSS]]\n");
	fprintf(msgFile, "# [[TIME_DDYY]]\n");
	fprintf(msgFile, "# [[TIME_HHMMSS]]\n");
	fprintf(msgFile, "# [[RAND_6]]\n\n");

	fprintf(msgFile, "HEADER=\n\n");
	fprintf(msgFile, "MTI\n");
	fprintf(msgFile, "MTI=100\nTYPE=\nFORMAT=\n\n\n");
	fprintf(msgFile, "1=\nTYPE=\nFORMAT=\n\n");
	fprintf(msgFile, "2=\nTYPE=\nFORMAT=\n\n");
	fprintf(msgFile, "3=\nTYPE=\nFORMAT=\n\n");

	fclose(msgFile);
	return(OK);
}

int client(int argc, char *argv[])
{
#define BUFFER_OUTPUT_SZ	(10000)

	FILE *fMsg = NULL;
	bit_t *MsgHeadBitList = NULL; /* Inicio da lista */
	unsigned char trx[BUFFER_OUTPUT_SZ] = {0}; /* Buffer da mensagem ISO8583 formatado */
	int opt = 0;
	fdLib_t *pluginsList = NULL;
	size_t trxMaxSz = 0;
	size_t trxSzFormatted = 0;

	int optWait = 0; /* -1: no wait, 0: perpetual wait, n: seconds to wait */
	int optServerPort = 0;
	char optServerIpDns[200] = {0};
	char optPathMsgInFile[500] = {0};
	char optPathMsgOutFile[500] = {0};
	char optPathTemplateMsgInFile[500] = {0};
	char optPathPlugins[500] = {0};
	tictime_t startProg, endProg;

	while((opt = getopt(argc, argv, "w:i:o:s:p:c:b:")) != -1){
		switch(opt){
			case 'w':
				optWait = atoi(optarg);
				break;
			case 'i':
				strncpy(optPathMsgInFile, optarg, sizeof(optPathMsgInFile));
				break;
			case 'o':
				strncpy(optPathMsgOutFile, optarg, sizeof(optPathMsgOutFile));
				break;
			case 's':
				strncpy(optServerIpDns, optarg, sizeof(optServerIpDns));
				break;
			case 'p':
				optServerPort = atoi(optarg);
				break;
			case 'c':
				strncpy(optPathTemplateMsgInFile, optarg, sizeof(optPathTemplateMsgInFile));
				break;
			case 'b':
				strncpy(optPathPlugins, optarg, sizeof(optPathPlugins));
				break;
			default:
				usage(argv[0]);
				return(1);
				break;
		}
	}

	if(optPathTemplateMsgInFile[0] != '\0'){
		fprintf(stderr, "Msg template output file: [%s]\n", optPathTemplateMsgInFile);
		createMessageFile(optPathTemplateMsgInFile);
		return(0);
	}

	if(optPathMsgInFile[0] == '\0'){
		fprintf(stderr, "Msg input file undefined (-i option)!\n");
		usage(argv[0]);
		return(1);
	}

	if(optServerIpDns[0] == '\0'){
		fprintf(stderr, "OLTP server Ip or DNS undefined (-s option)!\n");
		usage(argv[0]);
		return(1);
	}

	if(optServerPort == 0){
		fprintf(stderr, "OLTP server port undefined (-p option)!\n");
		usage(argv[0]);
		return(1);
	}

	getTime(&startProg);

	fprintf(stderr, "Parametros:\n");
	fprintf(stderr, "Timeout wait......: [%d]\n", optWait);
	fprintf(stderr, "Msg input file....: [%s]\n", optPathMsgInFile);
	fprintf(stderr, "Msg output file...: [%s]\n", optPathMsgOutFile);
	fprintf(stderr, "OLTP Server IP/DNS: [%s]\n", optServerIpDns);
	fprintf(stderr, "OLTP Server Port..: [%d]\n", optServerPort);
	fprintf(stderr, "Plugins path......: [%s]\n", optPathPlugins);
	fprintf(stderr, "-------------------------------------------------------------\n");

	fMsg = fopen(optPathMsgInFile, "r");
	if(fMsg == NULL){
		fprintf(stderr, "Erro opening file [%s]: [%s].\n", optPathMsgInFile, strerror(errno));
		return(2);
	}

	if(loadMsg(fMsg, &MsgHeadBitList) == ERRO){
		fprintf(stderr, "Erro loading msg.\n");
		return(3);
	}

	if(optPathPlugins[0] != '\0'){
		pluginsList = NULL;
		if(loadPlugins(&pluginsList, optPathPlugins) == ERRO){
			fprintf(stderr, "Erro loading plugins.\n");
			return(4);
		}
	}

	if(formatBits(MsgHeadBitList) == ERRO){
		fprintf(stderr, "Erro formmating msg.\n");
		return(5);
	}

	trxMaxSz = BUFFER_OUTPUT_SZ;
	if(assemblyMsg(MsgHeadBitList, trx, trxMaxSz, &trxSzFormatted) == ERRO){
		fprintf(stderr, "Erro assembling msg.\n");
		return(5);
	}

	fprintf(stderr, "\n");
	{
		char dumpSend[100000] = {0};
		if(dump(trx, trxSzFormatted, dumpSend, 100000) == 0)
			fprintf(stderr, "Sending %zu:\n%.*s\n", trxSzFormatted, (int)trxSzFormatted, dumpSend);
	}
	
#if 0
	if(sendTrx(optServerIpDns, optServerPort, trx, trxSzFormatted) == ERRO){
		fprintf(stderr, "Erro sending trx.\n");
		return(6);
	}

	if(receiveTrx(trx) == ERRO){
		fprintf(stderr, "Erro receiving trx.\n");
		return(7);
	}
#endif

	fprintf(stderr, "\n");
	{
		char dumpRecv[100000] = {0};
		if(dump(trx, trxSzFormatted, dumpRecv, 100000) == 0)
			fprintf(stderr, "Received %zu:\n%.*s\n", trxSzFormatted, (int)trxSzFormatted, dumpRecv);
	}
	
	if(optPathPlugins[0] != '\0'){
		if(unloadPlugins(pluginsList) == ERRO){
			fprintf(stderr, "Erro unloading plugins.\n");
			return(8);
		}
	}

	getTime(&endProg);

	fprintf(stderr, "Tempo total de execucao: %f\n", diffTime(&startProg, &endProg));

	return(0);
}

int server(int argc, char *argv[])
{
	return(0);
}

int main(int argc, char *argv[])
{
#ifdef SERVER
	return server(argc, argv);
#elif CLIENT
	return client(argc, argv);
#else
	fprintf(stderr, "Not exec profile compiled!\n");
	return(-1);
#endif
}
