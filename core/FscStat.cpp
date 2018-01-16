/*
 * FscStat.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "FscStat.h"

static ullong items[FscStatItem::STAT_END] = { 0 };

FscStat::FscStat()
{

}

void FscStat::inc(uint item)
{
	__sync_fetch_and_add(items + (item % FscStatItem::STAT_END), 1);
}

/** 自增v. */
void FscStat::incv(uint item, ullong v)
{
	__sync_fetch_and_add(items + (item % FscStatItem::STAT_END), v);
}

/** 获得某项的值. */
ullong FscStat::get(uint item)
{
	return items[item % FscStatItem::STAT_END];
}

FscStat::~FscStat()
{

}

