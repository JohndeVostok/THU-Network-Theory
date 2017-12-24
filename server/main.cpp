#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <thread>
#include <string>
#include <mutex>

using namespace std;

class User
{
	private:
	
	string username, password;
	vector <string> friendList;

	public:
	
	User(string name, string pass)
	{
		username = name;
		password = pass;
	}

	string getName()
	{
		return username;
	}

	int login(string pass)
	{
		return (pass != password);
	}

	int addFriend(string name)
	{
		for (auto i : friendList) if (i == name) return 1;
		friendList.push_back(name);
		return 0;
	}

	string searchFriend()
	{
		string buf = "HEAD\n";
		for (auto i : friendList) buf += i + "\n";
		return buf;
	}
};

class Manager
{
	private:
	
	vector<User> userList;
	
	public:
	
	mutex mut;
	
	Manager() {}
	
	void lock()
	{
		mut.lock();
	}

	void unlock()
	{
		mut.unlock();
	}

	int login(string user, string pass)
	{
		for (auto i : userList) if (user == i.getName()) return i.login(pass);
		return 2;
	}

	int regist(string user, string pass)
	{
		for (auto i : userList) if (user == i.getName()) return 1;
		userList.push_back(User(user, pass));
		return 0;
	}

	int addFriend(string user1, string user2)
	{
		int flag = 1;
		for (auto i : userList) if (user2 == i.getName()){flag = 0; break;}
		if (flag) return flag;
		for (auto i : userList) if (user1 == i.getName()) return i.addFriend(user2);
		return 2;
	}

	string search()
	{
		string buf = "HEAD\n";
		for (auto i : userList) buf += i.getName() + "\n";
		return buf;
	}

	string searchFriend(string user)
	{
		for (auto i : userList) if (user == i.getName()) return i.searchFriend();
		return "Undefined user.";
	}

	string profile(string user)
	{
		return user;
	}
} manager;

void recv_data(int client_sockfd)
{
	char buf[BUFSIZ], args[4][BUFSIZ];
	memset(buf, 0, BUFSIZ);
	int len;
	while ((len = recv(client_sockfd, buf, BUFSIZ, 0)) > 0)
	{
		buf[len] = '\0';
		sscanf(buf, "%s%s%s%s", args[0], args[1], args[2], args[3]);
		printf("%s\n", args[0]);
		if (!strcmp(args[0], "login"))
		{
			manager.lock();
			int status = manager.login(args[1], args[2]);
			manager.unlock();
			sprintf(buf, "%d", status);
			send(client_sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(args[0], "regist"))
		{
			manager.lock();
			int status = manager.regist(args[1], args[2]);
			manager.unlock();
			printf("%d\n", status);
			sprintf(buf, "%d", status);
			send(client_sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(args[0], "add"))
		{
			manager.lock();
			int status = manager.addFriend(args[1], args[2]);
			manager.unlock();
			sprintf(buf, "%d", status);
			send(client_sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(args[0], "search"))
		{
			manager.lock();
			string str = manager.search();
			manager.unlock();
			sprintf(buf, "%s", str.c_str());
			send(client_sockfd, buf, strlen(buf), 0);
		}
		if (!strcmp(args[0], "ls"))
		{
			manager.lock();
			string str = manager.searchFriend(args[1]);
			manager.unlock();
			sprintf(buf, "%s", str.c_str());
			send(client_sockfd, buf, strlen(buf), 0);
		}
	}
	close(client_sockfd);
}

int main(int argc, char *argv[])
{
	int server_sockfd;
	int len;
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	socklen_t sin_size;
	char buf[BUFSIZ];
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr=INADDR_ANY;
	my_addr.sin_port = htons(8000);

	if ((server_sockfd=socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		return 1;
	}

	if (bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("bind error");
		return 1;
	}

	if (listen(server_sockfd, 64) < 0)
	{
		perror("listen error");
		return 1;
	}

	sin_size = sizeof(struct sockaddr_in);
	while(1)
	{
		int client_sockfd;
		if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&remote_addr, &sin_size)) < 0)
		{
			perror("accept error");
			return 1;
		}
		printf("Accept client %s\n", inet_ntoa(remote_addr.sin_addr));
		len = send(client_sockfd, "Welcome to MZX Chat", 19, 0);
		thread(recv_data, client_sockfd).detach();
	}
	close(server_sockfd);
	return 0;
}

