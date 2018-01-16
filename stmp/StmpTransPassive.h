/*
 * StmpTransPassive.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef STMP_STMPTRANSPASSIVE_H_
#define STMP_STMPTRANSPASSIVE_H_

#include "StmpTrans.h"

class StmpTransPassive: public StmpTrans
{
public:
	StmpTransPassive(StmpNet* stmpNet, Message* begin, uint dtid);
	virtual ~StmpTransPassive();
};

#endif /* STMP_STMPTRANSPASSIVE_H_ */
