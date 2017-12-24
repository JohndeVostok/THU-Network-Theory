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
	string username, password;
} user;

int login(string username, string password, char *buf)
{
	sprintf(buf, "login %s %s", username.c_str(), password.c_str());
}

int regist(string username, string password, char *buf)
{
	sprintf(buf, "regist %s %s", username.c_str(), password.c_str());
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
	printf("%s", buf);
	printf("Enter 'help' for more.");

	user.clear();
	char s[128];
	string u, p;
	u.resize(128);
	p.resize(128);
	while(1)
	{
		scanf("%s", s);
		if (!strcmp(s, "login"))
		{
			if (user.state() == 2)
			{
				printf("Please exit chat.\n");
				continue;
			}
			printf("Enter username:\n");
			scanf("%s", &u[0]);
			printf("Enter password:\n");
			scanf("%s", &p[0]);
			login(u, p, buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "regist"))
		{
			if (!user.state() == 2)
			{
				printf("Please exit chat.\n");
				continue;
			}
			printf("Enter username:\n");
			scanf("%s", &u[0]);
			printf("Enter password:\n");
			scanf("%s", &p[0]);
			regist(u, p, buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "search"))
		{
			if (user.state() == 1)
			{
				printf("Please login first.\n");
				continue;
			}
			search(buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "ls") == 1)
		{
			if (!user.state())
			{
				printf("Please login first.\n");
				continue;
			}
			ls(buf);
			len = send(client_sockfd, buf, strlen(buf), 0);
			len = recv(client_sockfd, buf, strlen(buf), 0);
			buf[len] = '\0';
			printf("%s\n", buf);
			continue;
		}
		if (!strcmp(s, "exit")) break;
	}
	close(client_sockfd);
	return 0;
}
