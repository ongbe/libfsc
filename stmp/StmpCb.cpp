/*
 * StmpCb.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "StmpCb.h"

StmpCb::StmpCb(const Descriptor* begin, const Descriptor* end, const Descriptor* uni, void* cb, RpcType type, bool fusr, const char* doc)
{
	this->begin = begin;
	this->end = end;
	this->uni = uni;
	this->cb = cb;
	this->type = type;
	this->fusr = fusr;
	if (doc != NULL)
		this->doc.assign(doc);
}

StmpCb::~StmpCb()
{

}

