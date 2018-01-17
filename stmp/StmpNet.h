/*
 * StmpNet.h
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#ifndef STMP_STMPNET_H_
#define STMP_STMPNET_H_

#include "StmpCb.h"
#include "../actor/ActorNet.h"
#include "../fscinc.h"

#define STMP_NE_MAX_LEN                                                                                                         0x80
#define STMP_MSG_MAX_LEN                                                                                                        0x40

class StmpNet: public ActorNet
{
public:
	string sne;
public:
	bool evnRead(); /* 套接字可读. */
	void sendContinue(uint dtid, Message* continu); /* STMP-CONTINUE. */
	void sendEnd(uint dtid, ushort ret, Message* end, uint* sid /* 用于subscribe/publish. */); /* STMP-END. */
	void sendUni(uint stid, uint* sid /* 用于subscribe/publish. */, Message* end); /* STMP-UNI. */
public:
	static stmp_pdu* encodeSwitchBegin(string* sne, string* dne, uint stid, uchar* msg, uint msglen, uchar* dat, uint datlen); /* encode SWITCH-BEGIN. */
	static stmp_pdu* encodeContinue(uint dtid, string* pb); /* encode STMP-CONTINUE. */
	static stmp_pdu* encodeContinueWithPart(uint dtid, uchar* dat, int ofst, int len); /* encode-STMP-CONTINUE witch PART. */
	static stmp_pdu* encodeEnd(uint dtid, ushort ret, string* end, uint* sid /* 用于subscribe/publish. */); /*  encode-STMP-END. */
	static stmp_pdu* encodeEndWithPart(uint dtid, ushort ret, uint* sid /* 用于subscribe/publish. */, uchar* dat, int ofst, int len); /*  encode-STMP-END. */
	static stmp_pdu* encodePart(uchar tidTag, uint tid, uchar* dat, int ofst, int len, bool haveNext); /** encode-STMP-PART. */
	bool decodeBegin(stmp_node* root, uint* tid, Message** begin, StmpCb** cb); /* decode-STMP-BEGIN. */
public:
	static void publishMsg(string* subsribe, Message* publish); /* 发布订阅服务上的消息. */
public:
	StmpNet(ActorType type, int wk);
	virtual void evnDis() = 0;
	virtual bool evnMsg(stmp_node* root) = 0;
	virtual ~StmpNet();
public:
	bool switchBegin(stmp_node* root, string* dne); /* SWITCH-BEGIN. */
};

#endif /* STMP_STMPNET_H_ */
