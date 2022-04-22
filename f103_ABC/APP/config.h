#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <string.h>

#include "deca_device_api.h"
#include "deca_regs.h"
#include "deca_base.h"
#include "oled.h"

#define Multi_Num 3

#define MASTER_ID 0xFF
#define devA_ID 0x0A
#define devB_ID 0x0B
#define devC_ID 0x0C

static dwt_config_t config = {
    2,               /* Channel number. */
    DWT_PRF_64M,     /* Pulse repetition frequency. */
    DWT_PLEN_1024,   /* Preamble length. Used in TX only. */
    DWT_PAC32,       /* Preamble acquisition chunk size. Used in RX only. */
    9,               /* TX preamble code. Used in TX only. */
    9,               /* RX preamble code. Used in RX only. */
    1,               /* 0 to use standard SFD, 1 to use non-standard SFD. */
    DWT_BR_110K,     /* Data rate. */
    DWT_PHRMODE_STD, /* PHY header mode. */
    (1025 + 64 - 32) /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

#define TX_ANT_DLY 0
#define RX_ANT_DLY 32975


#define MSG_LEN 16

#define MSG_INIT_ID_IDX 0
#define MSG_RESP_ID_IDX 1
#define MSG_FRAME_IDX 2
#define MSG_TX_TS_IDX 4
#define MSG_RX_TS_IDX 9

#define K_timestamp (1/499.2e6/128)
#define Synt 0.02*499.2e6*128  
#define Synt_Multi 0.15*499.2e6*128  
#define Reg40Val 0xffffffffff
#define RealDetaVal  1.565325646879501e-11
#define overflow_s ((Reg40Val * K_timestamp) + RealDetaVal)

#define AnchorDelay 0.0005*499.2e6*128  


void initiator_init(void);
void responder_init(void);
void initiator_running(void);
void responder_running(void);

#endif
