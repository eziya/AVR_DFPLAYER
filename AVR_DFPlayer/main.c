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

#include "DFPlayer.h"

int main(void)
{
	DFP_Init();
	
	/* Waiting for DFPlayer module initialization */
	_delay_ms(2000);
	
	/* Set Volume */
	DFP_SetVolume(13);
	
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
				
		/* If track is finished, move to next track */
		if(DFPlayerInfo.FinishedTrack != 0)
		{
			uint16_t temp = DFPlayerInfo.FinishedTrack;
			DFPlayerInfo.FinishedTrack = 0;
			
			DFP_PlayTrackInRoot(temp+1);
		}
    }
}

