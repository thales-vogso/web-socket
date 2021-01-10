/****************************************************************************
*	@Copyright(c)	2015,Vogso
*	@desc	客户端
*	@date	2015-10-9
*	@author	minamoto
*	@E-mail	jiangtai@wushuang.me
*	@file	Client.h
******************************************************************************/
#ifndef CLIENT_H
#define CLIENT_H

#include "Socket.h"

class Client : public Socket
{
	public:
		/**
		 *	构造函数
		 */
		Client();
		/**
		 *	析构函数
		 */
		virtual ~Client();
		/**
		 *	链接
		 */
		bool connect(int fd);
		/**
		 *	断开链接
		 */
		bool disconnect();
		
		/**
		 *	发送消息
		 *	@param	msg	消息
		 *	@return	结果
		 */
		bool send(std::string msg);
		/**
		 *	接收消息
		 *	@return	消息
		 */
		std::string receive();
		/**
		 *      设置websocket头
		 *	@return	是否新连接
		 */
		bool setHeader();
		/**
		 *	获取唯一标识
		 */
		int getID();
		/**
		 *	设置姓名
		 *	@param	name
		 */
		void setName(std::string name);
		/**
		 *	获取姓名
		 *	@return	姓名
		 */
		std::string getName();
	private:
		/**
		 *	唯一标示
		 */
		int _id;
		/**
		 *	名字
		 */
		std::string _name;
		/**
		 *	新连接
		 */
		bool _newFD;
		/**
		 *	分配id
		 */
		static int increaseID;
		/**
		 *	最高接收字符
		 */
		static const int MAXLEN = 1024;
		/**
		 *	验证码
		 */
		static const int SHA1_LEN = 20;
		/**
		 *	获取key
		 */
		std::string fetchSecKey(const char* buf);
		/**
		 *	websocket头
		 */
		void websocketHandshake();
		/**
		 *	信息解包
		 *	@param	buf	接收的打包数据
		 *	@return	实际信息
		 */
		std::string incomePocket(char* buf);
		/**
		 *	信息打包
		 *	@param	msg	消息
		 *	@return	包
		 */
		std::string outgoingPokect(std::string msg);
};

#endif
