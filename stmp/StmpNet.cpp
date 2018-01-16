/*
 * StmpNet.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: root
 */

#include "StmpNet.h"
#include "StmpSsc.h"
#include "../core/FscMisc.h"
#include "../core/Cfg.h"
#include "../core/Fsc.h"
#include "../core/FscStat.h"

StmpNet::StmpNet(ActorType type, int wk) :
		ActorNet(type, wk)
{

}

/* 套接字可读. */
bool StmpNet::evnRead()
{
	uchar* dat = this->rbuf;
	int len = this->dlen;
	while (len > 0)
	{
		/** -------------------------------- */
		/**                                  */
		/** PING/PONG. */
		/**                                  */
		/** -------------------------------- */
		if (dat[0] == STMP_TAG_TRANS_PING || dat[0] == STMP_TAG_TRANS_PONG)
		{
			stmp_node* root = new stmp_node();
			root->self.t = dat[0];
			root->self.l = 0;
			root->self.v = NULL;
			//
			LOG_RECORD("\n  <-- PEER: %s CFD: %d\n%s%s", this->peer.c_str(), this->cfd, Misc::printhex2str(dat, 1).c_str(), stmpdec_printnode2str(root).c_str())
			FscStat::inc(FscStatItem::LIBFSC_RCV_MSGS);
			this->lts = Fsc::now;
			if (!this->evnMsg(root))
			{
				stmpdec_free(root);
				return false;
			}
			stmpdec_free(root);
			dat += 1;
			len -= 1;
			continue;
		}
		/** -------------------------------- */
		/**                                  */
		/** 太短. */
		/**                                  */
		/** -------------------------------- */
		if (len < 3)
			break;
		if (dat[1] == 0xFF)
		{
			LOG_DEBUG("unsupported over 64K PDU.")
			return false;
		}
		int l = (dat[1] == 0xFE ? 3 : 1);
		if (len < 1 + l)
			break;
		int size;
		if (dat[1] < 0xFE)
			size = 1/* tag*/+ 1 /* len字段本身. */+ dat[1] /* val */;
		else /** 有两字节表示长度. */
		{
			ushort s;
			memcpy(&s, dat + 2, 2);
			s = ntohs(s); /** 取出长度. */
			size = 1 + l + s;
		}
		if (size > Fsc::peer_mtu)
		{
			LOG_DEBUG("packet format error, we will close this connection, peer: %s, size: %08X", this->peer.c_str(), size)
			return false;
		}
		if (len < size) /* 还未到齐. */
			break;
		stmp_node root;
		if (stmpdec_unpack(dat, size, &root) != 0)
		{
			LOG_DEBUG("STMP protocol error.")
			return false;
		}
		LOG_RECORD("\n  <-- PEER: %s CFD: %d\n%s%s", this->peer.c_str(), this->cfd, Misc::printhex2str(dat, size).c_str(), stmpdec_printnode2str(&root).c_str())
		FscStat::inc(FscStatItem::LIBFSC_RCV_MSGS);
		this->lts = Fsc::now;
		if (!this->evnMsg(&root))
		{
			stmpdec_free(&root);
			return false;
		}
		stmpdec_free(&root);
		//
		dat += size;
		len -= size;
	}
	//
	if (len != this->dlen)
	{
		for (int i = 0; i < len; ++i)
			this->rbuf[i] = dat[i];
		this->dlen = len;
	}
	return true;
}

/* STMP-CONTINUE. */
void StmpNet::sendContinue(uint dtid, Message* continu)
{
	string pb = continu->SerializeAsString();
	int seglen = Fsc::peer_mtu - STMP_PDU_RESERVED; /* 一段的最大长度. */
	if ((int) pb.length() <= seglen) /* 不足一个段. */
	{
		stmp_pdu* pdu = StmpNet::encodeContinue(dtid, &pb);
		uint len;
		uchar* dat = stmpenc_take(pdu, &len);
		this->send(dat, len);
		free(pdu->buff);
		free(pdu);
		return;
	}
	//
	int segs = pb.length() / seglen; /* 段数. */
	int remain = pb.length() % seglen; /* 剩余. */
	//
	stmp_pdu* pdu = StmpNet::encodeContinueWithPart(dtid, (uchar*) pb.data(), 0, seglen); /* 第一段. */
	uint len;
	uchar* dat = stmpenc_take(pdu, &len);
	this->send(dat, len);
	free(pdu->buff);
	free(pdu);
	//
	for (int i = 1; i < segs && this->est; ++i) /* 完整的段. */
	{
		stmp_pdu* pdu = StmpNet::encodePart(STMP_TAG_DTID, dtid, (uchar*) pb.data(), i * seglen, seglen, remain != 0 /* 有剩余. */? true : (i == segs - 1 ? /* 最后一段. */false : true /* 非最后一段. */));
		uint len;
		uchar* dat = stmpenc_take(pdu, &len);
		this->send(dat, len);
		free(pdu->buff);
		free(pdu);
	}
	if (remain > 0 && this->est) /* 有剩余段. */
	{
		stmp_pdu* pdu = StmpNet::encodePart(STMP_TAG_DTID, dtid, (uchar*) pb.data(), segs * seglen, remain, false);
		uint len;
		uchar* dat = stmpenc_take(pdu, &len);
		this->send(dat, len);
		free(pdu->buff);
		free(pdu);
	}
}
/* STMP-END. */
void StmpNet::sendEnd(uint dtid, ushort ret, Message* end)
{
	if (end == NULL)
	{
		stmp_pdu* pdu = StmpNet::encodeEnd(dtid, ret, NULL);
		uint len;
		uchar* dat = stmpenc_take(pdu, &len);
		this->send(dat, len);
		free(pdu->buff);
		free(pdu);
		return;
	}
	//
	string pb = end->SerializeAsString();
	int seglen = Fsc::peer_mtu - STMP_PDU_RESERVED;
	if ((int) pb.length() <= seglen)
	{
		stmp_pdu* pdu = StmpNet::encodeEnd(dtid, ret, &pb);
		uint len;
		uchar* dat = stmpenc_take(pdu, &len);
		this->send(dat, len);
		free(pdu->buff);
		free(pdu);
		return;
	}
	//
	int segs = pb.length() / seglen;
	int remain = pb.length() % seglen;
	//
	stmp_pdu* pdu = StmpNet::encodeEndWithPart(dtid, ret, (uchar*) pb.data(), 0, seglen);
	uint len;
	uchar* dat = stmpenc_take(pdu, &len);
	this->send(dat, len);
	free(pdu->buff);
	free(pdu);
	//
	for (int i = 1; i < segs && this->est; ++i)
	{
		stmp_pdu* pdu = StmpNet::encodePart(STMP_TAG_DTID, dtid, (uchar*) pb.data(), i * seglen, seglen, remain != 0 ? true : (i == segs - 1 ? false : true));
		uint len;
		uchar* dat = stmpenc_take(pdu, &len);
		this->send(dat, len);
		free(pdu->buff);
		free(pdu);
	}
	if (remain > 0 && this->est)
	{
		stmp_pdu* pdu = StmpNet::encodePart(STMP_TAG_DTID, dtid, (uchar*) pb.data(), segs * seglen, remain, false);
		uint len;
		uchar* dat = stmpenc_take(pdu, &len);
		this->send(dat, len);
		free(pdu->buff);
		free(pdu);
	}
}

void StmpNet::sendUni(uint stid, Message* end)
{

}

/** ---------------------------------------------------------------- */
/**                                                                  */
/** encode */
/**                                                                  */
/** ---------------------------------------------------------------- */
/* encode SWITCH-BEGIN. */
stmp_pdu* StmpNet::encodeSwitchBegin(string* sne, string* dne, uint stid, uchar* msg, uint msglen, uchar* dat, uint datlen)
{
	stmp_pdu* pdu = (stmp_pdu*) malloc(sizeof(stmp_pdu));
	int size = sne->length() + dne->length() + msglen + datlen + STMP_PDU_RESERVED;
	pdu->len = size;
	pdu->rm = size;
	pdu->p = 0;
	pdu->buff = (uchar*) malloc(size);
	stmpenc_add_bin(pdu, STMP_TAG_DAT, dat, datlen);
	stmpenc_add_bin(pdu, STMP_TAG_MSG, msg, msglen);
	stmpenc_add_bin(pdu, STMP_TAG_DNE, (uchar*) dne->data(), dne->length());
	stmpenc_add_bin(pdu, STMP_TAG_SNE, (uchar*) sne->data(), dne->length());
	stmpenc_add_int(pdu, STMP_TAG_STID, stid);
	stmpenc_add_tag(pdu, STMP_TAG_TRANS_BEGIN);
	stmpenc_add_tag(pdu, STMP_TAG_TRANS_SWITCH);
	return pdu;
}

/*  encode-STMP-CONTINUE. */
stmp_pdu* StmpNet::encodeContinue(uint dtid, string* pb)
{
	stmp_pdu* pdu = (stmp_pdu*) malloc(sizeof(stmp_pdu));
	int size = pb->length() + STMP_PDU_RESERVED;
	pdu->len = size;
	pdu->rm = size;
	pdu->p = 0;
	pdu->buff = (uchar*) malloc(size);
	stmpenc_add_bin(pdu, STMP_TAG_DAT, (uchar*) pb->data(), pb->length());
	stmpenc_add_int(pdu, STMP_TAG_DTID, dtid);
	stmpenc_add_tag(pdu, STMP_TAG_TRANS_CONTINUE);
	return pdu;
}

/* encode-STMP-CONTINUE witch PART. */
stmp_pdu* StmpNet::encodeContinueWithPart(uint dtid, uchar* dat, int ofst, int len)
{
	stmp_pdu* pdu = (stmp_pdu*) malloc(sizeof(stmp_pdu));
	int size = len + STMP_PDU_RESERVED;
	pdu->len = size;
	pdu->rm = size;
	pdu->p = 0;
	pdu->buff = (uchar*) malloc(size);
	stmpenc_add_bin(pdu, STMP_TAG_DAT, dat + ofst, len);
	stmpenc_add_char(pdu, STMP_TAG_HAVE_NEXT_PART, RET_PRESENT);
	stmpenc_add_int(pdu, STMP_TAG_DTID, dtid);
	stmpenc_add_tag(pdu, STMP_TAG_TRANS_CONTINUE);
	return pdu;
}

/* encode-STMP-END.*/
stmp_pdu* StmpNet::encodeEnd(uint dtid, ushort ret, string* pb)
{
	stmp_pdu* pdu = (stmp_pdu*) malloc(sizeof(stmp_pdu));
	int size = pb == NULL ? 0 : pb->length();
	size = size + STMP_PDU_RESERVED;
	pdu->len = size;
	pdu->rm = size;
	pdu->p = 0;
	pdu->buff = (uchar*) malloc(size);
	if (pb != NULL)
		stmpenc_add_bin(pdu, STMP_TAG_DAT, (uchar*) pb->data(), pb->length());
	if (ret != RET_SUCCESS) /* no news is good news. */
		stmpenc_add_short(pdu, STMP_TAG_RET, ret);
	stmpenc_add_int(pdu, STMP_TAG_DTID, dtid);
	stmpenc_add_tag(pdu, STMP_TAG_TRANS_END);
	return pdu;
}

/* encode-STMP-END with PART. */
stmp_pdu* StmpNet::encodeEndWithPart(uint dtid, ushort ret, uchar* dat, int ofst, int len)
{
	stmp_pdu* pdu = (stmp_pdu*) malloc(sizeof(stmp_pdu));
	int size = len + STMP_PDU_RESERVED;
	pdu->len = size;
	pdu->rm = size;
	pdu->p = 0;
	pdu->buff = (uchar*) malloc(size);
	stmpenc_add_bin(pdu, STMP_TAG_DAT, dat + ofst, len);
	stmpenc_add_char(pdu, STMP_TAG_HAVE_NEXT_PART, RET_PRESENT);
	stmpenc_add_short(pdu, STMP_TAG_RET, ret);
	stmpenc_add_int(pdu, STMP_TAG_DTID, dtid);
	stmpenc_add_tag(pdu, STMP_TAG_TRANS_END);
	return pdu;
}

/* encode-STMP-PART. */
stmp_pdu* StmpNet::encodePart(uchar tidTag, uint tid, uchar* dat, int ofst, int len, bool haveNext)
{
	stmp_pdu* pdu = (stmp_pdu*) malloc(sizeof(stmp_pdu));
	int size = len + STMP_PDU_RESERVED;
	pdu->len = size;
	pdu->rm = size;
	pdu->p = 0;
	pdu->buff = (uchar*) malloc(size);
	//
	stmpenc_add_bin(pdu, STMP_TAG_DAT, dat + ofst, len);
	if (haveNext)
		stmpenc_add_char(pdu, STMP_TAG_HAVE_NEXT_PART, RET_PRESENT);
	stmpenc_add_int(pdu, tidTag, tid);
	stmpenc_add_tag(pdu, STMP_TAG_TRANS_PART);
	return pdu;
}

/** ---------------------------------------------------------------- */
/**                                                                  */
/** decode. */
/**                                                                  */
/** ---------------------------------------------------------------- */
/* decode-STMP-BEGIN. */
bool StmpNet::decodeBegin(stmp_node* root, uint* tid, Message** begin, StmpCb** cb)
{
	if (stmpdec_get_int(root, STMP_TAG_STID, tid) != 0)
	{
		LOG_DEBUG("missing required field: STMP_TAG_STID")
		return false;
	}
	uint mlen;
	uchar* m = stmpdec_peek_bin(root, STMP_TAG_MSG, &mlen);
	if (m == NULL)
	{
		LOG_DEBUG("missing required field: STMP_TAG_MSG.")
		return false;
	}
	if (mlen > STMP_MSG_MAX_LEN) /* ä¸<8d>å<85><81>è®¸è¶<85>è¿<87>è¿<99>ä¸ªé<95>¿åº¦. */
	{
		LOG_DEBUG("message name too long, len: %d, name: %s", mlen, Misc::hex2str0(m, mlen).c_str())
		return false;
	}
	string msg((char*) m, mlen);
	auto it = Fsc::stmpN2HMsgs.find(msg);
	if (it == Fsc::stmpN2HMsgs.end()) /* ä¸<8d>æ<94>¯æ<8c><81>ç<9a><84>å<91>½ä»¤å­<97> . */
	{
		LOG_DEBUG("can not found call back for this msg: %s", msg.c_str())
		return false;
	}
	//
	uint datlen;
	uchar* dat = stmpdec_peek_bin(root, STMP_TAG_DAT, &datlen);
	if (dat == NULL)
	{
		LOG_DEBUG("missing required field: STMP_TAG_DAT.")
		return false;
	}
	*begin = FscMisc::newPbMsg(it->second->begin, dat, datlen);
	if (*begin == NULL)
	{
		LOG_DEBUG("can not parse %s from STMP-BEGIN", it->second->begin->name().c_str())
		return false;
	}
	*cb = it->second;
	return true;
}

/** ---------------------------------------------------------------- */
/**                                                                  */
/** SSC-SWITCH. */
/**                                                                  */
/** ---------------------------------------------------------------- */
/* SWITCH-BEGIN. */
bool StmpNet::switchBegin(stmp_node* root, string* dne)
{
	StmpNet* to = StmpSsc::get(dne);
	if (to == NULL)
	{
		LOG_DEBUG("can not found DNE: %s, this: %s", dne->c_str(), this->toString().c_str())
		return true;
	}
	/** -------------------------------- */
	/**                                  */
	/** å¿<85>è¦<81>æ<95>°æ<8d>®è<8e>·å<8f><96>. */
	/**                                  */
	/** -------------------------------- */
	uint stid;
	if (stmpdec_get_int(root, STMP_TAG_STID, &stid) != 0)
	{
		LOG_DEBUG("missing required field: STMP_TAG_STID, this: %s", this->toString().c_str())
		return false;
	}
	uint mlen;
	uchar* m = stmpdec_peek_bin(root, STMP_TAG_MSG, &mlen);
	if (m == NULL)
	{
		LOG_DEBUG("missing required field: STMP_TAG_MSG, this: %s", this->toString().c_str())
		return false;
	}
	if (mlen > STMP_MSG_MAX_LEN) /* ä¸<8d>å<85><81>è®¸è¶<85>è¿<87>è¿<99>ä¸ªé<95>¿åº¦. */
	{
		LOG_DEBUG("message name too long, len: %d, name: %s, this: %s", mlen, Misc::hex2str0(m, mlen).c_str(), this->toString().c_str())
		return false;
	}
	uint datlen;
	uchar* dat = stmpdec_peek_bin(root, STMP_TAG_DAT, &datlen);
	if (dat == NULL)
	{
		LOG_DEBUG("missing required field: STMP_TAG_DAT, this: %s", this->toString().c_str())
		return false;
	}
	/** -------------------------------- */
	/**                                  */
	/** encode and switch. */
	/**                                  */
	/** -------------------------------- */
	stmp_pdu* pdu = StmpNet::encodeSwitchBegin(&this->sne, dne, stid, m, mlen, dat, datlen);
	to->future([to, pdu]
	{
		uint len;
		uchar* buf = stmpenc_take(pdu, &len);
		to->send(buf, len);
		free(pdu->buff);
		free(pdu);
	});
	return true;
}

StmpNet::~StmpNet()
{
	// TODO Auto-generated destructor stub
}

