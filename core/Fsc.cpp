/*
 * Fsc.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "Fsc.h"

#include "Cfg.h"
#include "Fsc.h"
#include "../stmp/StmpNet.h"
#include "../stmp/StmpSsc.h"

FscLog* Fsc::log = NULL;
Fworker* Fsc::wks;
pthread_key_t Fsc::pkey;
FscProtocolType Fsc::proto;
bool Fsc::sscEnable;
int Fsc::sfd;
int Fsc::worker;
ullong Fsc::now;
unordered_map<string, StmpCb*> Fsc::stmpN2HMsgs;

Fsc::Fsc()
{

}

bool Fsc::init(FscLog* log)
{
	Cfg::init();
	Fsc::log = log;
	Fsc::worker = Cfg::libfsc_worker;
	Fsc::peer_mtu = Cfg::libfsc_peer_mtu;
	Fsc::sscEnable = Cfg::libfsc_ssc_enable;
	if (Fsc::sscEnable)
		StmpSsc::init();
	//
	::signal(SIGPIPE, SIG_IGN);
	pthread_key_create(&Fsc::pkey, NULL);
	Fsc::wks = new Fworker[Fsc::worker];
	for (int i = 0; i < Fsc::worker; ++i)
	{
		Fsc::wks[i].wk = i;
		if (pthread_create(&Fsc::wks[i].t, NULL, Fsc::svc, (void*) (Fsc::wks + i)) != 0)
		{
			LOG_FAULT("no more thread can be create, libfsc server will be exist...")
			return false;
		}
	}
	Misc::sleep(100);
	return true;
}

bool Fsc::publish()
{
	Fsc::proto = (FscProtocolType) Cfg::libfsc_protocol;
	vector<string> tmp;
	Misc::split(Cfg::libfsc_server_addr.c_str(), ":", &tmp);
	Fsc::sfd = Net::tcpListen(tmp.at(0).c_str(), ::atoi(tmp.at(1).c_str()));
	if (Fsc::sfd < 0)
	{
		LOG_FAULT("libfsc can not listen on: %s, errno: %d", Cfg::libfsc_server_addr.c_str(), errno)
		return false;
	}
	Net::setNoBlocking(Fsc::sfd);
	/**
	 *
	 * 只有第一个Fworker处理accept事件.
	 *
	 * */
	struct epoll_event ee = { 0 };
	ee.data.fd = Fsc::sfd;
	ee.events = (EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLET);
	if (epoll_ctl(Fsc::wks[0].efd, EPOLL_CTL_ADD, Fsc::sfd, &ee) == -1)
	{
		LOG_FAULT("call epoll_ctl (EPOLL_CTL_ADD) failed, worker-indx: 0, efd: %d, errno: %d", Fsc::wks[0].efd, errno)
		return false;
	}
	LOG_INFO("libfsc already listen on %s, sfd: %d, and waiting for connection.", Cfg::libfsc_server_addr.c_str(), Fsc::sfd)
	return true;
}

void Fsc::hold()
{
	while (true)
	{
		Misc::sleep(1000); /* 定时器精度1sec. */
		Fsc::now = DateMisc::getMsec();
		for (int i = 0; i < Fsc::worker; ++i)
		{
			Fworker* wk = Fsc::wks + i;
			wk->future([wk]
			{
				wk->quartz(Fsc::now);
			});
		}
		if (Fsc::log != NULL)
			Fsc::log->quartz(Fsc::now);
	}
}

void* Fsc::svc(void* arg)
{
	((Fworker*) arg)->loop();
	return NULL;
}

Fworker* Fsc::getFwk()
{
	return (Fworker*) pthread_getspecific(Fsc::pkey);
}

int Fsc::getWk()
{
	Fworker* wk = (Fworker*) pthread_getspecific(Fsc::pkey);
	return wk == NULL ? -1 : wk->wk;
}

int Fsc::hashWk(ullong id)
{
	return (int) ((id & 0x7FFFFFFFFFFFFFFFL) % Fsc::worker);
}

void Fsc::publishMsg(string* subsribe, Message* publish)
{
	string* str = new string(*subsribe);
	int* counter = new int;
	*counter = 0;
	//
	for (int i = 0; i < Fsc::worker; ++i)
	{
		Fworker* wk = (Fsc::wks + i);
		wk->future([wk, str, publish, counter]
		{
			*counter = __sync_add_and_fetch(counter, 1);
			list<StmpNet*> list;
			auto it = wk->subcribes.find(*str);
			if(it == wk->subcribes.end())
			{
				if(*counter == Fsc::worker)
				{
					delete str;
					delete counter;
					delete publish;
				}
				return;
			}
			for(auto iter = it->second->begin(); iter != it->second->end(); ++iter)
			{
				list.push_back(*iter);
			}
			for(auto iter = list.begin(); iter != list.end(); ++iter)
			{
				StmpNet* an = *iter;
				an->sendUni(++an->tid, publish);
			}
			if(*counter == Fsc::worker)
			{
				delete str;
				delete counter;
				delete publish;
			}
		});
	}
}

/* 注册STMP-N2H上的RPC. */
bool Fsc::regStmpN2HMsg(const Descriptor* begin, const Descriptor* end, const Descriptor* uni, void* cb, RpcType type, bool fusr, const char* doc)
{
	if (Fsc::stmpN2HMsgs.find(begin->name()) != Fsc::stmpN2HMsgs.end())
	{
		LOG_ERROR("duplicate STMP-N2H MSG, begin: %s, end: %s", begin->name().c_str(), end->name().c_str())
		return false;
	}
	LOG_DEBUG("register STMP-N2H MSG successfully, begin: %s, end: %s", begin->name().c_str(), end->name().c_str())
	Fsc::stmpN2HMsgs[begin->name()] = new StmpCb(begin, end, uni, cb, type, fusr, doc);
	return true;
}

/* N2H上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于N2H主动发起的事务. */
bool Fsc::regIRPC(const Descriptor* begin, const Descriptor* end, bool fusr)
{
	return Fsc::regStmpN2HMsg(begin, end, NULL, NULL, RpcType::RPC_TYPE_IRPC, fusr, NULL);
}

/* N2H上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于N2H被动接收的事务. */
bool Fsc::regPRPC(const Descriptor* begin, const Descriptor* end, void* cb, bool fusr, const char* doc)
{
	return Fsc::regStmpN2HMsg(begin, end, NULL, cb, RpcType::RPC_TYPE_PRPC, fusr, doc);
}

/* N2H上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于N2H被动接收的事务(UNI). */
bool Fsc::regPRPC(const Descriptor* uni, void* cb, bool fusr, const char* doc)
{
	return Fsc::regStmpN2HMsg(NULL, NULL, uni, cb, RpcType::RPC_TYPE_PRPC, fusr, doc);
}

/* N2H上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于订阅其它网元提供的服务. */
bool Fsc::regSubcribe(const Descriptor* begin, const Descriptor* uni, bool fusr)
{
	return Fsc::regStmpN2HMsg(begin, NULL, uni, NULL, RpcType::RPC_TYPE_SUBSCIBE, fusr, NULL);
}

/* H2N上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于对外的SERVICE. */
bool Fsc::regService(const Descriptor* begin, const Descriptor* uni, void* cb, bool fusr, const char* doc)
{
	return Fsc::regStmpN2HMsg(begin, NULL, uni, cb, RpcType::RPC_TYPE_SERVICE, fusr, doc);
}

Fsc::~Fsc()
{

}

