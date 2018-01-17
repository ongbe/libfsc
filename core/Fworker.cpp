/*
 * Fworker.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "Fworker.h"
#include "Fsc.h"
#include "Cfg.h"
#include "FscStat.h"
#include "../actor/ActorNet.h"
#include "../stmp/StmpN2H.h"

Fworker::Fworker() :
		Actor(ActorType::ACTOR_ITC, INVALID)
{
	this->wk = 0;
	this->busy = false;
	this->efd = epoll_create(Cfg::libfsc_peer_limit / Fsc::worker);
	this->evn = eventfd(0, EFD_NONBLOCK);
	this->t = 0;
	pthread_mutex_init(&this->mutex, NULL);
}

/* lambda排队. */
void Fworker::push(actor_future* f)
{
	pthread_mutex_lock(&this->mutex);
	this->afs.push(f);
	pthread_mutex_unlock(&this->mutex);
	//
	static ullong count = 1;
	::write(this->evn, &count, sizeof(ullong));
}

/* 线程入口. */
void Fworker::loop()
{
	pthread_setspecific(Fsc::pkey, this);
	int size = Cfg::libfsc_peer_limit / Cfg::libfsc_worker;
	struct epoll_event* evns = (struct epoll_event*) calloc(1, sizeof(struct epoll_event) * size);
	this->addCfd4Read(this->evn);
	LOG_INFO("worker-thread start successfully, index: %02X, epoll-fd: %08X", this->wk, this->efd)
	int i = 0;
	int count = 0;
	while (1)
	{
		this->busy = false;
		count = epoll_wait(this->efd, evns, size, -1);
		this->busy = true;
		for (i = 0; i < count; ++i)
		{
			if (evns[i].events & EPOLLOUT) /* 写事件. */
			{
				this->evnSend(evns + i);
				continue;
			}
			if (!(evns[i].events & EPOLLIN))
			{
				this->evnErro(evns + i); /* 错误事件. */
				continue;
			}
			if (evns[i].data.fd == Fsc::sfd)
			{
				this->evnConn(); /* 连接到来事件. */
				continue;
			}
			this->evnRecv(evns + i); /* 读事件. */
		}
		this->doFuture(); /* 处理可能到达的future事件. */
	}
}

/* 连接到来事件. */
void Fworker::evnConn()
{
	struct sockaddr_in peer;
	socklen_t socklen = sizeof(struct sockaddr_in);
	int cfd;
	while (1) /* got the all in-coming connection. */
	{
		cfd = ::accept(Fsc::sfd, (struct sockaddr*) &peer, &socklen);
		if (cfd == -1) /* ( errno == EAGAIN || errno == EWOULDBLOCK) || thundering herd */
			break;
		if (cfd >= Cfg::libfsc_peer_limit)
		{
			LOG_FAULT("over the LIBFSC_PEER_LIMIT: %u, we will close this connection, cfd: %u", Cfg::libfsc_peer_limit, cfd)
			Net::close(cfd);
			continue;
		}
		/*
		 *  由于::accept被设计成工作在单线程下, 所以需要对上来的连接进行分配.
		 *  这里采取的策略是, 将cfd对LIBFSC_WORKER进行取模, 以期待尽量的负载均衡.
		 *  因此导致的结果是, 某个描述字总是分配给某个固定的线程, 这样在操作Fworker::ans散列表时更为方便(不会出现同一个描述字(值)出现在不同的ans散列表中).
		 */
		Fworker* f = Fsc::wks + Fsc::hashWk(cfd);
		StmpN2H* n2h = new StmpN2H(cfd, f->wk, &peer);
		FscStat::inc(FscStatItem::LIBFSC_N2H_TOTAL);
		n2h->future([cfd, n2h, f]
		{
			LOG_TRACE("got a connection from: %s, cfd: %d", n2h->peer.c_str(), cfd)
			f->setFdAtt(cfd);
			f->addActorNet(n2h);
			f->addCfd4Read(cfd);
		});
	}
}

/* 连接上的写事件. */
void Fworker::evnSend(struct epoll_event* evn)
{
	auto it = this->ans.find(evn->data.fd);
	if (it == this->ans.end()) /* 不应该找不到. */
	{
		LOG_FAULT("it`s a bug, fd: %d", evn->data.fd)
		return;
	}
	ActorNet* an = it->second;
	an->ref(); /* 如果消息处理失败, an可能已经被删除, 为防止这种情况, 这里引用一次, 并在下面释放. */
	an->evnWrite(this);
	an->unRef();
}

/* 连接上的读事件. */
void Fworker::evnRecv(struct epoll_event* evn)
{
	if (evn->data.fd == this->evn) /* 线程唤醒事件. */
	{
		this->evnItc();
		return;
	}
	auto it = this->ans.find(evn->data.fd);
	if (it == this->ans.end())
	{
		LOG_FAULT("it`s a bug.")
		return;
	}
	ActorNet* an = it->second;
	an->ref(); /* 如果消息处理失败, an可能已经被删除, 为防止这种情况, 这里引用一次, 并在下面释放. */
	int count;
	bool flag = true;
	while (flag)
	{
		count = ::recv(evn->data.fd, an->rbuf + an->dlen, Fsc::peer_mtu - an->dlen, MSG_DONTWAIT);
		if (count < 1)
		{
			if ((count == -1 && errno != EAGAIN) || count == 0)
				flag = false;
			break;
		}
		an->dlen += count;
		FscStat::incv(FscStatItem::LIBFSC_RCV_BYTES, count);
		if (Fsc::proto == FscProtocolType::STMP)
			flag = ((StmpNet*) an)->evnRead();
		else
		{
			flag = false;
			LOG_FAULT("it`s a bug, forgot set FscProtocolType?")
		}
	}
	if (!flag)
	{
		LOG_DEBUG("have a client disconnected: %s, errno: %d", an->toString().c_str(), errno)
		if (an->est)
		{
			this->removeActorNet(an);
			an->evnDis();
		} else
			; /** 如果连接已经不在, 则一定是已经调用过evnDis和上面的removeActorNet. */
	}
	an->unRef();
}

/* 连接上的错误事件. */
void Fworker::evnErro(struct epoll_event* evn)
{
	auto it = this->ans.find(evn->data.fd);
	if (it == this->ans.end())
	{
		LOG_WARN("can not found ActorNet for cfd: %d", evn->data.fd)
		return;
	}
	ActorNet* an = it->second;
	LOG_TRACE("have a client disconnected: %s, errno: %d", an->toString().c_str(), errno)
	this->removeActorNet(an);
}

/* eventfd读事件. */
void Fworker::evnItc()
{
	static ullong count;
	while (::read(this->evn, &count, sizeof(ullong)) > 0)
		;/** 一次读尽. */
}

/* 处理可能到达的future. */
void Fworker::doFuture()
{
	while (1)
	{
		actor_future* f = NULL;
		this->busy = false;
		pthread_mutex_lock(&this->mutex);
		if (this->afs.empty())
		{
			pthread_mutex_unlock(&this->mutex);
			return;
		}
		f = this->afs.front();
		this->afs.pop();
		pthread_mutex_unlock(&this->mutex);
		this->busy = true;
		f->cb();
		delete f;
	}
}

/* 添加ActorNet. */
void Fworker::addActorNet(ActorNet* an)
{
	auto it = this->ans.find(an->cfd);
	if (it != this->ans.end()) /* 描述字对应的ActorNet应该早已被删除. */
	{
		LOG_FAULT("it`s a bug, an: %s", an->toString().c_str())
		return;
	}
	this->ans[an->cfd] = an;
}

/* 移除ActorNet. */
void Fworker::removeActorNet(ActorNet* an)
{
	this->ans.erase(an->cfd);
	this->delCfd(an->cfd);
	Net::close(an->cfd);
	an->est = false;
}

/* 定时器振荡. */
void Fworker::quartz(ullong now)
{

}

/* 设置N2H/H2N套接字属性. */
void Fworker::setFdAtt(int cfd)
{
	Net::setNoBlocking(cfd);
	Net::setLinger(cfd);
	Net::setSNDBUF(cfd, Cfg::libfsc_peer_sndbuf / 2);
	Net::setRCVBUF(cfd, Cfg::libfsc_peer_rcvbuf / 2);
	Net::setNODELAY(cfd);
}

/* 注册读事件. */
void Fworker::addCfd4Read(int cfd)
{
	struct epoll_event ce = { 0 };
	ce.data.fd = cfd;
	ce.events = EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLET;
	if (epoll_ctl(this->efd, EPOLL_CTL_ADD, cfd, &ce) == -1)
		LOG_FAULT("add FD to epoll failed, cfd: %d, errno: %d", cfd, errno)
}

/* 注册写事件. */
void Fworker::addCfd4Write(int cfd)
{
	struct epoll_event ce = { 0 };
	ce.data.fd = cfd;
	ce.events = EPOLLOUT | EPOLLERR | EPOLLRDHUP | EPOLLET;
	if (epoll_ctl(this->efd, EPOLL_CTL_ADD, cfd, &ce) == -1)
		LOG_FAULT("add FD to epoll failed, cfd: %d, errno: %d", cfd, errno)
}

void Fworker::delCfd(int cfd)
{
	struct epoll_event ce = { 0 };
	ce.data.fd = cfd;
	ce.events = EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLET;
	if (epoll_ctl(this->efd, EPOLL_CTL_DEL, cfd, &ce) == -1)
		LOG_FAULT("remove FD from epoll failed, cfd: %d, errno: %d", cfd, errno)
}

string Fworker::toString()
{
	string str;
	return str;
}

Fworker::~Fworker()
{

}

