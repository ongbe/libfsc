/*
 * ActorBlocking.cpp
 *
 *  Created on: Feb 4, 2015 12:36:14 PM
 *      Author: xuzewen
 */

#include "ActorBlocking.h"

ActorBlocking::ActorBlocking(int pool) :
		Actor(ActorType::ACTOR_BLOCKING, INVALID)
{
	this->busy = false;
	pthread_mutex_init(&this->mutex, NULL);
	pthread_cond_init(&this->cond, NULL);
	for (int i = 0; i < pool; ++i)
	{
		pthread_t t;
		if (pthread_create(&t, NULL, ActorBlocking::svc, this) == -1)
		{
			LOG_FAULT("can not create thread any more.")
			exit(1);
		}
		pthread_detach(t);
	}
}

void* ActorBlocking::svc(void* arg)
{
	ActorBlocking* ab = (ActorBlocking*) arg;
	LOG_INFO("actor-blocking-thread start successfully, tid: %lu", gettid())
	actor_future* f = NULL;
	while (1)
	{
		ab->busy = false;
		pthread_mutex_lock(&ab->mutex);
		while (ab->afs.empty())
			pthread_cond_wait(&ab->cond, &ab->mutex);
		f = ab->afs.front();
		ab->afs.pop();
		pthread_mutex_unlock(&ab->mutex);
		ab->busy = true;
		f->cb();
		delete f;
	}
	return NULL;
}

void ActorBlocking::push(actor_future* f)
{
	bool nll = false;
	pthread_mutex_lock(&this->mutex);
	nll = this->afs.empty();
	this->afs.push(f);
	pthread_mutex_unlock(&this->mutex);
	if (nll)
		pthread_cond_signal(&this->cond);
}

/** 线程忙 ? true : false. */
bool ActorBlocking::isBusy()
{
	return this->busy;
}

ActorBlocking::~ActorBlocking()
{

}

