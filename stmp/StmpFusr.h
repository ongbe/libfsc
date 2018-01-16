/*
 * StmpFusr.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef STMP_STMPFUSR_H_
#define STMP_STMPFUSR_H_

#include "../actor/Fusr.h"
#include "StmpN2H.h"

class StmpFusr: public Fusr
{
public:
	StmpN2H* n2h;
public:
	string toString();
public:
	StmpFusr(const char* uid, StmpN2H* n2h);
	virtual void evnDis() = 0; /* 连接断开事件. */
	virtual ~StmpFusr();
};

#endif /* STMP_STMPFUSR_H_ */
