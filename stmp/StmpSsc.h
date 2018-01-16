/*
 * StmpSsc.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef CORE_STMPSSC_H_
#define CORE_STMPSSC_H_

#include "../fscinc.h"

class StmpNet;
/**
 *
 * Service Switch Center.
 *
 */
class StmpSsc
{
public:
	static StmpSsc* instance();
public:
	static void init();
	static StmpNet* get(string* ne); /* 查找网元. */
	static void del(string* ne); /* 删除网元. */
private:
	static StmpSsc* inst;
private:
	StmpSsc();
	virtual ~StmpSsc();
};

#endif /* CORE_STMPSSC_H_ */
