/*
 * StmpN2H.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef STMP_STMPN2H_H_
#define STMP_STMPN2H_H_

#include "StmpNet.h"

class StmpFusr;

class StmpN2H: public StmpNet
{
public:
	string ne; /** 网元标识. */
	StmpFusr* fusr; /* 连接上的用户数据. */
public:
	void evnDis(); /** 连接断开事件. */
	bool evnMsg(stmp_node* root); /** 连接上的消息事件. */
	bool evnBegin(stmp_node* root);
	bool evnEnd(stmp_node* root);
	bool evnContinue(stmp_node* root);
	bool evnDialog(stmp_node* root);
	bool evnSwitch(stmp_node* root);
	bool evnCancel(stmp_node* root);
	bool evnAbort(stmp_node* root);
	bool evnUni(stmp_node* root);
	bool evnPart(stmp_node* root);
	bool evnPing(stmp_node* root);
public:
	StmpN2H(int cfd, int wk, struct sockaddr_in* peer);
	string toString();
	virtual ~StmpN2H();
};

#endif /* STMP_STMPN2H_H_ */
