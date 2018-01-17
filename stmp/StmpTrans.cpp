/*
 * StmpTrans.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "StmpTrans.h"

StmpTrans::StmpTrans(StmpNet* stmpNet, Message* begin, uint dtid)
{
	this->stmpNet = stmpNet;
	this->beginMsg = begin;
	this->continous = NULL;
	this->endMsg = NULL;
	this->dtid = dtid;
	this->sid = NULL;
	this->ret = 0;
}

void StmpTrans::continu(Message* continu)
{
	if (this->continous != NULL)
		this->continous = new list<Message*>();
	this->continous->push_back(continu);
	//
	this->stmpNet->future([this, continu]
	{
		this->stmpNet->sendContinue(this->dtid, continu);
		this->finish();
	});
}

void StmpTrans::end(ushort ret, Message* end)
{
	this->ret = ret;
	this->endMsg = end;
	if (ret == RET_FAILURE) /* 关闭连接. */
	{
		this->stmpNet->future([this]
		{
			this->stmpNet->close();
			this->finish();
		});
		return;
	}
	this->stmpNet->future([this]
	{
		this->stmpNet->sendEnd(this->dtid, this->ret, this->endMsg, this->sid);
		this->finish();
	});
}

void StmpTrans::end(Message* end)
{
	this->end(RET_SUCCESS, end);
}

void StmpTrans::end(ushort ret)
{
	this->end(ret, NULL);
}

void StmpTrans::success()
{
	this->end(RET_SUCCESS, NULL);
}

void StmpTrans::failure()
{
	this->end(RET_FAILURE, NULL);
}

void StmpTrans::finish()
{
	delete this;
}

StmpTrans::~StmpTrans()
{
	delete this->beginMsg;
	if (this->endMsg != NULL)
		delete this->endMsg;
	if (this->sid != NULL)
		::free(this->sid);
}

