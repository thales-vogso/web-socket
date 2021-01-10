/****************************************************************************
*	@Copyright(c)	2015,Vogso
*	@desc	服务端
*	@date	2015-10-9
*	@author	minamoto
*	@E-mail	jiangtai@wushuang.me
*	@file	Epoll.cpp
******************************************************************************/
#include "Epoll.h"
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include "Message.h"

using namespace std;

bool Epoll::serviceFlag = true;
std::map<int, Client*> Epoll::_clients;

Epoll::Epoll(int port)
{
	_server = new Server();
	_server->launch(port);
	create();
}
Epoll::Epoll()
{
	_server = new Server();
	_server->launch();
	create();
}

Epoll::~Epoll()
{

}
void Epoll::run()
{
	wait();
}



bool Epoll::create(){
	int fd = _server->getFileDescripto();
	_evt.events = EPOLLIN | EPOLLET; //对读感兴趣，边沿触发
	_epfd = epoll_create(EPOLL_SIZE); //创建一个epoll描述符，并将监听socket加入epoll
	_evt.data.fd = fd;
	int res = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &_evt);
	return res != -1;
}
bool Epoll::wait(){
	while(serviceFlag)
	{
		int num = epoll_wait(_epfd, _events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);
		for(int i = 0; i < num ; i++)
		{
			if(_events[i].data.fd == _server->getFileDescripto())                    //新的连接到来，将连接添加到epoll中，并发送欢迎消息
			{
				Client* client = new Client();
				client->connect(_server->getFileDescripto());
				addClient(client);
				_evt.data.fd = client->getFileDescripto();
				epoll_ctl(_epfd, EPOLL_CTL_ADD, client->getFileDescripto(), &_evt);
				std::cout << "Welcome to seChat! You ID is: Client " << client->getID() << std::endl;
			}
			else  // 有消息需要读取
			{
				handle(_events[i].data.fd); //注意：这里并没有调用epoll_ctl重新设置socket的事件类型，但还是可以继续收到客户端发送过来的信息
			}
		}
		
	}
	_server->shut();
	::close(_epfd);
	return true;
}
bool Epoll::handle(int id){
	Client* client = _clients[id];
	if(client->setHeader()) {
		std::stringstream msg;
		msg << "Welcome to seChat! You ID is: Client " << client->getID() << "\r\n";
		//client->send(msg.str());
		return true;
	}
	std::string msg = client->receive();
	//std::cout << "recv is \r\n" << msg << std::endl;
	if(msg == "-1" || msg == "0"){
		delClient(client);
	}else{
		checkMessage(client, msg);
	}
	return true;
}
void Epoll::checkMessage(Client* client, std::string msg){
	Json::Value obj,oops,data,user,target;
	Json::Reader reader;
	Client* take;
	if(!reader.parse(msg, obj)){
		std::cout << "json error" << std::endl;
		return;
	}
	user["id"] = client->getID();
	user["name"] = client->getName();
	std::cout << "code is " << obj["code"].asInt() << std::endl;
	switch (obj["code"].asInt()){
		case Message::cEnter:
			oops["code"] = Message::sEnter;
			client->setName(obj["data"].asString());
			user["name"] = client->getName();
			data["user"] = user;
			data["list"] = getJsonClientList();
			oops["data"] = data;
			sendToAll(oops.toStyledString());
			break;
		case Message::cRename:
			oops["code"] = Message::sRename;
			data["user"] = user;
			client->setName(obj["data"].asString());
			target["id"] = client->getID();
			target["name"] = client->getName();
			data["target"] = target;
			oops["data"] = data;
			sendToAll(oops.toStyledString());
			break;
		case Message::cChat:
			oops["code"] = Message::sChat;
			data["user"] = user;
			data["content"] = obj["data"]["content"].asString();
			oops["data"] = data;
			sendToAll(oops.toStyledString());
			break;
		case Message::cWhisper:
			take = getClientByID(atoi(obj["data"]["id"].asString().c_str()));
			if(!take) return;
			oops["code"] = Message::sWhisper;
			data["user"] = user;
			target["id"] = take->getID();
			target["name"] = take->getName();
			data["target"] = target;
			data["content"] = obj["data"]["content"].asString();
			oops["data"] = data;
			take->send(oops.toStyledString());
			client->send(oops.toStyledString());
			break;
		default:
			break;
			
	};
}


void Epoll::addClient(Client* client)
{
	_clients[client->getFileDescripto()] = client;
	
	std::cout<<"Now "<<_clients.size()<<" users..";
	std::cout<<"New User: "<<client->getID()<<" ip :"<<client->getAddress()<<" "<<client->getPort()<<std::endl;
}
void Epoll::delClient(Client* client)
{
	Json::Value oops,data,user;
	user["id"] = client->getID();
	user["name"] = client->getName();
	_clients.erase(_clients.find(client->getFileDescripto()));
	
	oops["code"] = Message::sLeave;
	data["user"] = user;
	data["list"] = getJsonClientList();
	oops["data"] = data;
	sendToAll(oops.toStyledString());
}
void Epoll::sendToAll(std::string msg){
	for(std::map<int, Client*>::iterator it = _clients.begin();it != _clients.end(); it++){
		Client* client = it->second;
		client->send(msg);
	}
}
Client* Epoll::getClientByID(int id){
	for(std::map<int, Client*>::iterator it = _clients.begin();it != _clients.end(); it++){
		Client* client = it->second;
		if(id == client->getID()){
			return client;
		}
	}
	return NULL;
}
Json::Value Epoll::getJsonClientList(){
	Json::Value list;
	for(std::map<int, Client*>::iterator it = _clients.begin();it != _clients.end(); it++){
		Client* client = it->second;
		Json::Value single;
		single["id"] = client->getID();
		single["name"] = client->getName();
		list.append(single);
	}
	return list;
}
