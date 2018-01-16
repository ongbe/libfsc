/*
 * StmpCb.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef STMP_STMPCB_H_
#define STMP_STMPCB_H_

#include "../fscinc.h"

enum RpcType
{
	RPC_TYPE_IRPC, /* 一个INITIATIVE-RPC调用(指的是主动调用其它网元提供的RPC接口). */
	RPC_TYPE_PRPC, /* 一个PASSIVE-RPC调用(指的是当前网元向外提供的RPC接口). */
	RPC_TYPE_SUBSCIBE, /* 一个SUBSCIBE(指的是订阅其它它网元的服务, 也就是subscribe/publish对). */
	RPC_TYPE_SERVICE /* 一个SERVICE(指的是可以被其它网元订阅的服务, 也就是subscribe/publish对). */
};

/**
 *
 * StmpNet上的回调函数.
 *
 */
class StmpCb
{
public:
	bool fusr; /* 鉴权前/后. */
	RpcType type; /* RPC调用类型. */
	void* cb; /* 回调函数. */
	string msg; /* 消息名称. */
	string doc; /* 消息描述. */
	const Descriptor* begin;
	const Descriptor* end;
	const Descriptor* uni;
public:
	StmpCb(const Descriptor* begin, const Descriptor* end, const Descriptor* uni, void* cb, RpcType type, bool fusr, const char* doc);
	virtual ~StmpCb();
};

#endif /* STMP_STMPCB_H_ */
