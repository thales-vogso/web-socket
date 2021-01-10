/****************************************************************************
*	@Copyright(c)	2015,Vogso
*	@desc	服务端
*	@date	2015-10-9
*	@author	minamoto
*	@E-mail	jiangtai@wushuang.me
*	@file	Epoll.h
******************************************************************************/
#ifndef EPOLL_H
#define EPOLL_H

#include <vector>
#include <map>
#include <sys/epoll.h>
#include <semaphore.h>
#include "Server.h"
#include "Client.h"

#include "json/json.h"

class Epoll
{
	public:
		/**
		 *	构造函数
		 *	@param	port	端口
		 */
		Epoll(int port);
		Epoll();
		/**
		 *	析构函数
		 */
		virtual ~Epoll();
		/**
		 *	运行
		 */
		void run();
		/**
		 *      epoll客户端最大数目
		 */
		static const int EPOLL_SIZE = 10000;
		/**
		 *      epoll的超时时间
		 */
		static const int EPOLL_RUN_TIMEOUT = -1;
	private:
		/**
		 *	服务端
		 */
		Server* _server;
		/**
		 *	客户端列表
		 */
		static std::map<int, Client*> _clients;
		/**
		 *	epoll描述符
		 */
		int _epfd;
		/**
		 *	地址
		 */
		struct sockaddr_in _address;
		/**
		 *	事件
		 */
		struct epoll_event _evt;
		/**
		 *	事件集合
		 */
		struct epoll_event _events[EPOLL_SIZE];
		/**
		 *	创建
		 */
		bool create();
		/**
		 *	等待
		 */
		bool wait();
		/**
		 *	事件
		 */
		bool handle(int id);
		/**
		 *	检验消息
		 *	@param	client	消息源
		 *	@param	msg	接收的消息
		 */
		void checkMessage(Client* client, std::string msg);
		/**
		 *	添加客户端
		 *	@param	clientSocket	客户端
		 */
		void addClient(Client* client);
		/**
		 *	删除客户端
		 *	@param	clientSocket	客户端
		 */
		void delClient(Client* client);
		/**
		 *	给所有人发消息
		 *	@param	msg	  发送的消息
		 */
		void sendToAll(std::string msg);
		/**
		 *	获取链接列表
		 *	@return	客户端列表
		 */
		Json::Value getJsonClientList();
		/**
		 *	通过id获取客户端
		 *	@param	id	客户端id
		 *	@return	客户端
		 */
		Client* getClientByID(int id);
		/**
		 *	服务器状态
		 */
		static bool serviceFlag;
};

#endif
