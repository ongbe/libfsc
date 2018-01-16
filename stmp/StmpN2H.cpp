/*
 * StmpN2H.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "StmpN2H.h"

#include "StmpN2H.h"
#include "StmpFusr.h"
#include "StmpSsc.h"
#include "StmpTransPassive.h"
#include "StmpNet.h"
#include "../core/Fsc.h"

StmpN2H::StmpN2H(int cfd, int wk, struct sockaddr_in* peer) :
		StmpNet(ActorType::ACTOR_N2H, wk)
{
	this->cfd = cfd;
	this->est = true;
	this->peer = Net::sockaddr2str(peer);
	this->gts = Fsc::now;
	this->fusr = NULL;
}

/** 连接断开事件. */
void StmpN2H::evnDis()
{
	this->est = false;
	LOG_TRACE("have a connection lost, cfd: %d, peer: %s, wk: %d", this->cfd, this->peer.c_str(), this->wk);
	if (this->fusr != NULL)
	{
		this->fusr->evnDis();
		delete this->fusr;
	}
	if (Fsc::sscEnable && !this->ne.empty())
		StmpSsc::del(&this->ne);
	this->del();
}

/** 连接上的消息事件. */
bool StmpN2H::evnMsg(stmp_node* root)
{
	switch (root->self.t)
	{
	case STMP_TAG_TRANS_BEGIN:
		return this->evnBegin(root);
	case STMP_TAG_TRANS_END:
		return this->evnEnd(root);
	case STMP_TAG_TRANS_CONTINUE:
		return this->evnContinue(root);
	case STMP_TAG_TRANS_DIALOG:
		return this->evnDialog(root);
	case STMP_TAG_TRANS_SWITCH:
		return this->evnSwitch(root);
	case STMP_TAG_TRANS_CANCEL:
		return this->evnCancel(root);
	case STMP_TAG_TRANS_ABORT:
		return this->evnAbort(root);
	case STMP_TAG_TRANS_UNI:
		return this->evnUni(root);
	case STMP_TAG_TRANS_PART:
		return this->evnPart(root);
	case STMP_TAG_TRANS_PING:
		return this->evnPing(root);
	default:
		LOG_DEBUG("unsupported STMP transaction: %04X", root->self.t)
		return false;
	}
}

bool StmpN2H::evnBegin(stmp_node* root)
{
	if (Fsc::sscEnable)
	{
		uint dlen;
		uchar* dne = stmpdec_peek_bin(root, STMP_TAG_DNE, &dlen);
		if (dne != NULL)
		{
			if (dlen > STMP_NE_MAX_LEN)
			{
				LOG_DEBUG("DNE length over the STMP_NE_MAX_LEN: %d, this: %s", dlen, this->toString().c_str())
				return false;
			}
			if (this->ne.empty())
			{
				LOG_DEBUG("SNE is not ready: %s", this->toString().c_str())
				return false;
			}
			string str((char*) dne, dlen);
			return this->switchBegin(root, &str);
		}
	}
	//
	uint stid;
	Message* begin;
	StmpCb* cb;
	if (!this->decodeBegin(root, &stid, &begin, &cb))
		return false;
	((void (*)(StmpN2H* n2h, Message* begin, StmpTransPassive* trans)) (cb->cb))(this, begin, new StmpTransPassive(this, begin, stid));
	return true;
}

bool StmpN2H::evnEnd(stmp_node* root)
{
	return true;
}

bool StmpN2H::evnContinue(stmp_node* root)
{
	return true;
}

bool StmpN2H::evnDialog(stmp_node* root)
{
	return true;
}

bool StmpN2H::evnSwitch(stmp_node* root)
{
	return true;
}

bool StmpN2H::evnCancel(stmp_node* root)
{
	return true;
}

bool StmpN2H::evnAbort(stmp_node* root)
{
	return true;
}

bool StmpN2H::evnUni(stmp_node* root)
{
	return true;
}

bool StmpN2H::evnPart(stmp_node* root)
{
	return true;
}

bool StmpN2H::evnPing(stmp_node* root)
{
	static uchar pong[] = { STMP_TAG_TRANS_PONG };
	this->send(pong, 1);
	return true;
}

string StmpN2H::toString()
{
	string str;
	SPRINTF_STRING(&str, "cfd: %d, peer: %s", this->cfd, this->peer.c_str())
	return str;
}

StmpN2H::~StmpN2H()
{

}

