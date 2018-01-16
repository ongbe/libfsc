/*
 * FscMisc.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef CORE_FSCMISC_H_
#define CORE_FSCMISC_H_

#include "../fscinc.h"

class FscMisc
{
public:
	static Message* newPbMsg(const Descriptor* desc); /* 反射得到一个pb message对象. */
	static Message* newPbMsg(const Descriptor* desc, uchar* dat, int len); /* 反射得到一个pb message对象. */
private:
	FscMisc();
	virtual ~FscMisc();
};

#endif /* CORE_FSCMISC_H_ */
