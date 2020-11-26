#ifndef DFROBOT_QUEUE_H_
#define DFROBOT_QUEUE_H_
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
struct sQueueData{/*uart ����*/
    struct sQueueData *next;
    uint8_t handle;
    uint8_t len;
    uint8_t data[];
};

struct sRecData{/*���յ���MCU�������ݵĶ���*/
    struct sRecData *next;
    uint16_t id;
    uint8_t len;
    uint8_t data[];
};

struct sUPLOADData{/*�����������豸���ݵĶ���*/
    struct sUPLOADData *next;
    uint8_t id;
    uint8_t len;
    uint8_t status;
    uint8_t data[];
};
extern uint32_t queuedata;
extern void cuappEnqueue(uint8_t *pbuf, uint16_t len, uint16_t conn_id);
extern void cRecEnqueue(uint8_t *pbuf, uint16_t len, uint16_t conn_id);
extern void cUPLOADEnqueue(uint8_t *pbuf, uint16_t len, uint16_t conn_id, uint8_t status);
extern struct sQueueData *cuappDequeue( void );
extern struct sRecData *cRecDequeue( void );
extern struct sUPLOADData *cUPLOADDequeue( void );
extern struct sQueueData *getQueueTail(void);
extern struct sQueueData *getQueueHead(void);
#endif
