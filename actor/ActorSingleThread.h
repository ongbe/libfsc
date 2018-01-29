/**
 * 
 * Created on: Jan 24, 2018 9:04:38 AM
 *
 * Author: toby
 *
 */
#ifndef ACTOR_ACTORSINGLETHREAD_H_
#define ACTOR_ACTORSINGLETHREAD_H_

#include "ActorBlocking.h"

/**
 * 这是一个单线程的可阻塞的Actor, 它还携带了一个定时器实现.
 */

class ActorSingleThread: public ActorBlocking
{
public:
	ActorSingleThread();
	virtual ~ActorSingleThread();
};

#endif /* ACTOR_ACTORSINGLETHREAD_H_ */
