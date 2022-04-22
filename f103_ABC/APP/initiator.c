#include "config.h"
#include "usart.h"

/* mode */
//#define Normal
//#define Cycle
#define MultiPoint

uint8_t init_tx_msg[MSG_LEN] = {0};
uint8_t init_rx_msg[MSG_LEN] = {0};

#ifndef MultiPoint
/* T1_ts */
uint64_t T1_ts = 0;
uint64_t last_T1_ts = 0;
uint64_t tmp_T1_ts = 0;

double T1_s = 0;
double last_T1_s = 0;
double line_T1_s = 0;
double tmp_T1_s = 0;
double last_line_T1_s = 0;

/* T2 */
uint64_t T2_ts = 0;
uint64_t last_T2_ts = 0;
uint64_t tmp_T2_ts = 0;

double T2_s = 0;
double last_T2_s = 0;
double line_T2_s = 0;
double tmp_T2_s = 0;
double last_line_T2_s = 0;

/* T3 */
uint64_t T3_ts = 0;
uint64_t last_T3_ts = 0;
uint64_t tmp_T3_ts = 0;

double T3_s = 0;
double last_T3_s = 0;
double line_T3_s = 0;
double tmp_T3_s = 0;
double last_line_T3_s = 0;

/* T4 */
uint64_t T4_ts = 0;
uint64_t last_T4_ts = 0;
uint64_t tmp_T4_ts = 0;

double T4_s = 0;
double last_T4_s = 0;
double line_T4_s = 0;
double tmp_T4_s = 0;
double last_line_T4_s = 0;
#else
/* T1_ts */
uint64_t T1_ts[Multi_Num] = {0};
uint64_t last_T1_ts[Multi_Num] = {0};
uint64_t tmp_T1_ts[Multi_Num] = {0};

double T1_s[Multi_Num] = {0};
double last_T1_s[Multi_Num] = {0};
double line_T1_s[Multi_Num] = {0};
double tmp_T1_s[Multi_Num] = {0};
double last_line_T1_s[Multi_Num] = {0};

/* T2 */
uint64_t T2_ts[Multi_Num] = {0};
uint64_t last_T2_ts[Multi_Num] = {0};
uint64_t tmp_T2_ts[Multi_Num] = {0};

double T2_s[Multi_Num] = {0};
double last_T2_s[Multi_Num] = {0};
double line_T2_s[Multi_Num] = {0};
double tmp_T2_s[Multi_Num] = {0};
double last_line_T2_s[Multi_Num] = {0};

/* T3 */
uint64_t T3_ts[Multi_Num] = {0};
uint64_t last_T3_ts[Multi_Num] = {0};
uint64_t tmp_T3_ts[Multi_Num] = {0};

double T3_s[Multi_Num] = {0};
double last_T3_s[Multi_Num] = {0};
double line_T3_s[Multi_Num] = {0};
double tmp_T3_s[Multi_Num] = {0};
double last_line_T3_s[Multi_Num] = {0};

/* T4 */
uint64_t T4_ts[Multi_Num] = {0};
uint64_t last_T4_ts[Multi_Num] = {0};
uint64_t tmp_T4_ts[Multi_Num] = {0};

double T4_s[Multi_Num] = {0};
double last_T4_s[Multi_Num] = {0};
double line_T4_s[Multi_Num] = {0};
double tmp_T4_s[Multi_Num] = {0};
double last_line_T4_s[Multi_Num] = {0};
#endif


typedef enum init_state_e
{
	poll,
	wait,
	resp,
	cycle,
	multi
}init_state;


void initiator_init(void)
{
    reset_DW1000(); 
    set_dw1000_slowrate();
    if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
    {
        OLED_ShowString(0,0,(uint8_t*)"initiator failed");
        while (1)
        { };
    }
	else
	{
		uint8_t Str[20];
		uint32_t devID = dwt_readdevid();
		sprintf((char*)Str, "devID = %X %X", devID, MASTER_ID);
		OLED_ShowString(0,0,Str);
	}
    set_dw1000_fastrate();

    dwt_configure(&config);

    dwt_setrxantennadelay(RX_ANT_DLY);
    dwt_settxantennadelay(TX_ANT_DLY);

	OLED_ShowString(0,1,(uint8*)"initiator succeed");
    
    init_tx_msg[MSG_INIT_ID_IDX] = MASTER_ID;
}

#ifndef MultiPoint
void timestamp_process(void)
{
	/* data process */
	T1_s = T1_ts * K_timestamp;
	T2_s = T2_ts * K_timestamp;
	T3_s = T3_ts * K_timestamp;
	T4_s = T4_ts * K_timestamp;
	
	
	/* T1_ts */
	float deta = last_T1_s - T1_s;
	if(deta > 0)
		tmp_T1_s += overflow_s;
	line_T1_s = tmp_T1_s + T1_s;
	/* T2 */
	deta = last_T2_s - T2_s;
	if(deta > 0)
		tmp_T2_s += overflow_s;
	line_T2_s = tmp_T2_s + T2_s;
	/* T3 */
	deta = last_T3_s - T3_s;
	if(deta > 0)
		tmp_T3_s += overflow_s;
	line_T3_s = tmp_T3_s + T3_s;
	/* T4 */
	deta = last_T4_s - T4_s;
	if(deta > 0)
		tmp_T4_s += overflow_s;
	line_T4_s = tmp_T4_s + T4_s;
	
	printf("%X:%.15lf %.15lf %.15lf %.15lf\n",init_rx_msg[MSG_INIT_ID_IDX],
		last_T1_s,last_T2_s,T3_s,last_T4_s);
	/* last data update */
	last_T1_ts = T1_ts;
	last_T1_s = T1_s;
	last_line_T1_s = line_T1_s;
	
	last_T2_ts = T2_ts;
	last_T2_s = T2_s;
	last_line_T2_s = line_T2_s;	
	
	last_T3_ts = T3_ts;
	last_T3_s = T3_s;
	last_line_T3_s = line_T3_s;
	
	last_T4_ts = T4_ts;
	last_T4_s = T4_s;
	last_line_T4_s = line_T4_s;		
}
#else 
void timestamp_process(void)
{
	uint8_t index = init_rx_msg[MSG_INIT_ID_IDX] - devA_ID;
	/* data process */
	T1_s[index] = T1_ts[index] * K_timestamp;
	T2_s[index] = T2_ts[index] * K_timestamp;
	T3_s[index] = T3_ts[index] * K_timestamp;
	T4_s[index] = T4_ts[index] * K_timestamp;
	
	/* T1_ts */
	float deta = last_T1_s[index] - T1_s[index];
	if(deta > 0)
		tmp_T1_s[index] += overflow_s;
	line_T1_s[index] = tmp_T1_s[index] + T1_s[index];
	
	/* T2 */
	deta = last_T2_s[index] - T2_s[index];
	if(deta > 0)
		tmp_T2_s[index] += overflow_s;
	line_T2_s[index] = tmp_T2_s[index] + T2_s[index];
	
	/* T3 */
	deta = last_T3_s[index] - T3_s[index];
	if(deta > 0)
		tmp_T3_s[index] += overflow_s;
	line_T3_s[index] = tmp_T3_s[index] + T3_s[index];

	/* T4 */
	deta = last_T4_s[index] - T4_s[index];
	if(deta > 0)
		tmp_T4_s[index] += overflow_s;
	line_T4_s[index] = tmp_T4_s[index] + T4_s[index];
	
	printf("%X:%.15lf %.15lf %.15lf %.15lf\n",init_rx_msg[MSG_INIT_ID_IDX],
		last_line_T1_s[index],last_line_T2_s[index],
		line_T3_s[index],last_line_T4_s[index]);
	/* last data update */
	last_T1_ts[index] = T1_ts[index];
	last_T1_s[index] = T1_s[index];
	last_line_T1_s[index] = line_T1_s[index];
	
	last_T2_ts[index] = T2_ts[index];
	last_T2_s[index] = T2_s[index];
	last_line_T2_s[index] = line_T2_s[index];	
	
	last_T3_ts[index] = T3_ts[index];
	last_T3_s[index] = T3_s[index];
	last_line_T3_s[index] = line_T3_s[index];
	
	last_T4_ts[index] = T4_ts[index];
	last_T4_s[index] = T4_s[index];
	last_line_T4_s[index] = line_T4_s[index];		
}
#endif

#ifdef Normal
void initiator_running(void)
{
	static init_state initiator = poll;
    static uint32_t frame_seq_nb = 0;
	static uint32_t status_reg = 0;
	while(1)
	{
		switch(initiator)
		{
			case poll:
			{
				init_tx_msg[MSG_FRAME_IDX] = frame_seq_nb;
				dwt_writetxdata(sizeof(init_tx_msg),init_tx_msg,0);
				dwt_writetxfctrl(sizeof(init_tx_msg), 0, 1);
				/* tx immediate and rx expect */
				dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
				frame_seq_nb++;
				initiator = wait;
			}
			break;
			
			case wait:
			{
				if(((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) 
								& (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
				{
					if(status_reg & SYS_STATUS_RXFCG)
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);
						initiator = resp;
					}
					else
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
						initiator = poll;
					}
				}
			}
			break;
			
			case resp:
			{
				uint32_t frame_len = 0;
				dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
	
				frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
				dwt_readrxdata(init_rx_msg, frame_len, 0);

				initiator = poll;
			}
			break;
			
			case cycle:break;
		}
	}
}
#endif

#ifdef Cycle
void initiator_running(void)
{
	static init_state initiator = poll;
    static uint32_t frame_seq_nb = 0;
	static uint32_t status_reg = 0;
	while(1)
	{
		switch(initiator)
		{
			case poll:
			{
				init_tx_msg[MSG_FRAME_IDX] = frame_seq_nb;
				dwt_writetxdata(sizeof(init_tx_msg),init_tx_msg,0);
				dwt_writetxfctrl(sizeof(init_tx_msg), 0, 1);
				/* tx immediate firstly*/
				dwt_starttx(DWT_START_TX_IMMEDIATE);
				frame_seq_nb++;
				initiator = cycle;
			}
			break;
			
			case wait:
			{
				if(((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) 
								& (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
				{
					if(status_reg & SYS_STATUS_RXFCG)
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);
						initiator = cycle;
					}
					else
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
						initiator = poll;
					}
				}
			}
			break;
			
			case resp:break;
			
			case cycle:
			{
				/* RX */
				uint32_t frame_len = 0;
				frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
				dwt_readrxdata(init_rx_msg, frame_len, 0);
				msg_get_ts(&init_rx_msg[MSG_RX_TS_IDX],&T2_ts);
				msg_get_ts(&init_rx_msg[MSG_TX_TS_IDX],&T3_ts);
				/* TX */
				init_tx_msg[MSG_FRAME_IDX] = frame_seq_nb;
				dwt_writetxdata(sizeof(init_tx_msg),init_tx_msg,0);
				dwt_writetxfctrl(sizeof(init_tx_msg), 0, 1);
				T1_ts = get_tx_timestamp_u64();
				T4_ts = get_rx_timestamp_u64();
				dwt_write40bitreg(DX_TIME_ID, T1_ts + Synt);
				timestamp_process();
				/* tx enable */
				dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
				frame_seq_nb++;
				initiator = wait;
			}
			break;
		}
	}
}

#endif

#ifdef MultiPoint
void initiator_running(void)
{
	static init_state initiator = poll;
    static uint32_t frame_seq_nb = 0;
	static uint32_t status_reg = 0;
	while(1)
	{
		switch(initiator)
		{
			case poll:
			{
				init_tx_msg[MSG_FRAME_IDX] = ++frame_seq_nb;
				dwt_writetxdata(sizeof(init_tx_msg),init_tx_msg,0);
				dwt_writetxfctrl(sizeof(init_tx_msg), 0, 1);
				dwt_starttx(DWT_START_TX_IMMEDIATE);
				initiator = cycle;
			}
			break;
			
			case wait:
			{
				if(((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) 
								& (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
				{
					if(status_reg & SYS_STATUS_RXFCG)
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);
						initiator = resp;
					}
					else
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
					}
				}
			}
			break;
			
			case resp:
			{
				uint32_t frame_len = 0;
				dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
				
				frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
				dwt_readrxdata(init_rx_msg, frame_len, 0);
				
				switch(init_rx_msg[MSG_INIT_ID_IDX])
				{	
					case devA_ID:
					{
						T1_ts[0] = get_tx_timestamp_u64();
						msg_get_ts(&init_rx_msg[MSG_RX_TS_IDX],&T2_ts[0]);
						msg_get_ts(&init_rx_msg[MSG_TX_TS_IDX],&T3_ts[0]);
						T4_ts[0] = get_rx_timestamp_u64();
						initiator = wait;
						dwt_setrxtimeout(0);
						dwt_rxenable(0);
					}
					break;
					
					case devB_ID:
					{
						T1_ts[1] = get_tx_timestamp_u64();
						msg_get_ts(&init_rx_msg[MSG_RX_TS_IDX],&T2_ts[1]);
						msg_get_ts(&init_rx_msg[MSG_TX_TS_IDX],&T3_ts[1]);
						T4_ts[1] = get_rx_timestamp_u64();
						initiator = wait;
						dwt_setrxtimeout(0);
						dwt_rxenable(0);
					}
					break;	
					
					case devC_ID:
					{
						T1_ts[2] = get_tx_timestamp_u64();
						msg_get_ts(&init_rx_msg[MSG_RX_TS_IDX],&T2_ts[2]);
						msg_get_ts(&init_rx_msg[MSG_TX_TS_IDX],&T3_ts[2]);
						T4_ts[2] = get_rx_timestamp_u64();
						initiator = cycle;
					}
					break;
				}
				timestamp_process();
			}
			break;
			
			case cycle:
			{
				uint64_t poll_ts = get_tx_timestamp_u64();
				dwt_write40bitreg(DX_TIME_ID, poll_ts + Synt_Multi);
				dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
				initiator = wait;
			}
			break;
			case multi:break;
		}
	}
}

#endif
