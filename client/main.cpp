#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

struct user
{
	string username;
	string chatwith;
	int state;
} user;

int sendLogin(string username, string password, char *buf)
{
	sprintf(buf, "login %s %s", username.c_str(), password.c_str());
	user.username = username;
	user.state = 1;
}

int sendRegist(string username, string password, char *buf)
{
	sprintf(buf, "regist %s %s", username.c_str(), password.c_str());
}

int sendSearch(char *buf)
{
	sprintf(buf, "search %s", user.username.c_str());
}

int sendAdd(string username, char *buf)
{
	sprintf(buf, "add %s %s", user.username.c_str(), username.c_str());
}

int sendLs(char *buf)
{
	sprintf(buf, "ls %s", user.username.c_str());
}

int sendChat(string username, char *buf)
{
	sprintf(buf, "chat %s %s", user.username.c_str(), username.c_str());
	user.state = 2;
	user.chatwith = username;
}

int sendSendmsg(string content, char *buf)
{
	sprintf(buf, "sendmsg %s %s %s", user.username.c_str(), user.chatwith.c_str(), content.c_str());
}

int sendSendfile(string content, char *buf)
{
	sprintf(buf, "sendfile %s %s %s", user.username.c_str(), user.chatwith.c_str(), content.c_str());
}

int sendRecvmsg(char *buf)
{
	sprintf(buf, "recvmsg %s", user.username.c_str());
}

int sendRecvfile(char *buf)
{
	sprintf(buf, "recvfile %s", user.username.c_str());
}

int sendProfile(char *buf)
{
	sprintf(buf, "profile %s", user.username.c_str());
}

int sendSync(char *buf)
{
	sprintf(buf, "sync %s", user.username.c_str());
}

int main()
{
	char host_addr_str[128];
	printf("Enter host IP Address:\n");
	scanf("%s", host_addr_str);

	int client_sockfd;
	int len;
	struct sockaddr_in remote_addr;
	char buf[BUFSIZ];
	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(host_addr_str);
	remote_addr.sin_port=htons(8000);

	if ((client_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		return 1;
	}

	if (connect(client_sockfd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("connect error");
		return 1;
	}
	printf("connected to server\n");
	len = recv(client_sockfd, buf, BUFSIZ, 0);
	buf[len] = '\0';
	printf("%s\n", buf);
	printf("Enter 'help' for more.\n");

	user.username = "";
	char s[128];
	string u, p;
	u.resize(128);
	p.resize(128);
	while(1)
	{
		scanf("%s", s);
		if (!strcmp(s, "login"))
		{
			if (user.state == 2)
			{
				printf("Please exit chat.\n");
				continue;
			}
			printf("Enter username:\n");
			scanf("%s", &u[0]);
			printf("Enter password:\n");
			scanf("%s", &p[0]);
			sendLogin(u, p, buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "regist"))
		{
			if (!user.state == 2)
			{
				printf("Please exit chat.\n");
				continue;
			}
			printf("Enter username:\n");
			scanf("%s", &u[0]);
			printf("Enter password:\n");
			scanf("%s", &p[0]);
			sendRegist(u, p, buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "search"))
		{
			if (user.state != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			sendSearch(buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "add"))
		{
			if (user.state != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			scanf("%s", &u[0]);
			sendAdd(u, buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "ls"))
		{
			if (user.state != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			sendLs(buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "chat"))
		{
			if (user.state != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			scanf("%s", &u[0]);
			sendChat(u, buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "sendmsg"))
		{
			if (user.state != 2)
			{
				printf("Please enter chat first.\n");
				continue;
			}
			scanf("%s", &u[0]);
			sendSendmsg(u, buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "sendfile"))
		{
			if (user.state != 2)
			{
				printf("Please enter chat first.\n");
				continue;
			}
			scanf("%s", &u[0]);
			sendSendfile(u, buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "recvmsg"))
		{
			if (user.state != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			sendRecvmsg(buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "recvfile"))
		{
			if (user.state != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			sendRecvfile(buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "profile"))
		{
			if (user.state != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			sendProfile(buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "sync"))
		{
			if (user.state != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			sendSync(buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "exit"))
		{
			if (user.state == 2)
			{
				printf("Exit chat.\n");
				user.state = 1;
				continue;
			}
			else break;
		}
		printf("???\n");
	}
	close(client_sockfd);
	return 0;
}
