/*
 * DFPlayer.c
 *
 * Created: 2018-03-28 오후 6:00:01
 *  Author: kiki
 */ 
#ifndef F_CPU
#define F_CPU	16000000L
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "DFPlayer.h"
#include "328P_USART.h"

_DFPlayerInfo DFPlayerInfo;
static uint8_t RxBuffer[DFP_MSG_SIZE];
static uint8_t TxBuffer[DFP_MSG_SIZE];

static void DFP_SendControlMessage(uint8_t cmd, uint8_t param_msb, uint8_t param_lsb);
static bool DFP_SendQueryMessage(uint8_t cmd, uint8_t param_msb, uint8_t param_lsb);
static bool DFP_ParseMessage();
static uint16_t DFP_CalChecksum();

void DFP_PlayNext()
{
	DFP_SendControlMessage(DFP_CMD_PLAY_NEXT, 0x00, 0x00);
}

void DFP_PlayPrevious()
{
	DFP_SendControlMessage(DFP_CMD_PLAY_PREVIOUS, 0x00, 0x00);
}

void DFP_PlayTrackInRoot(uint16_t track)
{
	DFP_SendControlMessage(DFP_CMD_PLAY_TRACK, (track & 0xFF00) >> 8, (track & 0xFF));
}

void DFP_IncreaseVolume()
{
	DFP_SendControlMessage(DFP_CMD_INC_VOLUME, 0x00, 0x00);
}

void DFP_DecreaseVolume()
{
	DFP_SendControlMessage(DFP_CMD_DEC_VOLUME, 0x00, 0x00);
}

void DFP_SetVolume(uint8_t volume)
{
	if(volume > 30) volume = 30;
	
	DFP_SendControlMessage(DFP_CMD_SET_VOLUME, 0x00, volume);
}

void DFP_SetEQ(uint8_t eq)
{
	if(eq > 5) eq = 0;
	
	DFP_SendControlMessage(DFP_CMD_SET_EQ, 0x00, eq);
}

void DFP_RepeatTrack(uint16_t track)
{	
	DFP_SendControlMessage(DFP_CMD_REPEAT_TRACK, (track & 0xFF00) >> 8, track & 0xFF);
}

void DFP_SetStorage(uint8_t storage)
{
	if(storage > 2) storage = 2;
	
	DFP_SendControlMessage(DFP_CMD_SET_STORAGE, 0x00, storage);
}

void DFP_SetSleep()
{
	DFP_SendControlMessage(DFP_CMD_SET_SLEEP, 0x00, 0x00);
}

void DFP_Reset()
{
	DFP_SendControlMessage(DFP_CMD_RESET, 0x00, 0x00);
}

void DFP_Play()
{
	DFP_SendControlMessage(DFP_CMD_PLAY, 0x00, 0x00);
}

void DFP_Pause()
{
	DFP_SendControlMessage(DFP_CMD_PAUSE, 0x00, 0x00);
}

void DFP_PlayTrackInFolder(uint8_t folder, uint8_t track)
{
	if(folder > 99) folder = 99;
		
	DFP_SendControlMessage(DFP_CMD_PLAY_TRACK_FOLDER, folder, track);
}

void DFP_SetAudioGain(bool onoff, uint8_t gain)
{
	if(gain > 31) gain = 31;
	
	DFP_SendControlMessage(DFP_CMD_SET_AMP, onoff, gain);
}

void DFP_RepeatAll(bool repeat)
{
	DFP_SendControlMessage(DFP_CMD_REPEAT_ALL, 0x00, repeat);
}

void DFP_PlayTrackInMp3(uint16_t track)
{
	DFP_SendControlMessage(DFP_CMD_PLAY_MP3_FOLDER, (track & 0xFF00) >> 8, track & 0xFF);
}

void DFP_InsertAdvertisement(uint16_t track)
{
	DFP_SendControlMessage(DFP_CMD_INSERT_ADVERT, (track & 0xFF00) >> 8, track & 0xFF);
}

void DFP_PlayTrackIn3KFolder(uint8_t folder, uint16_t track)
{
	if(folder > 15) folder = 15;
	if(track > 3000) track = 3000;
	
	DFP_SendControlMessage(DFP_CMD_PLAY_3K_FOLDER, (folder << 4) | (track & 0xF00) >> 8 , track & 0xFF);
}

void DFP_StopAdvertisement()
{
	DFP_SendControlMessage(DFP_CMD_STOP_ADVERT, 0x00, 0x00);
}

void DFP_Stop()
{
	DFP_SendControlMessage(DFP_CMD_STOP, 0x00, 0x00);
}

void DFP_RepeatTrackInFolder(uint8_t folder)
{
	if(folder > 99) folder = 99;
	
	DFP_SendControlMessage(DFP_CMD_REPEAT_FOLDER, 0x00, folder);
}

void DFP_PlayRandomTrack()
{
	DFP_SendControlMessage(DFP_CMD_PLAY_RANDOM, 0x00, 0x00);
}

void DFP_RepeatCurrentTrack(bool onoff)
{
	DFP_SendControlMessage(DFP_CMD_REPEAT_CUR_TRACK, 0x00, !onoff);
}

void DFP_SetDAC(bool onoff)
{
	DFP_SendControlMessage(DFP_CMD_SET_DAC, 0x00, !onoff);
}

bool DFP_QueryCurrentStorage()
{
	return DFP_SendQueryMessage(DFP_CMD_GET_CUR_STORAGE, 0x00, 0x00);
}

bool DFP_QueryCurrentStatus()
{
	return DFP_SendQueryMessage(DFP_CMD_GET_CUR_STATUS, 0x00, 0x00);
}

bool DFP_QueryCurrentVolume()
{
	return DFP_SendQueryMessage(DFP_CMD_GET_CUR_VOLUME, 0x00, 0x00);
}

bool DFP_QueryCurrentEQ()
{
	return DFP_SendQueryMessage(DFP_CMD_GET_CUR_EQ, 0x00, 0x00);
}

bool DFP_QueryTracksInRoot()
{
	if(DFPlayerInfo.CurrentStorageDevice == DFP_STORAGE_USB)
	{
		return DFP_SendQueryMessage(DFP_CMD_GET_NUM_USB_ROOT_TRACKS, 0x00, 0x00);		
	}
	else
	{
		return DFP_SendQueryMessage(DFP_CMD_GET_NUM_SD_ROOT_TRACKS, 0x00, 0x00);		
	}	
}

bool DFP_QueryCurrentTrack()
{
	if(DFPlayerInfo.CurrentStorageDevice == DFP_STORAGE_USB)
	{
		return DFP_SendQueryMessage(DFP_CMD_GET_CUR_USB_TRACK, 0x00, 0x00);
	}
	else
	{
		return DFP_SendQueryMessage(DFP_CMD_GET_CUR_SD_TRACK, 0x00, 0x00);
	}
}

bool DFP_QueryTrackInFolder(uint8_t folder)
{
	if(folder > 99) folder = 99;
	
	return DFP_SendQueryMessage(DFP_CMD_GET_NUM_FOLDER_TRACKS, 0x00, 0x00);	
}

bool DFP_QueryFolderInStorage()
{
	return DFP_SendQueryMessage(DFP_CMD_GET_NUM_STORAGE_FOLDERS, 0x00, 0x00);	
}

void DFP_Initialize()
{
	memset(RxBuffer, 0, sizeof(RxBuffer));
	memset(TxBuffer, 0, sizeof(RxBuffer));
	
	/* Initialize USART */
	USART_Init();
}

void DFP_HandleMessage()
{	
	/* No received message */
	if(!USART_Available()) return;	
			
	/* Receive 10 bytes */
	if(!USART_RxBuffer(RxBuffer, sizeof(RxBuffer)))
	{
		/* When received garbage packets, try to receive again */
		if(!USART_RxBuffer(RxBuffer, sizeof(RxBuffer)))
		{						
			return;
		}
	}
	else 
	{
		DFP_ParseMessage();		
	}
}

void DFP_SendControlMessage(uint8_t cmd, uint8_t param_msb, uint8_t param_lsb)
{		
	memset(TxBuffer, 0, sizeof(TxBuffer));
	memset(RxBuffer, 0, sizeof(RxBuffer));
	
	TxBuffer[DFP_MSG_IDX_HEADER] = DFP_MSG_VAL_HEADER;
	TxBuffer[DFP_MSG_IDX_VERSION] = DFP_MSG_VAL_VERSION;
	TxBuffer[DFP_MSG_IDX_LENGTH] = DFP_MSG_VAL_LENGTH;
	TxBuffer[DFP_MSG_IDX_COMMAND] = cmd;
	TxBuffer[DFP_MSG_IDX_FEEDBACK] = DFP_MSG_VAL_NOFEEDBACK;
	TxBuffer[DFP_MSG_IDX_PARAM_MSB] = param_msb;
	TxBuffer[DFP_MSG_IDX_PARAM_LSB] = param_lsb;
	uint16_t checksum = DFP_CalChecksum();
	TxBuffer[DFP_MSG_IDX_CHECK_MSB] = (checksum & 0xFF00) >> 8;
	TxBuffer[DFP_MSG_IDX_CHECK_LSB] = checksum & 0xFF;
	TxBuffer[DFP_MSG_IDX_TAIL] = DFP_MSG_VAL_TAIL;
	
	USART_TxBuffer(TxBuffer, sizeof(TxBuffer));
	
	/* Delay between messages */		
	_delay_ms(DFP_MSG_DELAY);
}

bool DFP_SendQueryMessage(uint8_t cmd, uint8_t param_msb, uint8_t param_lsb)
{		
	memset(TxBuffer, 0, sizeof(TxBuffer));
	memset(RxBuffer, 0, sizeof(RxBuffer));
	
	TxBuffer[DFP_MSG_IDX_HEADER] = DFP_MSG_VAL_HEADER;
	TxBuffer[DFP_MSG_IDX_VERSION] = DFP_MSG_VAL_VERSION;
	TxBuffer[DFP_MSG_IDX_LENGTH] = DFP_MSG_VAL_LENGTH;
	TxBuffer[DFP_MSG_IDX_COMMAND] = cmd;
	TxBuffer[DFP_MSG_IDX_FEEDBACK] = DFP_MSG_VAL_NOFEEDBACK;
	TxBuffer[DFP_MSG_IDX_PARAM_MSB] = param_msb;
	TxBuffer[DFP_MSG_IDX_PARAM_LSB] = param_lsb;	
	uint16_t checksum = DFP_CalChecksum();		
	TxBuffer[DFP_MSG_IDX_CHECK_MSB] = (checksum & 0xFF00) >> 8;
	TxBuffer[DFP_MSG_IDX_CHECK_LSB] = checksum & 0xFF;
	TxBuffer[DFP_MSG_IDX_TAIL] = DFP_MSG_VAL_TAIL;
	
	USART_TxBuffer(TxBuffer, sizeof(TxBuffer));		
		
	if(!USART_RxBuffer(RxBuffer, sizeof(RxBuffer)))
	{		
		/* When received garbage packets, try to receive again */
		if(!USART_RxBuffer(RxBuffer, sizeof(RxBuffer)))
		{
			/* Delay between messages */
			_delay_ms(DFP_MSG_DELAY);
			
			return false;
		}		
	}
	
	DFP_ParseMessage();	
	
	/* Delay between messages */
	_delay_ms(DFP_MSG_DELAY);
	
	return true;
}

static bool DFP_ParseMessage()
{
	/* Check protocol format */
	if(RxBuffer[DFP_MSG_IDX_HEADER] != DFP_MSG_VAL_HEADER) return false;
	if(RxBuffer[DFP_MSG_IDX_LENGTH] != DFP_MSG_VAL_LENGTH) return false;
	if(RxBuffer[DFP_MSG_IDX_TAIL] != DFP_MSG_VAL_TAIL) return false;
	
	/* Check message command */
	uint8_t cmd = RxBuffer[DFP_MSG_IDX_COMMAND];
	
	switch(cmd)
	{
		case DFP_CMD_STORAGE_IN:				/* Storage plugged in */
		case DFP_CMD_STORAGE_OUT:				/* Storage plugged out */
		DFPlayerInfo.StorageStatus = RxBuffer[DFP_MSG_IDX_PARAM_LSB];
		break;
		case DFP_CMD_USB_TRACK_FINISHED:		/* USB track finished playing */
		case DFP_CMD_SD_TRACK_FINISHED:			/* SD card track finished playing */
		DFPlayerInfo.FinishedTrack = (RxBuffer[DFP_MSG_IDX_PARAM_MSB] << 8 | RxBuffer[DFP_MSG_IDX_PARAM_LSB]);
		break;
		case DFP_CMD_GET_CUR_STORAGE:			/* Current storage device */
		DFPlayerInfo.CurrentStorageDevice = RxBuffer[DFP_MSG_IDX_PARAM_LSB];
		break;
		case DFP_CMD_ERROR:						/* Error */
		DFPlayerInfo.Error = RxBuffer[DFP_MSG_IDX_PARAM_LSB];
		break;
		case DFP_CMD_FEEDBACK:					/* Feedback */
		break;
		case DFP_CMD_GET_CUR_STATUS:			/* Current status */
		DFPlayerInfo.CurrentStatus = RxBuffer[DFP_MSG_IDX_PARAM_LSB];
		break;
		case DFP_CMD_GET_CUR_VOLUME:			/* Current volume */
		DFPlayerInfo.CurrentVolume = RxBuffer[DFP_MSG_IDX_PARAM_LSB];
		break;
		case DFP_CMD_GET_CUR_EQ:				/* Current EQ */
		DFPlayerInfo.CurrentEQ = RxBuffer[DFP_MSG_IDX_PARAM_LSB];
		break;
		case DFP_CMD_GET_NUM_USB_ROOT_TRACKS:	/* # of tracks in USB */
		case DFP_CMD_GET_NUM_SD_ROOT_TRACKS:	/* # of tracks in SD card */
		DFPlayerInfo.NumOfTracks = (RxBuffer[DFP_MSG_IDX_PARAM_MSB] << 8 | RxBuffer[DFP_MSG_IDX_PARAM_LSB]);
		break;
		case DFP_CMD_GET_CUR_USB_TRACK:			/* Current track in USB */
		case DFP_CMD_GET_CUR_SD_TRACK:			/* Current track in SD card */
		DFPlayerInfo.CurrentTrack = (RxBuffer[DFP_MSG_IDX_PARAM_MSB] << 8 | RxBuffer[DFP_MSG_IDX_PARAM_LSB]);
		break;
		case DFP_CMD_GET_NUM_FOLDER_TRACKS:		/* # of tracks in a folder */
		DFPlayerInfo.NumOfTracksInFolder = (RxBuffer[DFP_MSG_IDX_PARAM_MSB] << 8 | RxBuffer[DFP_MSG_IDX_PARAM_LSB]);
		break;
		case DFP_CMD_GET_NUM_STORAGE_FOLDERS:	/* # of folders in root directory */
		DFPlayerInfo.NumOfFolders = RxBuffer[DFP_MSG_IDX_PARAM_LSB];
		break;
		default:		/* Unknown command */
		break;
	}
	return true;
}

static uint16_t DFP_CalChecksum()
{
	uint16_t checksum = TxBuffer[DFP_MSG_IDX_VERSION];
	checksum += TxBuffer[DFP_MSG_IDX_LENGTH];
	checksum += TxBuffer[DFP_MSG_IDX_COMMAND];
	checksum += TxBuffer[DFP_MSG_IDX_FEEDBACK];
	checksum += TxBuffer[DFP_MSG_IDX_PARAM_MSB];
	checksum += TxBuffer[DFP_MSG_IDX_PARAM_LSB];
	
	checksum = 0xFFFF - checksum + 1;
	
	return checksum;
}