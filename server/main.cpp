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

struct Msg
{
	int read;
	string user, content;
	string getStr()
	{
		return user + ": " + content;
	}
};

struct File
{
	int read;
	string filename;
	vector <string> fragmentList;
};

class User
{
	private:
	
	string username, password;
	vector <string> friendList;
	vector <Msg> msgList;
	vector <File*> fileList;

	public:
	
	User(string name, string pass)
	{
		username = name;
		password = pass;
		friendList.push_back(name);
		msgList.clear();
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
		for (auto const &i : friendList) if (name == i) return 1;
		friendList.push_back(name);
		return 0;
	}

	string searchFriend()
	{
		string buf = "";
		for (auto const &i : friendList) buf += i + "\n";
		return buf;
	}

	int chat(string user)
	{
		for (auto const &i : friendList) if (i == user) return 0;
		return 1;
	}

	int sendmsg(string user, string str)
	{
		Msg msg;
		msg.user = user;
		msg.content = str;
		msg.read = 0;
		msgList.push_back(msg);
		return 0;
	}

	string recvmsg()
	{
		string str("undefined");
		for (auto &i : msgList) if (!i.read)
		{
			i.read = 1;
			str = i.getStr();
			break;
		}
		return str;
	}

	string recvmsgFrom(string user)
	{
		string str("undefined");
		for (auto &i : msgList) if (user == i.user && (!i.read))
		{
			i.read = 1;
			str = i.getStr();
			break;
		}
		return str;
	}

	int sendFile(string user, File *pf)
	{
		fileList.push_back(pf);
		return 0;
	}

	int checkFile()
	{
		for (auto const i : fileList) if (!i->read) return 0;
		return 1;
	}

	File *recvfile()
	{
		for (auto i : fileList) if (!i->read)
		{
			i->read = 1;
			return i;
		}
		return 0;
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
		for (auto &i : userList) if (user == i.getName()) return i.login(pass);
		return 2;
	}

	int regist(string user, string pass)
	{
		for (auto &i : userList) if (user == i.getName()) return 1;
		userList.push_back(User(user, pass));
		return 0;
	}

	int addFriend(string user1, string user2)
	{
		int flag = 1;
		for (auto &i : userList) if (user2 == i.getName()){flag = 0; break;}
		if (flag) return flag;
		for (auto &i : userList) if (user1 == i.getName()) return i.addFriend(user2);
		return 2;
	}

	string search()
	{
		string buf = "";
		for (auto &i : userList) buf += i.getName() + "\n";
		return buf;
	}

	string searchFriend(string user)
	{
		for (auto &i : userList) if (user == i.getName()) return i.searchFriend();
		return "Undefined user.";
	}

	string profile(string user)
	{
		return user;
	}

	int chat(string user1, string user2)
	{
		for (auto &i : userList) if (user1 == i.getName()) return i.chat(user2);
	}

	int sendmsg(string user1, string user2, string content)
	{
		for (auto &i : userList) if (user2 == i.getName()) return i.sendmsg(user1, content);
	}

	string recvmsg(string user)
	{
		for (auto &i : userList) if (user == i.getName()) return i.recvmsg();
	}

	string recvmsgFrom(string user1, string user2)
	{
		for (auto &i : userList) if (user1 == i.getName()) return i.recvmsgFrom(user2);
	}

	int sendFile(string user1, string user2, File *pf)
	{
		for (auto &i : userList) if (user2 == i.getName()) return i.sendFile(user1, pf);
	}

	int checkUser(string user)
	{
		for (auto &i : userList) if (user == i.getName()) return 0;
		return 1;
	}

	int checkFile(string user)
	{
		for (auto &i : userList) if (user == i.getName()) return i.checkFile();
		return 1;
	}

	File *recvfile(string user)
	{
		for (auto &i : userList) if (user == i.getName()) return i.recvfile();
		return 0;
	}

} manager;

void recv_data(int client_sockfd)
{
	char buf[BUFSIZ], args[8][128];
	memset(buf, 0, BUFSIZ);
	int len;
	while ((len = recv(client_sockfd, buf, BUFSIZ, 0)) > 0)
	{
		buf[len] = '\0';
		sscanf(buf, "%s%s%s%s", args[0], args[1], args[2], args[3], args[4]);
		printf("Recv: %s\n", buf);
		if (!strcmp(args[0], "login"))
		{
			manager.lock();
			int status = manager.login(args[1], args[2]);
			manager.unlock();
			sprintf(buf, "%d", status);
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "regist"))
		{
			manager.lock();
			int status = manager.regist(args[1], args[2]);
			manager.unlock();
			sprintf(buf, "%d", status);
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "add"))
		{
			manager.lock();
			printf("%s %s\n", args[1], args[2]);
			int status = manager.addFriend(args[1], args[2]);
			manager.unlock();
			sprintf(buf, "%d", status);
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "search"))
		{
			manager.lock();
			string str = manager.search();
			manager.unlock();
			sprintf(buf, "%s", str.c_str());
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "ls"))
		{
			manager.lock();
			string str = manager.searchFriend(args[1]);
			manager.unlock();
			sprintf(buf, "%s", str.c_str());
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "profile"))
		{
			manager.lock();
			string str = manager.profile(args[1]);
			manager.unlock();
			sprintf(buf, "%s", str.c_str());
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "chat"))
		{
			manager.lock();
			int status = manager.chat(args[1], args[2]);
			manager.unlock();
			sprintf(buf, "%d", status);
			printf("%s\n", buf);
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "sendmsg"))
		{
			manager.lock();
			manager.sendmsg(args[1], args[2], args[3]);
			manager.unlock();
			sprintf(buf, "Sendmsg succeed.");
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "recvmsg"))
		{
			manager.lock();
			string str = manager.recvmsg(args[1]);
			manager.unlock();
			sprintf(buf, "%s", str.c_str());
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "recvmsgfrom"))
		{
			manager.lock();
			string str = manager.recvmsgFrom(args[1], args[2]);
			manager.unlock();
			sprintf(buf, "%s", str.c_str());
			send(client_sockfd, buf, strlen(buf), 0);
			continue;
		}
		if (!strcmp(args[0], "sendfile"))
		{
			char s[4][128];
			int n;
			sscanf(buf, "%s%s%s%s%d", s[0], s[1], s[2], s[3], &n);
			File *pf = new File();
			pf->filename = string(args[3]);
			pf->read = 0;
			pf->fragmentList.clear();
			sprintf(buf, "ready");
			if (manager.checkUser(args[2]))
			{
				sprintf(buf, "nouser");
				send(client_sockfd, buf, strlen(buf), 0);
				continue;
			}
			send(client_sockfd, buf, strlen(buf), 0);
			for (int i = 0; i < n; i++)
			{
				len = recv(client_sockfd, buf, BUFSIZ, 0);
				buf[len] = '\0';
				pf->fragmentList.push_back(buf);
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "ok");
				send(client_sockfd, buf, strlen(buf), 0);
			}
			manager.sendFile(args[1], args[2], pf);
			continue;
		}
		if (!strcmp(args[0], "recvfile"))
		{
			File *pf;
			int status = 1;
			status = manager.checkFile(args[1]);
			if (!status) pf = manager.recvfile(args[1]);
			if (status)	sprintf(buf, "%d", status);
			else sprintf(buf, "%d %s %d", status, pf->filename.c_str(), pf->fragmentList.size());
			printf("%s\n", buf);
			send(client_sockfd, buf, strlen(buf), 0);
			if (status) continue;
			for (auto &i : pf->fragmentList)
			{
				len = recv(client_sockfd, buf, BUFSIZ, 0);
				buf[len] = '\0';
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s", i.c_str());
				send(client_sockfd, buf, strlen(buf), 0);
			}
		}
		sprintf(buf, "Undefined.");
		send(client_sockfd, buf, strlen(buf), 0);
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

