/**
 * 
 * Created on: Jan 24, 2018 9:09:57 AM
 *
 * Author: toby
 *
 */
#ifndef CORE_FTIMERMGR_H_
#define CORE_FTIMERMGR_H_

/**
 *
 * 基于时间轮的定时器实现.
 *
 * 它工作在单线程环境下.
 *
 */
class FtimerMgr
{
public:
	FtimerMgr();
	virtual ~FtimerMgr();
};

#endif /* CORE_FTIMERMGR_H_ */
