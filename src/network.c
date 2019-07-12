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
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <ISO8583.h>
/* #include <Tools.h> */

int sockfd;

int sendTrx(char *serverAddr, int port, unsigned char *trx, size_t szTrx)
{
	/* unsigned char *trxDump; */
	struct sockaddr_in servaddr;
	struct hostent *servResolved;
	struct in_addr ip_addr;

	signal(SIGPIPE, SIG_IGN);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "ERRO: socket() [%s]\n", strerror(errno));
		return(ERRO);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(port);

	servResolved = gethostbyname(serverAddr);
	if(servResolved == NULL){
		fprintf(stderr, "ERRO: gethostbyname() ");
		switch(h_errno){
			case HOST_NOT_FOUND:
				fprintf(stderr, "[The specified host is unknown]\n");
				break;

			case NO_ADDRESS:
				/*case NO_DATA:*/
				fprintf(stderr, "[The requested name is valid but does not have an IP address]\n");
				break;

			case NO_RECOVERY:
				fprintf(stderr, "[A non-recoverable name server error occurred]\n");
				break;

			case TRY_AGAIN:
				fprintf(stderr, "[A temporary error occurred on an authoritative name server. Try again later]\n");
				break;

			default:
				fprintf(stderr, "[error unknow]\n");
				break;
		}

		return(ERRO);
	}

	ip_addr = *(struct in_addr *)(servResolved->h_addr);

	if(inet_pton(AF_INET, inet_ntoa(ip_addr), &servaddr.sin_addr) < 0){
		fprintf(stderr, "ERRO: inet_pton() [%s]\n", strerror(errno));
		return(ERRO);
	}

	if(connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
		fprintf(stderr, "ERRO: connect() [%s]\n", strerror(errno));
		return(ERRO);
	}

	/*
	trxDump = (unsigned char *)malloc(szTrx * 5);
	dump(trx, szTrx, trxDump, (szTrx * 5));
	*/
	fprintf(stderr, "Enviando:\n%s\n", /*trxDump*/ "NaN");
	/*free(trxDump);*/
	
	/* HEADER */
	/* hton pra transformar o int!! */
	uint32_t len = 13;
	write(sockfd, &len, sizeof(len));


	strcpy(trx, "abcdefghijlmnopqrstuvxz1234567890");
	szTrx = strlen(trx);

	/* TRX */
	write(sockfd, trx, szTrx);

	/* close(sock); */

	return(OK);
}

int receiveTrx(unsigned char *Msg)
{


/*
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	char* pBuf = buffer;
	int bytesLeft = sizeof(buffer) - sizeof(char);

	while(bytesLeft > 0)
	{
		int n = read(newsockfd, pBuf, bytesLeft);
		if (n < 0)
		{
			error("ERROR reading from socket");
			break;
		}
		if(n == 0)
		{
			error("peer shutted down");
			break;
		}
		pBuf += n;
		bytesLeft -= n;
	}
*/

	return(OK);
}
