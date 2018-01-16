/*
 * Ssc.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "StmpSsc.h"
#include "StmpNet.h"

StmpSsc* StmpSsc::inst = new StmpSsc();

StmpSsc::StmpSsc()
{

}

void StmpSsc::init()
{

}

/* 查找网元. */
StmpNet* StmpSsc::get(string* ne)
{
	return NULL;
}

/* 删除网元. */
void StmpSsc::del(string* ne)
{

}

StmpSsc* StmpSsc::instance()
{
	return StmpSsc::inst;
}

StmpSsc::~StmpSsc()
{

}

