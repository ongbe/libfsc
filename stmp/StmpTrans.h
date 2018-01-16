/*
 * StmpTrans.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef STMP_STMPTRANS_H_
#define STMP_STMPTRANS_H_

#include "StmpNet.h"

/**
 *
 * STMP事务抽象.
 *
 */
class StmpTrans
{
public:
	StmpNet* stmpNet; /** STMP连接. */
	Message* beginMsg; /* BEGIN.*/
	list<Message*>* continous; /* 可能的continue. */
	Message* endMsg; /* END.*/
	uint dtid; /* DTID, 指的是由BEGIN带来的事务ID. */
	ushort ret; /* 事务结束时的返回值. */
public:
	void continu(Message* continu);
	void end(ushort ret, Message* end);
	void end(Message* end);
	void end(ushort ret);
	void success();
	void failure();
	void finish();
public:
	StmpTrans(StmpNet* stmpNet, Message* begin, uint dtid);
	virtual ~StmpTrans();
};

#endif /* STMP_STMPTRANS_H_ */
