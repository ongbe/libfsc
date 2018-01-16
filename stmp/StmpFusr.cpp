/*
 * StmpFusr.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "StmpFusr.h"

StmpFusr::StmpFusr(const char* uid, StmpN2H* n2h) :
		Fusr(uid, n2h->wk)
{
	this->n2h = n2h;
}

string StmpFusr::toString()
{
	return "";
}

StmpFusr::~StmpFusr()
{

}

