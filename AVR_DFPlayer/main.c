/*
 * AVR_DFPlayer.c
 *
 * Created: 2018-03-28 오후 9:49:02
 * Author : kiki
 */ 

#ifndef F_CPU
#define F_CPU	16000000L
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "DFPlayer.h"
#include "328P_SWUART.h"

uint8_t rcvByte;
char msgBuffer[50];

int main(void)
{
	DFP_Initialize();
	
	/* To receive command via software usart */
	SWU_Initialize();
		
	/* Waiting for DFPlayer module initialization */
	_delay_ms(2000);
	
	/* Set Volume */
	DFP_SetVolume(10);
	
	/* Set EQ */
	DFP_SetEQ(DFP_EQ_ROCK);
	
	/* Play first track in root directory */
	DFP_PlayTrackInRoot(1);			
	
	/* Query current status */
	DFP_QueryCurrentStorage();
	DFP_QueryCurrentStatus();
	DFP_QueryCurrentEQ();
	DFP_QueryCurrentTrack();
	DFP_QueryCurrentVolume();
	DFP_QueryTracksInRoot();
	DFP_QueryCurrentTrack();
				
    /* Replace with your application code */
    while (1) 
    {					
		DFP_HandleMessage();			
		
		if(sRxDone)
		{
			SWU_RxByte(&rcvByte);
			memset(msgBuffer, 0, sizeof(msgBuffer));
			
			switch(rcvByte)
			{
				case 'N':	/* Play next */
				DFP_PlayNext();		
				sprintf(msgBuffer, "%s", "Play the next track.\r\n");		
				break;
				case 'P':	/* Play previous */
				DFP_PlayPrevious();
				sprintf(msgBuffer, "%s", "Play the previous track.\r\n");
				break;
				case '+':	/* Increase volume */
				DFP_IncreaseVolume();
				sprintf(msgBuffer, "%s", "Increase the volume.\r\n");
				break;
				case '-':	/* Decrease volume */
				DFP_DecreaseVolume();
				sprintf(msgBuffer, "%s", "Decrease the volume.\r\n");
				break;		
				case 'R':	/* Repeat current track on */
				DFP_RepeatCurrentTrack(true);
				sprintf(msgBuffer, "%s", "Repeat the current track.\r\n");
				break;
				case 'r':	/* Repeat current track off */
				DFP_RepeatCurrentTrack(false);
				sprintf(msgBuffer, "%s", "Stop repeating the current track.\r\n");
				break;
				case 'p':	/* Play */
				DFP_Play();
				sprintf(msgBuffer, "%s", "Play the current track.\r\n");
				break;
				case 'a':	/* Pause */
				DFP_Pause();
				sprintf(msgBuffer, "%s", "Pause the current track.\r\n");
				break;
				case 's':	/* Stop */
				DFP_Stop();
				sprintf(msgBuffer, "%s", "Stop the current track.\r\n");
				break;
				default:
				break;				
			}
			
			SWU_TxBuffer((uint8_t*)msgBuffer, strlen(msgBuffer));			
		}		
				
		/* If track is finished, move to next track */
		if(DFPlayerInfo.FinishedTrack != 0)
		{
			uint16_t temp = DFPlayerInfo.FinishedTrack;
			DFPlayerInfo.FinishedTrack = 0;
			
			DFP_PlayTrackInRoot(temp+1);
		}
    }
}

