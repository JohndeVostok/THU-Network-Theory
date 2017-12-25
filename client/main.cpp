#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

int client_sockfd;

void recvMsgAuto();

class Manager
{
	private:
	
	string name, chatUser, buffer;
	mutex mut;
	int threadExitFlag;

	int status;
	
	public:
	
	Manager()
	{
		threadExitFlag = 0;
	}

	string getName()
	{
		return name;
	}

	int setBuff(string str)
	{
		buffer = str;
	}

	int login()
	{
		name = buffer;
		status = 1;
	}

	int logout()
	{
		name = "";
		status = 0;
	}

	int getStatus()
	{
		return status;
	}

	int startChat(string user)
	{
		chatUser = user;
		status = 2;
		mut.lock();
		threadExitFlag = 1;
		mut.unlock();
		thread(recvMsgAuto).detach();
	}

	string getChat()
	{
		return chatUser;
	}

	int endChat()
	{
		chatUser = "";
		status = 1;
		mut.lock();
		threadExitFlag = 0;
		mut.unlock();
	}

	int lock()
	{
		mut.lock();
	}

	int unlock()
	{
		mut.unlock();
	}

	int getFlag()
	{
		return threadExitFlag;
	}

} manager;

class Network
{
	public:

	int login(string username, string password)
	{
		char buf[BUFSIZ];
		sprintf(buf, "login %s %s", username.c_str(), password.c_str());
		manager.setBuff(username);
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		int status;
		sscanf(buf, "%d", &status);
		if (status == 0)
		{
			manager.login();
			printf("Login succeed. Your ID: %s\n", manager.getName().c_str());
			return 0;
		}
		if (status == 1)
		{
			printf("Password wrong.\n");
			return 1;
		}
		if (status == 2)
		{
			printf("No such user.\n");
			return 1;
		}
	}

	int regist(string username, string password)
	{
		char buf[BUFSIZ];
		sprintf(buf, "regist %s %s", username.c_str(), password.c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		int status;
		sscanf(buf, "%d", &status);
		if (status == 0)
		{
			printf("Register succeed.\n");
			return 0;
		}
		if (status == 1)
		{
			printf("Username existed.\n");
			return 1;
		}
	}

	int search()
	{
		char buf[BUFSIZ];
		sprintf(buf, "search");
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		printf("User list:\n");
		printf("%s\n", str.c_str());
	}

	int addFriend(string username)
	{
		char buf[BUFSIZ];
		sprintf(buf, "add %s %s", manager.getName().c_str(), username.c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		int status;
		sscanf(buf, "%d", &status);
		if (status == 0)
		{
			printf("Add friend succeed.\n");
			return 0;
		}
		if (status == 1)
		{
			printf("No such user.\n");
			return 1;
		}
	}

	int searchFriend()
	{
		char buf[BUFSIZ];
		sprintf(buf, "ls %s", manager.getName().c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		printf("Friend list:\n");
		printf("%s\n", str.c_str());
	}

	int chat(string username)
	{
		char buf[BUFSIZ];
		sprintf(buf, "chat %s %s", manager.getName().c_str(), username.c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		int status;
		sscanf(buf, "%d", &status);
		if (status == 1)
		{
			printf("Not your friend.\n");
			return 1;
		}
		printf("Chat succeed.\n");
		manager.startChat(username);
		return 0;
	}

	int sendmsg(string content)
	{
		char buf[BUFSIZ];
		sprintf(buf, "sendmsg %s %s %s", manager.getName().c_str(), manager.getChat().c_str(), content.c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		printf("%s\n", str.c_str());
	}

	int sendfile(string content)
	{
		char buf[BUFSIZ];
		sprintf(buf, "sendfile %s %s %s", manager.getName().c_str(), manager.getChat().c_str(), content.c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		printf("%s\n", str.c_str());
	}

	int recvmsg()
	{
		char buf[BUFSIZ];
		sprintf(buf, "recvmsg %s", manager.getName().c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		printf("%s\n", str.c_str());
	}

	int recvmsgFrom()
	{
		char buf[BUFSIZ];
		sprintf(buf, "recvmsgfrom %s %s", manager.getName().c_str(), manager.getChat().c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		if (str != "undefined") printf("%s\n", str.c_str());
	}

	int recvfile()
	{
		char buf[BUFSIZ];
		sprintf(buf, "recvfile %s", manager.getName().c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		printf("%s\n", str.c_str());
	}

	int recvfileFrom()
	{
		char buf[BUFSIZ];
		sprintf(buf, "recvfilefrom %s %s", manager.getName().c_str(), manager.getChat().c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		printf("%s\n", str.c_str());
	}

	int profile()
	{
		char buf[BUFSIZ];
		sprintf(buf, "profile %s", manager.getName().c_str());
		send(client_sockfd, buf, strlen(buf), 0);
		int len = recv(client_sockfd, buf, BUFSIZ, 0);
		buf[len] = '\0';
		string str(buf);
		printf("Your profile: %s\n", str.c_str());
	}
} network;


void recvMsgAuto()
{
	int flag = 1;
	while (flag)
	{
		network.recvmsgFrom();
		manager.lock();
		flag = manager.getFlag();
		manager.unlock();
		sleep(1);
	}
}

int main()
{
	char host_addr_str[128];
	printf("Enter host IP Address:\n");
	scanf("%s", host_addr_str);

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

	manager.getName() = "";
	char s[128];
	string u, p;
	u.resize(128);
	p.resize(128);
	while(1)
	{
		scanf("%s", s);
		if (!strcmp(s, "login"))
		{
			if (manager.getStatus() != 0)
			{
				printf("Please logout.\n");
				continue;
			}
			printf("Enter username:\n");
			scanf("%s", &u[0]);
			printf("Enter password:\n");
			scanf("%s", &p[0]);
			network.login(u, p);
			continue;
		}
		if (!strcmp(s, "regist"))
		{
			if (manager.getStatus() == 2)
			{
				printf("Please exit chat.\n");
				continue;
			}
			printf("Enter username:\n");
			scanf("%s", &u[0]);
			printf("Enter password:\n");
			scanf("%s", &p[0]);
			network.regist(u, p);
			continue;
		}
		if (!strcmp(s, "search"))
		{
			if (manager.getStatus() != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			network.search();
			continue;
		}
		if (!strcmp(s, "add"))
		{
			if (manager.getStatus() != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			scanf("%s", &u[0]);
			network.addFriend(u);
			continue;
		}
		if (!strcmp(s, "ls"))
		{
			if (manager.getStatus() != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			network.searchFriend();
			continue;
		}
		if (!strcmp(s, "chat"))
		{
			if (manager.getStatus() != 1)
			{
				printf("Please login or exit chat.\n");
				continue;
			}
			scanf("%s", &u[0]);
			network.chat(u);
			continue;
		}
		if (!strcmp(s, "sendmsg"))
		{
			if (manager.getStatus() != 2)
			{
				printf("Please enter chat first.\n");
				continue;
			}
			scanf("%s", &u[0]);
			network.sendmsg(u);
			continue;
		}
		if (!strcmp(s, "sendfile"))
		{
			if (manager.getStatus() != 2)
			{
				printf("Please enter chat first.\n");
				continue;
			}
			scanf("%s", &u[0]);
			network.sendfile(u);
			continue;
		}
		if (!strcmp(s, "recvmsg"))
		{
			if (manager.getStatus() == 1) network.recvmsg();
			else printf("Please login or exit chat.");
			continue;
		}
		if (!strcmp(s, "recvfile"))
		{
			if (manager.getStatus() == 0) printf("Please login first.\n");
			if (manager.getStatus() == 1) network.recvfile();
			if (manager.getStatus() == 2) network.recvfileFrom();
			continue;
		}
		if (!strcmp(s, "profile"))
		{
			if (manager.getStatus() != 1) printf("Please login or exit chat.\n");
			else network.profile();
			continue;
		}
		if (!strcmp(s, "exit"))
		{
			if (manager.getStatus() == 1)
			{
				manager.logout();
				printf("Logout.\n");
				continue;
			}
			if (manager.getStatus() == 2)
			{
				manager.endChat();
				printf("Exit chat.\n");
				continue;
			}
			else break;
		}
		printf("Invalid operation.\n");
	}
	close(client_sockfd);
	return 0;
}
