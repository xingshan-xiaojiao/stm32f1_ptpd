#include "config.h"
#include "usart.h"

/* device */
#define devA
//#define devB
//#define devC

/* mode */
//#define Normal
//#define Cycle
#define MultiPoint

uint8_t resp_tx_msg[MSG_LEN] = {0};
uint8_t resp_rx_msg[MSG_LEN] = {0};

uint64_t tx_T2_ts = 0;
uint64_t tx_T3_ts = 0;

typedef enum resp_state_e
{
	wait,
	resp,
	poll,
	cycle,
	multi
}resp_state;

void responder_init(void)
{
    reset_DW1000(); 
    set_dw1000_slowrate();
    if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
    {
        OLED_ShowString(0,0,(uint8_t*)"responder failed");
        while (1)
        { };
    }
	else
	{
		uint8_t Str[20];
		uint32_t devID = dwt_readdevid();
#ifdef devA
		sprintf((char*)Str, "devID = %X %X", devID, devA_ID);
		resp_tx_msg[MSG_INIT_ID_IDX] = devA_ID;
#endif
#ifdef devB
		sprintf((char*)Str, "devID = %X %X", devID, devB_ID);
		resp_tx_msg[MSG_INIT_ID_IDX] = devB_ID;
#endif
#ifdef devC
		sprintf((char*)Str, "devID = %X %X", devID, devC_ID);
		resp_tx_msg[MSG_INIT_ID_IDX] = devC_ID;
#endif
		OLED_ShowString(0,0,Str);
	}
    set_dw1000_fastrate();

    dwt_configure(&config);

    dwt_setrxantennadelay(RX_ANT_DLY);
    dwt_settxantennadelay(TX_ANT_DLY);

	OLED_ShowString(0,1,(uint8*)"responder succeed");
    
    resp_tx_msg[MSG_RESP_ID_IDX] = MASTER_ID;
	
	dwt_setrxtimeout(0);
	dwt_rxenable(0);
}
#ifdef Normal
void responder_running(void)
{
	static resp_state responder = wait;
	static uint32_t frame_seq_nb = 0;
    static uint32_t status_reg = 0;
			
	while(1)
	{	
		switch(responder)
		{
			case wait:
			{
				if(((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) 
								& (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
				{
					if(status_reg & SYS_STATUS_RXFCG)
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);
						responder = resp;
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
				dwt_readrxdata(resp_rx_msg, frame_len, 0);		
				responder = poll;
			}
			break;
			
			case poll:
			{
				tx_T2_ts = get_tx_timestamp_u64();
				tx_T3_ts = get_rx_timestamp_u64();
				
				resp_tx_msg[MSG_FRAME_IDX] = frame_seq_nb;
				msg_set_ts(&resp_tx_msg[MSG_TX_TS_IDX],tx_T2_ts);
				msg_set_ts(&resp_tx_msg[MSG_RX_TS_IDX],tx_T3_ts);
				dwt_writetxdata(sizeof(resp_tx_msg),resp_tx_msg,0);
				dwt_writetxfctrl(sizeof(resp_tx_msg), 0, 1);
				dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
				frame_seq_nb++;
				responder = wait;
			}
			break;
			
			case cycle:break;
		}
	}	
}
#endif

#ifdef Cycle
void responder_running(void)
{
	static resp_state responder = resp;
	static uint32_t frame_seq_nb = 0;
    static uint32_t status_reg = 0;
	static uint32_t resp2cnt = 0;
	while(1)
	{	
		switch(responder)
		{	
			case resp:
			{
				if(((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) 
								& (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
				{
					if(status_reg & SYS_STATUS_RXFCG)
					{
						dwt_write32bitreg(SYS_STATUS_ID,
											SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);
						if(resp2cnt < 2)
						{
							resp2cnt++;
						}
						responder = poll;
					}
					else
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
					}
				}				
			}
			break;
				
			case poll:
			{	
				uint32_t frame_len = 0;
				frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
				dwt_readrxdata(resp_rx_msg, frame_len, 0);
				
				resp_tx_msg[MSG_FRAME_IDX] = ++frame_seq_nb;
				tx_T2_ts = get_tx_timestamp_u64();				
				tx_T3_ts = get_rx_timestamp_u64();
				msg_set_ts(&resp_tx_msg[MSG_TX_TS_IDX],tx_T2_ts);
				msg_set_ts(&resp_tx_msg[MSG_RX_TS_IDX],tx_T3_ts);
				dwt_writetxdata(sizeof(resp_tx_msg),resp_tx_msg,0);
				dwt_writetxfctrl(sizeof(resp_tx_msg), 0, 1);
				if(resp2cnt < 2)
				{
					dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
					responder = resp;
				}
				else
				{
					dwt_starttx(DWT_START_TX_IMMEDIATE);
					responder = cycle;
				}
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
						responder = cycle;
					}
					else
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
					}
				}
			}
			break;
			
			case cycle:
			{
				uint32_t frame_len = 0;
				frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
				dwt_readrxdata(resp_rx_msg, frame_len, 0);
				
				resp_tx_msg[MSG_FRAME_IDX] = ++frame_seq_nb;
				tx_T2_ts = get_tx_timestamp_u64();
				tx_T3_ts = get_rx_timestamp_u64();
				msg_set_ts(&resp_tx_msg[MSG_TX_TS_IDX],tx_T2_ts);
				msg_set_ts(&resp_tx_msg[MSG_RX_TS_IDX],tx_T3_ts);
				dwt_writetxdata(sizeof(resp_tx_msg),resp_tx_msg,0);
				dwt_writetxfctrl(sizeof(resp_tx_msg), 0, 1);				
				dwt_write40bitreg(DX_TIME_ID, tx_T2_ts + Synt);
				dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
				responder = wait;
			}
			break;
		}
	}	
}
#endif

#ifdef MultiPoint
void responder_running(void)
{
	static resp_state responder = wait;
	static uint32_t frame_seq_nb = 0;
    static uint32_t status_reg = 0;
	static uint32_t resp2cnt[3] = {0};
	static uint64_t delay_ts = 0;
	
	while(1)
	{	
		switch(responder)
		{
			case wait:
			{
				if(((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) 
								& (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
				{
					if(status_reg & SYS_STATUS_RXFCG)
					{
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);
						responder = resp;
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
				dwt_readrxdata(resp_rx_msg, frame_len, 0);
				if(resp_rx_msg[MSG_INIT_ID_IDX] == MASTER_ID)
				{
					uint8_t index = resp_tx_msg[MSG_INIT_ID_IDX] - devA_ID;
					if(resp2cnt[index] < 2)
					{
						resp2cnt[index]++;
						responder = multi;
					}
					else
					{
						responder = cycle;
					}
				}
				else
				{
					responder = wait;
					dwt_setrxtimeout(0);
					dwt_rxenable(0);
				}
				
			}
			break;
			
			case multi:
			{
				delay_ts = get_rx_timestamp_u64();
				switch(resp_tx_msg[MSG_INIT_ID_IDX])
				{
					case devA_ID: delay_ts += AnchorDelay * 10; break;
					case devB_ID: delay_ts += AnchorDelay * 20; break;
					case devC_ID: delay_ts += AnchorDelay * 40; break;
				}						
				responder = poll;
			}
			break;
			
			case poll:
			{
				tx_T2_ts = get_rx_timestamp_u64();
				tx_T3_ts = get_tx_timestamp_u64();
				
				resp_tx_msg[MSG_FRAME_IDX] = ++frame_seq_nb;
				msg_set_ts(&resp_tx_msg[MSG_RX_TS_IDX], tx_T2_ts);
				msg_set_ts(&resp_tx_msg[MSG_TX_TS_IDX], tx_T3_ts);
				dwt_writetxdata(sizeof(resp_tx_msg), resp_tx_msg, 0);
				dwt_writetxfctrl(sizeof(resp_tx_msg), 0, 1);
				dwt_write40bitreg(DX_TIME_ID, delay_ts);
				dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
				responder = wait;
			}
			break;
			
			case cycle:
			{
				tx_T2_ts = get_rx_timestamp_u64();
				tx_T3_ts = get_tx_timestamp_u64();
				
				resp_tx_msg[MSG_FRAME_IDX] = ++frame_seq_nb;
				msg_set_ts(&resp_tx_msg[MSG_RX_TS_IDX], tx_T2_ts);
				msg_set_ts(&resp_tx_msg[MSG_TX_TS_IDX], tx_T3_ts);
				dwt_writetxdata(sizeof(resp_tx_msg), resp_tx_msg, 0);
				dwt_writetxfctrl(sizeof(resp_tx_msg), 0, 1);
				dwt_write40bitreg(DX_TIME_ID, tx_T3_ts + Synt_Multi);
				dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
				responder = wait;				
			}
			break;
		}
	}	
}
#endif
