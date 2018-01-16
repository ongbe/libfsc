/*
 * ActorBlocking.h
 *
 *  Created on: Feb 4, 2015 12:36:14 PM
 *      Author: xuzewen
 */

#ifndef ACTORBLOCKING_H_
#define ACTORBLOCKING_H_

#include "Actor.h"

class ActorBlocking: public Actor
{
private:
	queue<actor_future*> afs; /** 缓存的待调用的lambda. */
	pthread_mutex_t mutex; /** afs上的锁. */
	pthread_cond_t cond; /** afs上的条件. */
	bool busy; /** 忙 ? true : false. */
	static void* svc(void* arg);
public:
	ActorBlocking(int pool);
	virtual ~ActorBlocking();
	void push(actor_future* f);
	bool isBusy(); /** 线程忙 ? true : false. */
};

#endif /* ACTORBLOCKING_H_ */
