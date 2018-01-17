/*
 * Cfg.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "Cfg.h"

string Cfg::libfsc_server_addr = "0.0.0.0:1224";
int Cfg::libfsc_worker = 4;
FscProtocolType Cfg::libfsc_protocol = FscProtocolType::STMP;
int Cfg::libfsc_peer_limit = 1024;
int Cfg::libfsc_peer_mtu = 0x10000;
int Cfg::libfsc_peer_rcvbuf = 0x10000;
int Cfg::libfsc_peer_sndbuf = 0x10000;
ullong Cfg::libfsc_peer_heartbeat = 15 * 1000;
ullong Cfg::libfsc_n2h_zombie = 10 * 1000;
ullong Cfg::libfsc_h2n_reconn = 3 * 1000;
bool Cfg::libfsc_ssc_enable = false;
string Cfg::libfsc_log_level = "INFO";

Cfg::Cfg()
{

}

void Cfg::init()
{
	Cfg::libfsc_server_addr = Misc::getSetEnvStr(LIBFSC_SERVER_ADDR, Cfg::libfsc_server_addr.c_str());
	Cfg::libfsc_worker = Misc::getSetEnvInt(LIBFSC_WORKER, Cfg::libfsc_worker);
	Cfg::libfsc_protocol = (FscProtocolType) Misc::getSetEnvInt(LIBFSC_PROTOCOL, Cfg::libfsc_protocol);
	Cfg::libfsc_peer_limit = Misc::getSetEnvInt(LIBFSC_PEER_LIMIT, Cfg::libfsc_peer_limit);
	Cfg::libfsc_peer_mtu = Misc::getSetEnvInt(LIBFSC_PEER_MTU, Cfg::libfsc_peer_mtu);
	Cfg::libfsc_peer_rcvbuf = Misc::getSetEnvInt(LIBFSC_PEER_RCVBUF, Cfg::libfsc_peer_rcvbuf);
	Cfg::libfsc_peer_sndbuf = Misc::getSetEnvInt(LIBFSC_PEER_SNDBUF, Cfg::libfsc_peer_sndbuf);
	Cfg::libfsc_peer_heartbeat = Misc::getSetEnvInt(LIBFSC_PEER_HEARTBEAT, Cfg::libfsc_peer_heartbeat);
	Cfg::libfsc_n2h_zombie = Misc::getSetEnvInt(LIBFSC_N2H_ZOMBIE, Cfg::libfsc_n2h_zombie);
	Cfg::libfsc_h2n_reconn = Misc::getSetEnvInt(LIBFSC_H2N_RECONN, Cfg::libfsc_h2n_reconn);
	Cfg::libfsc_ssc_enable = Misc::getSetEnvStr(LIBFSC_SSC_ENABLE, "false") == string("true");
	Cfg::libfsc_log_level = Misc::getSetEnvStr(LIBFSC_LOG_LEVEL, "INFO");
	//
	Log::setLevel(Cfg::libfsc_log_level.c_str());
	list<string> envs;
	Misc::getEnvs(&envs);
	LOG_INFO("\n----------------------------------------------------------------")
	for (auto& it : envs)
	{
		if (strstr(it.c_str(), "LIBFSC_"))
			LOG_INFO("%s", it.c_str())
	}
	LOG_INFO("\n----------------------------------------------------------------")
}

Cfg::~Cfg()
{

}

