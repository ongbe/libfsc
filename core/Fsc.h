/*
 * Fsc.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef FSC_H_
#define FSC_H_

#if !defined (__LIBFSCH__) && !defined (LIBFSC)
#error only libfsc.h can be included directly.
#endif

#include "FscLog.h"
#include "Fworker.h"
#include "../fscinc.h"
#include "../stmp/StmpCb.h"

#define STMP_IRPC_NOUSR(__BEGIN__, __END__)  									    (Fsc::regIRPC(__BEGIN__::descriptor(), __END__::descriptor(), false));
#define STMP_IPRC_ONUSR(__BEGIN__, __END__)    									    (Fsc::regIRPC(__BEGIN__::descriptor(), __END__::descriptor(),  true));
//
#define STMP_PRPC_NOUSR(__BEGIN__, __END__, __CB__)  							    (Fsc::regPRPC(__BEGIN__::descriptor(), __END__::descriptor(), (void*)(__CB__), false, NULL));
#define STMP_PRPC_ONUSR(__BEGIN__, __END__, __CB__)  							    (Fsc::regPRPC(__BEGIN__::descriptor(), __END__::descriptor(), (void*)(__CB__), true, NULL));
//
#define STMP_PRPC_UNI_NOUSR(__UNI__, __CB__)  									    (Fsc::regPRPC(__UNI__::descriptor(), (void*)(__CB__), false, NULL));
#define STMP_PRPC_UNI_ONUSR(__UNI__, __CB__)  									    (Fsc::regPRPC(__UNI__::descriptor(), (void*)(__CB__), true, NULL));
//
#define STMP_SUBSCRIBE_NOUSR(__BEGIN__, __UNI__)  								    (Fsc::regIRPC(__BEGIN__::descriptor(), __UNI__::descriptor(), false));
#define STMP_SUBSCRIBE_ONUSR(__BEGIN__, __UNI__)  								    (Fsc::regIRPC(__BEGIN__::descriptor(), __UNI__::descriptor(),  true));
//
#define STMP_SERVICE_NOUSR(__BEGIN__, __UNI__, __CB__)  						    (Fsc::regService(__BEGIN__::descriptor(), __UNI__::descriptor(), (void*)(__CB__), false, NULL));
#define STMP_SERVICE_ONUSR(__BEGIN__, __UNI__, __CB__)  						    (Fsc::regService(__BEGIN__::descriptor(), __UNI__::descriptor(), (void*)(__CB__),  true, NULL));

#define LIBFSC_PROTOCOL																"LIBFSC_PROTOCOL"			/* libfsc支持的协议. */
#define LIBFSC_SERVER_ADDR															"LIBFSC_SERVER_ADDR" 		/* 服务地址. */
#define LIBFSC_WORKER																"LIBFSC_WORKER" 			/* 消息总线线程数. */
#define LIBFSC_PEER_LIMIT															"LIBFSC_PEER_LIMIT" 		/* 允许的最大连接数. */
#define LIBFSC_PEER_MTU																"LIBFSC_PEER_MTU" 			/* 上行报文尺寸限制. */
#define LIBFSC_PEER_RCVBUF															"LIBFSC_PEER_RCVBUF" 		/* 连接上的接收缓冲区尺寸. */
#define LIBFSC_PEER_SNDBUF															"LIBFSC_PEER_SNDBUF" 		/* 连接上的发送缓冲区尺寸. */
#define LIBFSC_PEER_HEARTBEAT														"LIBFSC_PEER_HEARTBEAT" 	/* 连接上的心跳间隔(秒), 同时用于H2N的心跳发送和N2H的心跳检测. */
#define LIBFSC_N2H_ZOMBIE															"LIBFSC_N2H_ZOMBIE" 		/* 当一个N2H连接上来后, 等待其第一个消息的时间(秒), 超过此时间连接将被强制断开. */
#define LIBFSC_H2N_RECONN															"LIBFSC_H2N_RECONN" 		/* H2N重连等待(秒). */
#define LIBFSC_H2N_TRANS_TIMEOUT													"LIBFSC_H2N_TRANS_TIMEOUT"	/* H2N网络事务超时时间(秒), 即发送请求后等待响应的时间. */
#define LIBFSC_SSC_ENABLE															"LIBFSC_SSC_ENABLE"			/* 使能SSC. */
#define LIBFSC_LOG_LEVEL															"LIBFSC_LOG_LEVEL"			/* libfsc的日志级别. */

enum FscProtocolType
{
	STMP = 0x00, WEBSOCKET
};

class Fsc
{
public:
	static FscLog* log;
	static Fworker* wks;
	static pthread_key_t pkey;
	static FscProtocolType proto;
	static bool sscEnable;
	static int sfd;
	static int worker;
	static int peer_mtu;
	static ullong now;
	static unordered_map<string, StmpCb*> stmpN2HMsgs;
public:
	static bool init(FscLog* log);
	static bool publish();
	static void hold();
	static Fworker* getFwk();
	static int getWk();
	static int hashWk(ullong id);
	static bool regIRPC(const Descriptor* begin, const Descriptor* end, bool fusr); /* N2H上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于N2H主动发起的事务. */
	static bool regPRPC(const Descriptor* begin, const Descriptor* end, void* cb, bool fusr, const char* doc); /* N2H上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于N2H被动接收的事务. */
	static bool regPRPC(const Descriptor* uni, void* cb, bool fusr, const char* doc); /* N2H上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于N2H被动接收的事务(UNI). */
	static bool regSubcribe(const Descriptor* begin, const Descriptor* uni, bool fusr); /* N2H上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于订阅其它网元提供的服务. */
	static bool regService(const Descriptor* begin, const Descriptor* uni, void* cb, bool fusr, const char* doc); /* H2N上的网络消息(基于STMP-PROTOBUF的RPC)注册, 用于对外的SERVICE. */
private:
	Fsc();
	virtual ~Fsc();
	static void* svc(void* arg);
private:
	static bool regStmpN2HMsg(const Descriptor* begin, const Descriptor* end, const Descriptor* uni, void* cb, RpcType type, bool fusr, const char* doc); /* 注册STMP-N2H上的RPC. */
};

#endif /* FSC_H_ */
