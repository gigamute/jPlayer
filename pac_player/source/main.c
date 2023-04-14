#include <nds.h>
#include <nf_lib.h>
#include "songArrays.h"

#define TIMER_SPEED (BUS_CLOCK/1024) // Speed of hardware timer
#define TUNE_FREQ 11025 // Sample frequency
#define SAMPLE_RATE 0 // Sample rate
#define MUSIC_SIZE 12 // Amount of songs

// Struct definition for icon object
typedef struct {
	// X position of object
	int x;
	// Y position of object
	int y;
} Icon;

// Struct definition of metasprite object
typedef struct {
	// X position of object
	int x;
	// Y position of object
	int y;

	// ID of object.
	int id;
} MetaSprite;

// Struct definition for music player object called jPlayer
typedef struct {
	bool PAUSED;
	// int variable called time needed for hardware timer.
	int time;
	bool PLAYING;
	// int variable called playIndex needed to increment song segments.
	int playIndex;
	// channel slot id
	u8 id;

	const char* songArray[ARRAY_SIZE];
	// An array of arrays, where each element is copied to songArray[]
	const char* arrayBank[MUSIC_SIZE][ARRAY_SIZE];
	u8 songId;
} jPlayer;

typedef enum {

	timerState_Running,
	timerState_Paused,
	timerState_Stopped

} timerStates;

void loadAssets() {
	NF_LoadSpriteGfx("GFX/playback",0,16,16);
	NF_LoadSpritePal("GFX/playback",0);
	NF_VramSpriteGfx(1,0,0,false);
	NF_VramSpritePal(1,0,0);
	NF_LoadSpriteGfx("GFX/2pacalypsenow_coverart",1,32,32);
	NF_LoadSpritePal("GFX/2pacalypsenow_coverart",1);
	NF_VramSpriteGfx(1,1,1,false);
	NF_VramSpritePal(1,1,1);
	NF_LoadSpriteGfx("GFX/pacstricly_coverart",2,32,32);
	NF_LoadSpritePal("GFX/pacstricly_coverart",2);
	NF_VramSpriteGfx(1,2,2,false);
	NF_VramSpritePal(1,2,2);
	NF_LoadSpriteGfx("GFX/thuglife_coverart",3,32,32);
	NF_LoadSpritePal("GFX/thuglife_coverart",3);
	NF_VramSpriteGfx(1,3,3,false);
	NF_VramSpritePal(1,3,3);
	NF_LoadSpriteGfx("GFX/matw_coverart",4,32,32);
	NF_LoadSpritePal("GFX/matw_coverart",4);
	NF_VramSpriteGfx(1,4,4,false);
	NF_VramSpritePal(1,4,4);
	NF_LoadSpriteGfx("GFX/aeom_coverart",5,32,32);
	NF_LoadSpritePal("GFX/aeom_coverart",5);
	NF_VramSpriteGfx(1,5,5,false);
	NF_VramSpritePal(1,5,5);
	NF_LoadSpriteGfx("GFX/makiaveli_coverart",6,32,32);
	NF_LoadSpritePal("GFX/makiaveli_coverart",6);
	NF_VramSpriteGfx(1,6,6,false);
	NF_VramSpritePal(1,6,6);
}

void drawSprites(int x1,int x2,int x3, int y1, int y2, int y3) {
	NF_CreateSprite(1,0,0,0,x1,y1);
	NF_CreateSprite(1,1,0,0,x2,y2);
	NF_CreateSprite(1,2,0,0,x3,y3);
	NF_SpriteFrame(1,0,1);
	NF_SpriteFrame(1,1,0);
	NF_SpriteFrame(1,2,2);
}

void drawCoverArt(int x, int y, int id)
{
	NF_CreateSprite(1,3,id,id,x,y);
	NF_CreateSprite(1,4,id,id,x,y+32);
	NF_CreateSprite(1,5,id,id,x+32,y);
	NF_CreateSprite(1,6,id,id,x+32,y+32);
	NF_SpriteFrame(1,3,0);
	NF_SpriteFrame(1,4,1);
	NF_SpriteFrame(1,5,2);
	NF_SpriteFrame(1,6,3);

}

void chooseCoverArtID(int x, int y, int song_id,int cover_id)
{
	if ((song_id >= 0) && (song_id <= 1)) {
		cover_id = 1;
		drawCoverArt(x,y,cover_id);
	}
	else if ((song_id >= 2) && (song_id <= 3)) {
		cover_id = 2;
		drawCoverArt(x,y,cover_id);
	}
	else if ((song_id >= 4) && (song_id <= 5)) {
		cover_id = 3;
		drawCoverArt(x,y,cover_id);
	}
	else if ((song_id >= 6) && (song_id <= 7)) {

		cover_id = 4;
		drawCoverArt(x,y,cover_id);
	}
	else if ((song_id >= 8) && (song_id <= 9)) {
		cover_id = 5;
		drawCoverArt(x,y,cover_id);
	}
	else if ((song_id >= 10) && (song_id <= 11)) {
		cover_id = 6;
		drawCoverArt(x,y,cover_id);
	}

}

void copySongsToBank(const char* array[MUSIC_SIZE][ARRAY_SIZE])
{
	memcpy(array[0],tupac_brenda,sizeof(tupac_brenda));
	memcpy(array[1],tupac_rebel,sizeof(tupac_rebel));
	memcpy(array[2],tupac_keepyahead,sizeof(tupac_keepyahead));
	memcpy(array[3],tupac_getaround,sizeof(tupac_getaround));
	memcpy(array[4],tupac_buryg,sizeof(tupac_buryg));
	memcpy(array[5],tupac_liquor,sizeof(tupac_liquor));
	memcpy(array[6],tupac_mama,sizeof(tupac_mama));
	memcpy(array[7],tupac_ftw,sizeof(tupac_ftw));
}

int main(int argc, char* argv[]) {
	NF_Set2D(0,0);
	NF_Set2D(1,0);
	NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
	NF_InitSpriteSys(1);
	NF_InitRawSoundBuffers();
	NF_SetRootFolder("NITROFS");
	loadAssets();
	soundEnable();
	jPlayer player;
	Icon left;
	left.x = 256/2 - 16/2 - 60;
	left.y = 192/2 - 16/2 + 60;
	Icon right;
	right.x = 256/2 - 16/2 + 60;
	right.y = 192/2 - 16/2 + 60;
	Icon middle;
	middle.x = 256/2 - 16/2;
	middle.y = 192/2 - 16/2 + 60;

	MetaSprite coverArt;
	coverArt.x = 256/2 - 64/2;
	coverArt.y = 20;
	coverArt.id = 1;

	player.time = 0;
	player.PLAYING = true;
	player.playIndex = 0;
	player.songId = 0;
	player.PAUSED = false;
	copySongsToBank(player.arrayBank);
	memcpy(player.songArray,player.arrayBank[player.songId],sizeof(player.arrayBank[player.songId]));
	player.id = 0;
	int channel_id;
	NF_LoadRawSound(player.songArray[player.playIndex],player.id,TUNE_FREQ,SAMPLE_RATE);
	NF_LoadRawSound("SFX/press",1,22050,0);
	timerStates state = timerState_Stopped;
	while (1)
	{
		drawSprites(left.x,right.x,middle.x,left.y,right.y,middle.y);
		chooseCoverArtID(coverArt.x,coverArt.y,player.songId,coverArt.id);

		scanKeys();
		if (keysDown() & KEY_RIGHT) {
			NF_PlayRawSound(1,127,64,false,0);
			if (player.songId < 7) {
				player.PLAYING = false;
				timerStop(3);
				NF_UnloadRawSound(player.id);
				soundKill(channel_id);
				player.playIndex = 0;
				player.songId++;
				memcpy(player.songArray,player.arrayBank[player.songId],sizeof(player.arrayBank[player.songId]));
				player.time = 0;
				state = timerState_Stopped;
			}
		}
		else if (keysDown() & KEY_LEFT) {
			NF_PlayRawSound(1,127,64,false,0);
			if (player.songId > 0) {
				player.PLAYING = false;
				timerStop(3);
				NF_UnloadRawSound(player.id);
				soundKill(channel_id);
				player.playIndex = 0;
				player.songId--;
				memcpy(player.songArray,player.arrayBank[player.songId],sizeof(player.arrayBank[player.songId]));
				player.time = 0;
				state = timerState_Stopped;
			}
		}
		if (!player.PLAYING)
		{
			NF_LoadRawSound(player.songArray[player.playIndex],player.id,TUNE_FREQ,SAMPLE_RATE);
			player.PLAYING = true;
		}

		if (player.PLAYING)
		{
			if (state == timerState_Stopped)
			{
				channel_id = NF_PlayRawSound(player.id,127,64,false,0);
				timerStart(3,ClockDivider_1024,0,NULL);
				state = timerState_Running;
			}
			else if (state == timerState_Running)
				player.time += timerElapsed(3);
			

			if ((player.time/TIMER_SPEED) > 4)
			{
				timerStop(3);
				player.time = 0;
				soundKill(channel_id);
				NF_UnloadRawSound(player.id);
				state = timerState_Stopped;
				player.playIndex++;

				if (player.songArray[player.playIndex] == NULL) {
					player.playIndex = 0;
					player.PLAYING = false;
				}
				else
					NF_LoadRawSound(player.songArray[player.playIndex],player.id,TUNE_FREQ,SAMPLE_RATE);
			}
		}

		NF_SpriteOamSet(0);
		NF_SpriteOamSet(1);
		swiWaitForVBlank();
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);
	}
	
	return 0;
}
