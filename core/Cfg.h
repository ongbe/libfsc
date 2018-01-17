/*
 * Cfg.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef CORE_CFG_H_
#define CORE_CFG_H_

#include "Cfg.h"
#include "Fsc.h"

class Cfg
{
public:
	static string libfsc_server_addr;
	static int libfsc_worker;
	static FscProtocolType libfsc_protocol;
	static int libfsc_peer_limit;
	static int libfsc_peer_mtu;
	static int libfsc_peer_rcvbuf;
	static int libfsc_peer_sndbuf;
	static ullong libfsc_peer_heartbeat;
	static ullong libfsc_n2h_zombie;
	static ullong libfsc_h2n_reconn;
	static bool libfsc_ssc_enable;
	static string libfsc_log_level;
public:
	static void init();
public:
	Cfg();
	virtual ~Cfg();
};

#endif /* CORE_CFG_H_ */
