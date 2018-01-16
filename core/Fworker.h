/*
 * Fworker.h
 *
 *  Created on: Jan 15, 2018
 *      Author: toby
 */

#ifndef CORE_FWORKER_H_
#define CORE_FWORKER_H_

#include "../actor/Actor.h"

class ActorNet;
class StmpNet;
class StmpH2N;

class Fworker: public Actor
{
public:
	volatile bool busy;
	int efd;
	int evn;
	pthread_t t;
	pthread_mutex_t mutex;
	queue<actor_future*> afs;
	unordered_map<int /* socket-fd. */, ActorNet*> ans;
	unordered_map<string /* message-name. */, unordered_set<StmpNet*>*> subcribes;
	list<StmpH2N*> h2ns;
public:
	void push(actor_future* f); /* lambda排队. */
	string toString();
public:
	void quartz(ullong now); /* 定时器振荡. */
	void loop(); /* Fworker线程入口.*/
	void addActorNet(ActorNet* an); /* 添加ActorNet. */
	void removeActorNet(ActorNet* an); /* 移除ActorNet. */
	void addCfd4Write(int cfd); /* 注册写事件. */
	void addCfd4Read(int cfd); /* 注册读事件. */
	void delCfd(int cfd); /* 删除描述字. */
public:
	Fworker();
	virtual ~Fworker();
private:
	void evnConn(); /* 连接到来事件. */
	void evnSend(struct epoll_event* evn); /* 连接上的写事件. */
	void evnRecv(struct epoll_event* evn); /* 连接上的读事件. */
	void evnErro(struct epoll_event* evn); /* 连接上的错误事件. */
	void evnItc(); /* eventfd读事件. */
	void doFuture(); /* 处理可能到达的future. */
private:
	static void setFdAtt(int cfd); /* 设置N2H/H2N套接字属性. */
};

#endif /* CORE_FWORKER_H_ */
