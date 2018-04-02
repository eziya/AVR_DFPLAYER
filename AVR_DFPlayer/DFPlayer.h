/*
 * DFPlayer.h
 *
 * Created: 2018-03-28 오후 5:59:49
 *  Author: kiki
 */ 


#ifndef __DFPLAYER_H_
#define __DFPLAYER_H_

#include "328P_USART.h"

#define DFP_MSG_SIZE		10
#define DFP_MSG_DELAY		200

#define DFP_STORAGE_USB			1
#define DFP_STORAGE_SDCARD		2

#define DFP_EQ_NORMAL			0
#define DFP_EQ_POP				1
#define DFP_EQ_ROCK				2
#define DFP_EQ_JAZZ				3
#define DFP_EQ_CLASSIC			4
#define DFP_EQ_BASS				5

/* Protocol Index */
#define DFP_MSG_IDX_HEADER		0
#define DFP_MSG_IDX_VERSION		1
#define DFP_MSG_IDX_LENGTH		2
#define DFP_MSG_IDX_COMMAND		3
#define DFP_MSG_IDX_FEEDBACK	4
#define DFP_MSG_IDX_PARAM_MSB	5
#define DFP_MSG_IDX_PARAM_LSB	6
#define DFP_MSG_IDX_CHECK_MSB	7
#define DFP_MSG_IDX_CHECK_LSB	8
#define DFP_MSG_IDX_TAIL		9

/* Default values */
#define DFP_MSG_VAL_HEADER		0x7E
#define DFP_MSG_VAL_VERSION		0xFF
#define DFP_MSG_VAL_LENGTH		0x06
#define DFP_MSG_VAL_NOFEEDBACK	0x00
#define DFP_MSG_VAL_TAIL		0xEF

/* Operation commands */
#define DFP_CMD_PLAY_NEXT			0x01
#define DFP_CMD_PLAY_PREVIOUS		0x02
#define DFP_CMD_PLAY_TRACK			0x03
#define DFP_CMD_INC_VOLUME			0x04
#define DFP_CMD_DEC_VOLUME			0x05
#define DFP_CMD_SET_VOLUME			0x06
#define DFP_CMD_SET_EQ				0x07
#define DFP_CMD_REPEAT_TRACK		0x08
#define DFP_CMD_SET_STORAGE			0x09
#define DFP_CMD_SET_SLEEP			0x0A
#define DFP_CMD_RESET				0x0C
#define DFP_CMD_PLAY				0x0D
#define DFP_CMD_PAUSE				0x0E
#define DFP_CMD_PLAY_TRACK_FOLDER	0x0F
#define DFP_CMD_SET_AMP				0x10
#define DFP_CMD_REPEAT_ALL			0x11
#define DFP_CMD_PLAY_MP3_FOLDER		0x12
#define DFP_CMD_INSERT_ADVERT		0x13
#define DFP_CMD_PLAY_3K_FOLDER		0x14
#define DFP_CMD_STOP_ADVERT			0x15
#define DFP_CMD_STOP				0x16
#define DFP_CMD_REPEAT_FOLDER		0x17
#define DFP_CMD_PLAY_RANDOM			0x18
#define DFP_CMD_REPEAT_CUR_TRACK	0x19
#define DFP_CMD_SET_DAC				0x1A
#define DFP_CMD_STORAGE_IN			0x3A
#define DFP_CMD_STORAGE_OUT			0x3B
#define DFP_CMD_USB_TRACK_FINISHED	0x3C
#define DFP_CMD_SD_TRACK_FINISHED	0x3D

/* Query commands */
#define DFP_CMD_GET_CUR_STORAGE				0x3F
#define DFP_CMD_ERROR						0x40
#define DFP_CMD_FEEDBACK					0x41
#define DFP_CMD_GET_CUR_STATUS				0x42
#define DFP_CMD_GET_CUR_VOLUME				0x43
#define DFP_CMD_GET_CUR_EQ					0x44
#define DFP_CMD_GET_NUM_USB_ROOT_TRACKS		0x47
#define DFP_CMD_GET_NUM_SD_ROOT_TRACKS		0x48
#define DFP_CMD_GET_CUR_USB_TRACK			0x4B
#define DFP_CMD_GET_CUR_SD_TRACK			0x4C
#define DFP_CMD_GET_NUM_FOLDER_TRACKS		0x4E
#define DFP_CMD_GET_NUM_STORAGE_FOLDERS		0x4F

typedef struct {
	uint8_t StorageStatus;
	uint16_t FinishedTrack;
	uint8_t CurrentStorageDevice;
	uint8_t Error;
	uint8_t CurrentStatus;
	uint8_t CurrentVolume;
	uint8_t CurrentEQ;
	uint16_t NumOfTracks;
	uint16_t CurrentTrack;
	uint16_t NumOfTracksInFolder;
	uint8_t NumOfFolders;	
} _DFPlayerInfo;

extern _DFPlayerInfo DFPlayerInfo;

void DFP_PlayNext(void);
void DFP_PlayPrevious(void);
void DFP_PlayTrackInRoot(uint16_t track);
void DFP_IncreaseVolume(void);
void DFP_DecreaseVolume(void);
void DFP_SetVolume(uint8_t volume);
void DFP_SetEQ(uint8_t eq);
void DFP_RepeatTrack(uint16_t track);
void DFP_SetStorage(uint8_t storage);
void DFP_SetSleep(void);
void DFP_Reset(void);
void DFP_Play(void);
void DFP_Pause(void);
void DFP_PlayTrackInFolder(uint8_t folder, uint8_t track);
void DFP_SetAudioGain(bool onoff, uint8_t gain);
void DFP_RepeatAll(bool repeat);
void DFP_PlayTrackInMp3(uint16_t track);
void DFP_InsertAdvertisement(uint16_t track);
void DFP_PlayTrackIn3KFolder(uint8_t folder, uint16_t track);
void DFP_StopAdvertisement(void);
void DFP_Stop(void);
void DFP_RepeatTrackInFolder(uint8_t folder);
void DFP_PlayRandomTrack(void);
void DFP_RepeatCurrentTrack(bool onoff);
void DFP_SetDAC(bool onoff);

bool DFP_QueryCurrentStorage(void);
bool DFP_QueryCurrentStatus(void);
bool DFP_QueryCurrentVolume(void);
bool DFP_QueryCurrentEQ(void);
bool DFP_QueryTracksInRoot(void);
bool DFP_QueryCurrentTrack(void);
bool DFP_QueryTrackInFolder(uint8_t folder);
bool DFP_QueryFolderInStorage(void);

void DFP_Initialize(void);
void DFP_HandleMessage(void);

#endif /* __DFPLAYER_H_ */