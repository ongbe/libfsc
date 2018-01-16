/*
 * FscStat.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef CORE_FSCSTAT_H_
#define CORE_FSCSTAT_H_

#include <libmisc.h>

enum FscStatItem
{
	LIBFSC_RCV_BYTES = 0x00, /** 服务器收到的字节数. */
	LIBFSC_RCV_MSGS, /** 服务器收到的消息数. */
	LIBFSC_SND_BYTES, /** 服务器发出的字节数. */
	LIBFSC_SND_MSGS, /** 服务器发出的消息数. */
	//
	LIBFSC_N2H_TOTAL, /** 总共生成的N2H数量. */
	LIBFSC_N2H_DELETE, /** 调用了delete n2h的数量. */
	STAT_END
};

class FscStat
{
private:
	FscStat();
	virtual ~FscStat();
public:
	static void inc(uint item); /** 自增1. */
	static void incv(uint item, ullong v); /** 自增v. */
	static ullong get(uint item); /** 获得某项的值. */
};

#endif /* CORE_FSCSTAT_H_ */
