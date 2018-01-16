/*
 * FscLog.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef CORE_FSCLOG_H_
#define CORE_FSCLOG_H_

#include <libmisc.h>

class FscLog
{
public:
	virtual void quartz(ullong now) = 0;
public:
	FscLog();
	virtual ~FscLog();
};

#endif /* CORE_FSCLOG_H_ */
