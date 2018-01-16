/*
 * ActorNet.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "ActorNet.h"
#include "../core/Cfg.h"
#include "../core/Fsc.h"
#include "../core/FscStat.h"

ActorNet::ActorNet(ActorType type, int wk) :
		Actor(type, wk)
{
	this->est = false;
	this->needBlock = false;
	this->tid = 0;
	this->cfd = 0;
	this->dlen = 0;
	this->rbuf = (uchar*) malloc(Fsc::peer_mtu);
	this->wbuf = new list<an_wbuf*>();
	this->gts = 0ULL;
	this->lts = 0ULL;
}

/** 消息出栈. */
void ActorNet::send(uchar* dat, int len)
{
	FscStat::incv(FscStatItem::LIBFSC_SND_BYTES, len);
	FscStat::inc(FscStatItem::LIBFSC_SND_MSGS);
	LOG_RECORD("\n  --> PEER: %s CFD: %d\n%s%s", this->peer.c_str(), this->cfd, Misc::printhex2str(dat, len).c_str(), len == 1 ? "" : stmpdec_print2str(dat, len).c_str())
	if (!this->est)
		return;
	if (this->wbuf == NULL) /* 简单粗暴型或客户端不可信赖型. */
	{
		if (::send(this->cfd, dat, len, MSG_DONTWAIT) == len) /* 全部到达TCP缓冲区. */
			return;
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			LOG_DEBUG("TCP buffer was full, can not send anymore, we will close this peer: %s, size: %08X", this->peer.c_str(), len)
		} else
		{
			LOG_DEBUG("client socket exception, peer: %s, cfd: %d, size: %08X, errno: %d", this->peer.c_str(), this->cfd, len, errno)
		}
		Fsc::getFwk()->removeActorNet(this);
		this->evnDis();
		return;
	}
	this->sendBuf(dat, len); /* 应用层处理发送缓冲区. */
}

void ActorNet::sendBuf(uchar* dat, int len)
{
	/** -------------------------------- */
	/**                                  */
	/** 不需阻塞. */
	/**                                  */
	/** -------------------------------- */
	if (!this->needBlock)
	{
		int w = ::send(this->cfd, dat, len, MSG_DONTWAIT);
		if (w == len) /* 全部到达TCP缓冲区. */
			return;
		if (errno == EAGAIN || errno == EWOULDBLOCK) /* 需要阻塞. */
		{
			an_wbuf* wbuf = (an_wbuf*) ::malloc(sizeof(an_wbuf));
			wbuf->len = len - w;
			wbuf->pos = 0;
			wbuf->dat = (uchar*) ::malloc(wbuf->len);
			::memcpy(wbuf->dat, dat + w, wbuf->len);
			this->wbuf->push_back(wbuf);
			this->needBlock = true;
			Fsc::getFwk()->addCfd4Write(this->cfd);
			return;
		}
		Fsc::getFwk()->removeActorNet(this);
		this->evnDis();
		return;
	}
	/** -------------------------------- */
	/**                                  */
	/** 等待写事件. */
	/**                                  */
	/** -------------------------------- */
	an_wbuf* wbuf = (an_wbuf*) ::malloc(sizeof(an_wbuf));
	wbuf->len = len;
	wbuf->pos = 0;
	wbuf->dat = (uchar*) ::malloc(wbuf->len);
	::memcpy(wbuf->dat, dat, wbuf->len);
	this->wbuf->push_back(wbuf);
}

/** 连接上的写事件. */
void ActorNet::evnWrite(Fworker* fwk)
{
	this->needBlock = false;
	while (!this->wbuf->empty())
	{
		an_wbuf* wbuf = this->wbuf->front();
		int r = wbuf->len - wbuf->pos;
		int w = ::send(this->cfd, wbuf->dat + wbuf->pos, r, MSG_DONTWAIT);
		if (w == r) /* 已全部写入. */
		{
			this->wbuf->pop_front();
			::free(wbuf->dat);
			::free(wbuf);
			continue;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK) /* 需阻塞. */
		{
			wbuf->pos += w;
			this->needBlock = true;
			return;
		}
		Fsc::getFwk()->removeActorNet(this); /* 连接已失去. */
		this->evnDis();
		return;
	}
}

/** 连接关闭. */
void ActorNet::close()
{
	this->closeSlient();
	this->evnDis();
}

/** 连接静默关闭. */
void ActorNet::closeSlient()
{
	if (!this->est)
		return;
	Fsc::getFwk()->removeActorNet(this);
}

ActorNet::~ActorNet()
{
	::free(this->rbuf);
	if (this->wbuf != NULL)
	{
		while (!this->wbuf->empty())
		{
			an_wbuf* wbuf = this->wbuf->front();
			this->wbuf->pop_front();
			free(wbuf->dat);
			free(wbuf);
			continue;
		}
		delete this->wbuf;
	}
}

