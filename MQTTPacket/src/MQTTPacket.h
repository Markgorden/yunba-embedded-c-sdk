/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Xiang Rong - 442039 Add makefile to Embedded C client
 *******************************************************************************/

#ifndef MQTTPACKET_H_
#define MQTTPACKET_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#if defined(WIN32_DLL) || defined(WIN64_DLL)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#elif defined(LINUX_SO)
  #define DLLImport extern
  #define DLLExport  __attribute__ ((visibility ("default")))
#else
  #define DLLImport
  #define DLLExport  
#endif

//typedef unsigned int            uint32_t;
//typedef unsigned long long int  uint64_t;
#include <stdint.h>

enum errors
{
	MQTTPACKET_BUFFER_TOO_SHORT = -2,
	MQTTPACKET_READ_ERROR = -1,
	MQTTPACKET_READ_COMPLETE
};

enum msgTypes
{
	CONNECT = 1, CONNACK, PUBLISH, PUBACK, PUBREC, PUBREL,
	PUBCOMP, SUBSCRIBE, SUBACK, UNSUBSCRIBE, UNSUBACK,
	PINGREQ, PINGRESP, DISCONNECT, EXTCMD
};

typedef enum {
	GET_ALIAS =1,
	GET_ALIAS_ACK,
	GET_TOPIC,
	GET_TOPIC_ACK,
	GET_ALIAS_LIST,
	GET_ALIAS_LIST_ACK,
	PUBLISH2,
	PUBLISH2_ACK,
	GET_STATUS = 9,
	GET_STATUS_ACK,
	GET_TOPIC_LIST2 = 13,
	GET_TOPIC_LIST2_ACK,
	GET_ALIASLIST2,
	GET_ALIASLIST2_ACK,
	GET_STATUS2 = 19,
	GET_STATUS2_ACK = 20
} EXTED_CMD;

enum {
	PUBLISH2_TLV_TOPIC,
	PUBLISH2_TLV_PAYLOAD,
	PUBLISH2_TLV_PLAT,
	PUBLISH2_TLV_TTL,
	PUBLISH2_TLV_TIME_DELAY,
	PUBLISH2_TLV_LOCATION,
	PUBLISH2_TLV_QOS,
	PUBLISH2_TLV_APN_JSON,
	PUBLISH2_TLV_MAX_NUM
};

/**
 * Bitfields for the MQTT header byte.
 */
typedef union
{
	unsigned char byte;	                /**< the whole byte */
#if defined(REVERSED)
	struct
	{
		unsigned int type : 4;			/**< message type nibble */
		unsigned int dup : 1;				/**< DUP flag bit */
		unsigned int qos : 2;				/**< QoS value, 0, 1 or 2 */
		unsigned int retain : 1;		/**< retained flag bit */
	} bits;
#else
	struct
	{
		unsigned int retain : 1;		/**< retained flag bit */
		unsigned int qos : 2;				/**< QoS value, 0, 1 or 2 */
		unsigned int dup : 1;				/**< DUP flag bit */
		unsigned int type : 4;			/**< message type nibble */
	} bits;
#endif
} MQTTHeader;

typedef struct
{
	int len;
	char* data;
} MQTTLenString;

typedef struct
{
	char* cstring;
	MQTTLenString lenstring;
} MQTTString;

#define MQTTString_initializer {NULL, {0, NULL}}

int MQTTstrlen(MQTTString mqttstring);

#include "MQTTConnect.h"
#include "MQTTPublish.h"
#include "MQTTExtendedCmd.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"
#include "MQTTFormat.h"

int MQTTSerialize_ack(unsigned char* buf, int buflen, unsigned char type, unsigned char dup, uint64_t packetid);
int MQTTDeserialize_ack(unsigned char* packettype, unsigned char* dup, uint64_t* packetid, unsigned char* buf, int buflen);

int MQTTPacket_len(int rem_len);
int MQTTPacket_equals(MQTTString* a, char* b);

int MQTTPacket_encode(unsigned char* buf, int length);
int MQTTPacket_decode(int (*getcharfn)(unsigned char*, int), int* value);
int MQTTPacket_decodeBuf(unsigned char* buf, int* value);

int readInt(unsigned char** pptr);
uint64_t readInt64(char** pptr);
char readChar(unsigned char** pptr);
void writeChar(unsigned char** pptr, char c);
void writeInt64(char** pptr, uint64_t anInt);
void writeInt(unsigned char** pptr, int anInt);
int readMQTTLenString(MQTTString* mqttstring, unsigned char** pptr, unsigned char* enddata);
void writeCString(unsigned char** pptr, const char* string);
void writeMQTTString(unsigned char** pptr, MQTTString mqttstring);

DLLExport int MQTTPacket_read(unsigned char* buf, int buflen, int (*getfn)(unsigned char*, int));

typedef struct {
	int (*getfn)(void *, unsigned char*, int); /* must return -1 for error, 0 for call again, or the number of bytes read */
	void *sck;	/* pointer to whatever the system may use to identify the transport */
	int multiplier;
	int rem_len;
	int len;
	char state;
}MQTTTransport;

int MQTTPacket_readnb(unsigned char* buf, int buflen, MQTTTransport *trp);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif


#endif /* MQTTPACKET_H_ */
