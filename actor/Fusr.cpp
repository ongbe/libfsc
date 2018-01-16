/*
 * Fusr.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "Fusr.h"

Fusr::Fusr(const char* uid, int wk) :
		Actor(ActorType::ACTOR_ITC, wk)
{
	this->uid.assign(uid);
}

Fusr::~Fusr()
{

}

