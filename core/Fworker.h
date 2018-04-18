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
	volatile bool busy; /* 线程忙 ? true : false.*/
	int efd; /* epoll句柄. */
	int evn; /* eventfd句柄. */
	pthread_t t; /* 线程标识. */
	pthread_mutex_t mutex; /* 线程上的锁. */
	queue<actor_future*> afs; /* 待处理的lambda. */
	unordered_map<int /* socket-fd. */, ActorNet*> ans; /* 工作线程上的网络连接. */
	unordered_map<string /* message-name/path/path/path/. */, list<pair<StmpNet* /* 订阅者. */, uint /* session id. */>>*> subscribers; /* 被订阅的服务. */
	list<StmpH2N*> h2ns; /* 线程上的STMP-H2N连接. */
public:
	void push(actor_future* f); /* lambda排队. */
	string toString();
public:
	void quartz(ullong now); /* 定时器振荡. */
	void loop(); /* Fworker线程入口.*/
	void addActorNet(ActorNet* an); /* 添加ActorNet. */
	void removeActorNet(ActorNet* an); /* 移除ActorNet. */
	void addCfd4Read(int cfd); /* 注册读事件. */
	void addCfd4ReadWrite(int cfd); /* 注册读写事件. */
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
