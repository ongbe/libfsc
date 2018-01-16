/*
 * Fusr.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef ACTOR_FUSR_H_
#define ACTOR_FUSR_H_

#include "Actor.h"

class Fusr: public Actor
{
public:
	string uid;
public:
	Fusr(const char* uid, int wk);
	virtual ~Fusr();
};

#endif /* ACTOR_FUSR_H_ */
