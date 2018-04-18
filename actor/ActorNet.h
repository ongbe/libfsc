/*
 * ActorNet.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef ACTOR_ACTORNET_H_
#define ACTOR_ACTORNET_H_

#include <libmisc.h>
#include "../core/Fworker.h"

typedef struct
{
	int len; /* 缓冲区的长度. */
	int pos; /* 缓冲区的写位置. */
	uchar* dat; /* 缓冲区. */
} an_wbuf; /* 手动发送缓冲区管理. */

class ActorNet: public Actor
{
public:
	bool est; /* 连接是否已建立. */
	bool wevn; /* 在连接上是否已关注过写事件. */
	int tid; /* 连接上的事务id发生器. */
	int cfd; /* 远端描述字. */
	int dlen; /* rbuf的剩余长度. */
	uchar* rbuf; /* 读缓冲区. */
	list<an_wbuf*>* wbuf; /* 写缓冲区. */
	ullong gts; /* 连接建立时间. */
	ullong lts; /* 连接上最后收到消息的时间. */
	string peer; /* 远端地址描述字. */
public:
	void send(uchar* dat, int len); /* 消息出栈. */
	void sendBuf(uchar* dat, int len); /* 消息出栈. */
	void close(); /* 连接关闭. */
	void closeSlient(); /* 连接静默关闭. */
public:
	void evnWrite(); /* 连接上的写事件. */
	virtual void evnDis() = 0; /* 连接断开事件. */
public:
	ActorNet(ActorType type, int wk);
	virtual ~ActorNet();
};

#endif /* ACTOR_ACTORNET_H_ */
