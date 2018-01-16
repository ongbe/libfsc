/*
 * FscMisc.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "FscMisc.h"

FscMisc::FscMisc()
{

}

/* 反射得到一个pb message对象. */
Message* FscMisc::newPbMsg(const Descriptor* desc)
{
	const Message* pt = MessageFactory::generated_factory()->GetPrototype(desc);
	if (pt == NULL)
	{
		printf("can`t not found message type stub for this desc, pb name: %s", desc->name().c_str());
		return NULL;
	}
	return pt->New();
}

/* 反射得到一个pb message对象. */
Message* FscMisc::newPbMsg(const Descriptor* desc, uchar* dat, int len)
{
	Message* msg = FscMisc::newPbMsg(desc);
	if (msg == NULL)
		return NULL;
	if (msg->ParseFromArray(dat, len))
		return msg;
	LOG_ERROR("serialize message failed, name: %s", desc->name().c_str());
	delete msg;
	return NULL;
}

FscMisc::~FscMisc()
{

}

