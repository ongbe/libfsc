/*
 * Actor.cpp
 *
 *  Created on: Jul 31, 2014 9:05:43 AM
 *      Author: xuzewen
 */

#include "Actor.h"
#include "ActorBlocking.h"
#include "../core/Fsc.h"
#include "../core/Fworker.h"

Actor::Actor(ActorType type, int wk)
{
	this->rc = 0;
	this->rf = false;
	this->type = type;
	this->wk = wk;
}

/** future函数可以在任意线程中调用, 但lambda一定是在Actor所在的线程中被执行. */
void Actor::future(function<void()> cb)
{
	int wk = Fsc::getWk();
	if (this->type == ACTOR_BLOCKING)
	{
		actor_future* f = new actor_future();
		f->cb = cb;
		((ActorBlocking*) this)->push(f);
		return;
	}
	if (this->wk == wk)
		cb();
	else
	{
		actor_future* f = new actor_future();
		f->cb = cb;
		Fsc::wks[this->wk].push(f);
	}
}

/** 增加一个引用计数. */
void Actor::ref()
{
	this->rc += 1;
}

/** 减少一个引用计数. */
void Actor::unRef()
{
	this->rc -= 1;
	if (this->rc < 1 && this->rf) /** 已经没有其它引用, 且需要被删除. */
	{
		LOG_TRACE("no more thread refer to this actor, we will delete it: %s", this->toString().c_str())
		delete this;
	}
}

/** delete this. */
void Actor::del()
{
	if (this->wk != Fsc::getWk())
		LOG_FAULT("it`s a bug: %s", this->toString().c_str())
	this->rf = true;
	if (this->rc < 1) /** 需要被删除. */
	{
		LOG_TRACE("no more thread refer to this actor, we will delete it: %s", this->toString().c_str())
		delete this;
	}
}

/** 是否被设置了删除标记. */
bool Actor::isDel()
{
	return this->rf;
}

Actor::~Actor()
{

}

