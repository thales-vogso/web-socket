/****************************************************************************
*	@Copyright(c)	2015,Vogso
*	@desc	主程序
*	@date	2015-9-30
*	@author	minamoto
*	@E-mail	jiangtai@wushuang.me
*	@file	Socket.cpp
******************************************************************************/
#include "Client.h"
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include <sstream>
#include <fcntl.h>

#include "json/json.h"

#include "lib/sha1.c"
#include "lib/base64.c"

using namespace std;

int Client::increaseID = 100001;

Client::Client():_newFD(true){
	_id = increaseID++;
	_name = "";
}

Client::~Client(){
	close();
}

bool Client::connect(int fd){
	int res = accept(fd);
	return res != -1;
}
bool Client::disconnect(){
	int res = close();
	return res != -1;
}

// Data Transmission
bool Client::send(std::string msg)
{
	std::cout << "send is \r\n" << msg << std::endl;
	std::string buf = outgoingPokect(msg);
	int res = ::send(getFileDescripto(), buf.c_str(), buf.length(), MSG_NOSIGNAL);;
	return res != -1;
}

std::string Client::receive()
{
	char buf[MAXLEN];
	std::string msg;
	int res = ::recv(getFileDescripto(), buf, MAXLEN, 0);
	if(res==-1){
		std::cout<<"error in Socket::Receive\n"<<std::endl;
		msg = "-1";
	}else if(res==0){
		msg = "0";
	}else {
		msg = incomePocket(buf);
	}
	return msg;
}

int Client::getID(){
	return _id;
}
void Client::setName(std::string name){
	_name = name;
}
std::string Client::getName(){
	std::stringstream sstr;
	sstr << "id:" << _id;
	if(_name == "") _name = sstr.str();
	return _name;
}


bool Client::setHeader(){
	if(!_newFD) return false;
	websocketHandshake();
	_newFD = false;
	return true;
}
	
std::string Client::fetchSecKey(const char* buf){
	std::string key = "";
	char *keyBegin;
	const char *flag="Sec-WebSocket-Key: ";
	int i=0, bufLen=0;
	if(!buf) {
		return "";
	}
	keyBegin=(char*)strstr(buf,flag);
	if(!keyBegin) {
		return "";
	}
	keyBegin+=strlen(flag);
	bufLen=strlen(buf);
	for(i=0; i<bufLen; i++) {
		if(keyBegin[i]==0x0A||keyBegin[i]==0x0D) {
			break;
		}
		key += keyBegin[i];
	}
	return key;
}

void Client::websocketHandshake(){
	std::string rev = receive();
	
	const char *GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	std::string clientKey = fetchSecKey(rev.c_str());
	if(clientKey == "") return;
	clientKey.append(GUID);
	char result[SHA1_LEN];
	char serverKey[MAXLEN];
	sha1_buffer (clientKey.c_str(), strlen(clientKey.c_str()), result);
	base64_encode (result, SHA1_LEN, serverKey, MAXLEN);
	
	//Return first part of the HTTP response
	std::string header = "HTTP/1.1 101 Switching Protocols\r\n";
	header.append("Upgrade: websocket\r\n");
	header.append("Connection: Upgrade\r\n");
	//header.append("Access-Control-Allow-Credentials: true\r\n");
	//header.append("Access-Control-Allow-Headers: content-type\r\n");
	header.append("Sec-WebSocket-Accept: ");
	header.append(serverKey);
	header.append("\r\n");
	header.append("Sec-WebSocket-Protocol: superchat\r\n\r\n");
	send(header);
}

std::string Client::incomePocket(char* buf){
	std::string msg;
	int i=0, j;
	int FIN = ((unsigned char)(buf[i]))>>7;
	int opcode = buf[i++]&15;
	int mask = (unsigned char)buf[i]>>7;
	int payloadLength = (unsigned char)buf[i++]&0x7F;
	if(payloadLength == 126)
		payloadLength = ((unsigned char)buf[i++]<<8)+buf[i++];
	if(payloadLength == 127)
		payloadLength = ((unsigned char)buf[i++]<<24)+((unsigned char)buf[i++]<<16)+((unsigned char)buf[i++]<<8)+buf[i++];
	if(mask){
		int maskingKey[4] = {buf[i++],buf[i++],buf[i++],buf[i++]};
		for(j=0; j<payloadLength; j++){
			msg.push_back(buf[i+j]^maskingKey[j%4]);
		}
		//std::cout << "fin=" << FIN << "\r\nopcode=" << Opcode << "\r\nmask=" << Mask << "\r\npayloadlength=" << PayloadLength << "\r\nstr=" << msg << "\r\n --end--" << std::endl;
	}else{
		msg = buf;
	}
	return msg;
}
std::string Client::outgoingPokect(std::string msg){
	if(_newFD) return msg;
	std::string buf;
	unsigned long long payloadSize = msg.size();
	char payloadFlags = 129;
	buf.append(&payloadFlags, 1);
std::cout << "size is " << payloadSize << std::endl;
	if (payloadSize <= 125){
		char basicSize = payloadSize;
		buf.append(&basicSize, 1);
	}else if (payloadSize > 125 & payloadSize <= 65535){
		char basicSize = 126;
		buf.append(&basicSize, 1);

		char len[2];
		len[0] = ( payloadSize >> 8 ) & 255;
		len[1] = ( payloadSize ) & 255;
		buf.append(len, 2);
	}else{
		char basicSize = 127;
		buf.append(&basicSize, 1);

		char len[8];
		len[0] = ( payloadSize >> 56 ) & 255;
		len[1] = ( payloadSize >> 48  ) & 255;
		len[2] = ( payloadSize >> 40 ) & 255;
		len[3] = ( payloadSize >> 32  ) & 255;
		len[4] = ( payloadSize >> 24 ) & 255;
		len[5] = ( payloadSize >> 16  ) & 255;
		len[6] = ( payloadSize >> 8 ) & 255;
		len[7] = ( payloadSize ) & 255;
		buf.append(len, 8);
	}
	buf.append(msg.c_str(), msg.size());
	return buf;
}
