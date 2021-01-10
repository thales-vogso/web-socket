/****************************************************************************
*	@Copyright(c)	2015,Vogso
*	@desc	主程序
*	@date	2015-10-13
*	@author	minamoto
*	@E-mail	jiangtai@wushuang.me
*	@file	main.cpp
******************************************************************************/

#include <iostream>
#include <string>
#include "Epoll.h"

using namespace std;

int main()
{
	cout<<"Running server..."<<endl;
	Epoll epoll;
	epoll.run();
	return 0;
}
