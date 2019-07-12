/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

/* Sample TCP server */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#define SZ_BUF (10000)

int main(int argc, char**argv)
{
	int listenfd, connfd, n, i;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t clilen;
	pid_t     childpid;
	char mesg[SZ_BUF] = {0};

	if(argc != 2)
	{
		printf("Usage:\n%s PORT\n", argv[0]);
		return(0);
	}

	listenfd=socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(atoi(argv[1]));
	bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	listen(listenfd,1024);

	for(;;)
	{
		clilen=sizeof(cliaddr);
		connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);

		if ((childpid = fork()) == 0)
		{
			close (listenfd);
			for(;;)
			{
				memset(mesg, 0, SZ_BUF);
				n = recvfrom(connfd, mesg, SZ_BUF, 0,(struct sockaddr *)&cliaddr, &clilen);
				if(n == -1){
					return(0);
				}
				/* sendto(connfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));*/
				printf("-------------------------------------------------------\n");
				mesg[n] = 0;
				printf("Received (sz %d):\n", n);
				/* printf("%s",mesg); */
				for(i = 0; (i < n) && (i < SZ_BUF); i++)
				{
					printf("%02X ", mesg[i]);
				}
				printf("\n");
			}

		}
		close(connfd);
	}
	return(0);
}
