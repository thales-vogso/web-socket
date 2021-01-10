/****************************************************************************
*	@Copyright(c)	2015,Vogso
*	@desc	消息
*	@date	2015-10-26
*	@author	minamoto
*	@E-mail	jiangtai@wushuang.me
*	@file	Message.h
******************************************************************************/
#ifndef MESSAGE_H
#define MESSAGE_H


class Message
{
	public:
		/**
		 *	构造函数
		 */
		Message();
		/**
		 *	析构函数
		 */
		virtual ~Message();
		/**
		 *	消息码
		 *	c客户端
		 *	s服务端
		 */
		enum MessageCodeTag {
			cEnter = 110101,//进入
			sEnter = 120101,//进入
				
			sLeave = 120102,//离开
				
			cRename = 110201,//改名
			sRename = 120201,//改名
			
			cChat = 110301,//聊天
			sChat = 120301,//聊天
			cWhisper = 110302,//悄悄话
			sWhisper = 120302//悄悄话
		} MessageCode;
};

#endif
