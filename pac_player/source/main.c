// Wondersound presents: Makaveli's (2Pac actually) album, The Don Killuminati: The 7 Day Theory.

/* This Nintendo DS application is the entire album put on the console, so that people can listen to this
in the low freq and bitrate that it is. It's the entire 12 songs, and due to my method, and some limitations,
there are some quirks (long gaps and such). However, you can just pretend it's 2004 while listening to this.

The application is meant to be part of a possible series known as Wondersound. Wondersound is meant to be a digital album of whichever. A brand new way to own music.*/
#include <nds.h>
#include <nf_lib.h>
#include "songArrays.h"
#include <string.h>
#include <stdio.h>

#define TIMER_SPEED (BUS_CLOCK/1024) // Speed of hardware timer
#define TUNE_FREQ 11025 // Sample frequency
#define SAMPLE_RATE 0 // Sample rate
#define MUSIC_SIZE 12 // Amount of songs
#define INIT_MENU_OPTIONS 4 // Amount of menu options of first screen
#define BOOKLET_PAGES 2
#define MENU_LOGO_WIDTH 128
#define MENU_LOGO_HEIGHT 128
#define NUM_GAMESTATES 4
// Determines the state of the game, whether we are in the menu
// enum GameState {
// 	MENU_MODE,
// 	INSTRUCT_ALBUM_MODE,
// 	ALBUM_MODE
// };

typedef enum {
	MENU_MODE,
	INSTRUCT_ALBUM_MODE,
	ALBUM_MODE,
	INSTRUCT_ALBUM_MODE_2
} GameStates;

GameStates gState = MENU_MODE;
int cursor_index = 0;

typedef enum {
	SPRITE_NOT_CREATED,
	SPRITE_CREATED
} SpriteState;

typedef enum {
	BACKGROUND_CREATED,
	BACKGROUND_NOT_CREATED,
} backgroundState;

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

// Create a new player object that will represent the music player used to play the album.
jPlayer player;

// Timer states used to check if the timer used to play music is running, paused, or stopped.
typedef enum {
	timerState_Running,
	timerState_Paused,
	timerState_Stopped
} timerStates;

typedef enum {
	ALBUM_PLAYING,
	ALBUM_NOT_PLAYING
} albumStates;

SpriteState menuBkgState = SPRITE_NOT_CREATED;
SpriteState sprState = SPRITE_NOT_CREATED;
// GameState splashStates[3] = {SPLASH_MODE_1,SPLASH_MODE_2,SPLASH_MODE_3};

// Load assets from the filesystem (folder 'nitrofiles)
void loadAssets() {
	NF_LoadSpriteGfx("GFX/playback",0,16,16);
	NF_LoadSpritePal("GFX/playback",0);
	NF_Vram3dSpriteGfx(0,0,false);
	NF_Vram3dSpritePal(0,0);
	NF_LoadSpriteGfx("GFX/mini_album_art",1,64,64);
	NF_LoadSpritePal("GFX/mini_album_art",1);
	NF_Vram3dSpriteGfx(1,1,false);
	NF_Vram3dSpritePal(1,1);
	//NF_LoadSpriteGfx("GFX/makaveli_logo",2,MENU_LOGO_WIDTH,MENU_LOGO_HEIGHT);
	//NF_LoadSpritePal("GFX/makaveli_logo",2);
	//NF_Vram3dSpriteGfx(2,2,false);
	//NF_Vram3dSpritePal(2,2);
	NF_LoadTextFont16("fonts/font16","font",256,256,0);
	//NF_LoadSpriteGfx("GFX/tracklist",2,64,512);
	//NF_LoadSpritePal("GFX/tracklist",2);
	//NF_Vram3dSpriteGfx(2,2,false);
	//NF_Vram3dSpritePal(2,2)
	//NF_LoadTiledBg("GFX/menu_art","album_art",256,256);
	NF_LoadTiledBg("GFX/instruct_bg","instruct_template",256,256);
	NF_LoadTiledBg("GFX/menu_bg","menu_template",256,256);
	NF_LoadTiledBg("GFX/album_front","album",256,256);
	NF_LoadTiledBg("GFX/m_back","mback",256,256);
	NF_LoadTiledBg("GFX/m_back2","mback2",256,256);
	NF_LoadTiledBg("GFX/whiteback","white",256,256);
	//NF_LoadTiledBg("GFX/pac_menu_screen","pac_2",256,256);
	//NF_LoadTiledBg("GFX/interscope_splash","interscope",256,256);
	//NF_LoadTiledBg("GFX/deathrow_splash","deathrow",256,256);
	//NF_LoadTiledBg("GFX/amaru_splash","amaru",256,256);
	//NF_LoadTiledBg("GFX/credit","credit",256,256);
}

// Draw sprites of left and right arrow, and the pause button (to be removed.)
void drawSprites(int x1,int x2,int x3, int y1, int y2, int y3,int id) {
	NF_Create3dSprite(0,0,0,x1,y1);
	NF_Create3dSprite(1,0,0,x2,y2);
	//NF_Create3dSprite(2,0,0,x3,y3);
	NF_Set3dSpriteFrame(0,1);
	NF_Set3dSpriteFrame(1,0);
	//NF_Set3dSpriteFrame(2,id);
	//NF_Rotate3dSprite(0,0,0,128);
	//NF_Rotate3dSprite(1,0,0,128);
	//NF_Rotate3dSprite(2,0,0,128);
}

// Draw cover art
void drawCoverArt(int x, int y, int id)
{
	NF_Create3dSprite(3,id,id,x,y);
}

// void chooseCoverArtID(int x, int y, int song_id,int cover_id)
// {
// 	if ((song_id >= 0) && (song_id <= 1)) {
// 		cover_id = 1;
// 		drawCoverArt(x,y,cover_id);
// 	}
// 	else if ((song_id >= 2) && (song_id <= 3)) {
// 		cover_id = 2;
// 		drawCoverArt(x,y,cover_id);
// 	}
// 	else if ((song_id >= 4) && (song_id <= 5)) {
// 		cover_id = 3;
// 		drawCoverArt(x,y,cover_id);
// 	}
// 	else if ((song_id >= 6) && (song_id <= 7)) {

// 		cover_id = 4;
// 		drawCoverArt(x,y,cover_id);
// 	}
// 	else if ((song_id >= 8) && (song_id <= 9)) {
// 		cover_id = 5;
// 		drawCoverArt(x,y,cover_id);
// 	}
// 	else if ((song_id >= 10) && (song_id <= 11)) {
// 		cover_id = 6;
// 		drawCoverArt(x,y,cover_id);
// 	}

// }

// Copy the songs into the array bank. The array bank is a multi-dimensional array.
void copySongsToBank(const char* array[MUSIC_SIZE][ARRAY_SIZE])
{
	memcpy(&array[0],&bomb,sizeof(bomb));
	memcpy(&array[1],&hail,sizeof(hail));
	memcpy(&array[2],&toss,sizeof(toss));
	memcpy(&array[3],&live,sizeof(live));
	memcpy(&array[4],&blas,sizeof(blas));
	memcpy(&array[5],&outlaw,sizeof(outlaw));
	memcpy(&array[6],&daddy,sizeof(daddy));
	memcpy(&array[7],&krazy,sizeof(krazy));
	memcpy(&array[8],&white,sizeof(white));
	memcpy(&array[9],&girlfriend,sizeof(girlfriend));
	memcpy(&array[10],&head,sizeof(head));
	memcpy(&array[11],&odds,sizeof(odds));

}

// Used to 
int checkStylusPos(touchPosition pen, int iconX, int iconY) {

	if (pen.px <= iconX + 16 && pen.px >= iconX) {
		if (pen.py <= iconY + 16 && pen.py >= iconY) {
			return 1;
		}
	}

	return 0;
}

int checkStylusPosMenu(touchPosition pen, int x, int y) {

	if (pen.px <= x + 256 && pen.px >= y) {
		if (pen.py <= y + 192 && pen.py >= y) {
			return 1;
		}
	}

	return 0;
}

const char* musicTitleList [MUSIC_SIZE][2] = {
	{"1. Bomb First (My Second Reply)","(featuring Outlawz)"},
	{"2. Hail Mary","(featuring Outlawz)"},
	{"3. Toss It Up",""},
	{"4. To Live & Die in L.A.","(featuring Val Young)"},
	{"5. Blasphemy",""},
	{"6. Life of an Outlaw","(featuring Outlawz)"},
	{"7. Just Like Daddy","(featuring Outlawz)"},
	{"8. Krazy","(featuring Bad Azz)"},
	{"9. White Man'z World",""},
	{"10. Me and My Girlfriend",""},
	{"11. Hold Ya Head","(featuring Hurt-M-Badd)"},
	{"12. Against All Odds",""}

};
 
int cursor_y_pos[INIT_MENU_OPTIONS][2] = {
	{9,2},
	{14,4},
	{20,6},
	{12,8}
};

int stopTimes[MUSIC_SIZE] = {
	17,
	10,
	7,
	14,
	19,
	16,
	8,
	16,
	19,
	9,
	19,
	18
};

int getActualSize() {
	int size = 0;
	for (size; size < ARRAY_SIZE; size++) {
		if (player.songArray[size] == NULL) {
			return size - 1;
		}
	}

	return size; 
}

void fade(bool f1, bool f2,int f_amt) {
	if ((f1) && !(f2))  {
		for (f_amt; f_amt >= -16; f_amt--) {
			setBrightness(1,f_amt);
			//swiWaitForVBlank();

			if (f_amt == -16) {
				if (gState == MENU_MODE) {
					NF_DeleteTiledBg(0,3);
					//NF_Delete3dSprite(0);
					NF_ClearTextLayer16(0,1);
					//NF_ClearTextLayer16(0,2);
					menuBkgState = SPRITE_NOT_CREATED;
					gState = INSTRUCT_ALBUM_MODE;
				}
				else if (gState == INSTRUCT_ALBUM_MODE) {
					NF_ClearTextLayer16(0,1);
					//NF_ClearTextLayer16(0,2);
					gState = INSTRUCT_ALBUM_MODE_2;
				}
				else if (gState == INSTRUCT_ALBUM_MODE_2) {
					NF_ClearTextLayer16(0,1);
					//NF_ClearTextLayer16(0,2);
					NF_DeleteTiledBg(0,3);
					menuBkgState = SPRITE_NOT_CREATED;
					gState = ALBUM_MODE;
				}
				else if (gState == ALBUM_MODE) {
					NF_ClearTextLayer16(0,1);
					NF_Delete3dSprite(0);
					NF_Delete3dSprite(1);
					NF_Delete3dSprite(3);
					NF_ScrollBg(0,1,0,0);
					cursor_index = 0;
					sprState = SPRITE_NOT_CREATED;
					menuBkgState = SPRITE_NOT_CREATED;
					gState = MENU_MODE;
				}
			}
		}

		f1 = false;
		f2 = true;	
	}

	if ((f2) && !(f1)) {
		for (f_amt; f_amt <= 0; f_amt++) {
			setBrightness(1,f_amt);
			//swiWaitForVBlank();

			if (f_amt == 0) {
				f1 = false;
				f2 = false;
				f_amt = 0;
			}
		}
	}
}
int main(int argc, char* argv[]) {
	NF_SetRootFolder("NITROFS");
	// Set up the 2D engine for the top and bottom screen in mode 0.
	NF_Set2D(0,0);
	NF_Set2D(1,0);

	// Set up the 3D engine with mode 0, for the bottom engine, from this point onward, 0 and 1 (top and bottom), is now (1 and 0)
	NF_Set3D(1,0);
	// Initiate tiled background buffers
	NF_InitTiledBgBuffers();
	// Initiate tiled background system for both screens
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);
	// Initiate sprite buffers
	NF_InitSpriteBuffers();
	// Inititate 3D sprite system for the 3D screen
	NF_Init3dSpriteSys();
	// Initiate raw sound buffers.
	NF_InitRawSoundBuffers();
	// Init text system
	NF_InitTextSys(0);
	// Set root folder to the nitro filesystem
	// Load assets
	loadAssets();
	// Enable sound
	soundEnable();
	// Create a new left icon
	Icon left;
	// Set left icon x and y position, to left side of the screen
	left.x = (256/2 - (16/2)) - 72;
	left.y = (192/2 - (16/2));
	// Create a new right icon
	Icon right;
	// Set right x and y position, to the right side of the screen
	right.x = (256/2 - (16/2)) + 72;
	right.y = (192/2 - (16/2));
	// Create the middle icon
	Icon middle;
	// Set the middle x and y position, to the middle of the screen
	middle.x = 60;
	middle.y = 192/2 - 16/2;

	// Create a new MetaSprite object, signifying the 
	MetaSprite coverArt;
	// Set the coverArt x and y position, to the middle of the screen, slighty upper.
	coverArt.x = 256/2 - (64/2);
	coverArt.y = 192/2 - (64/2);
	coverArt.id = 1;
	// Tick used for our DS timer
	player.time = 0;
	// Set playing equal to true
	player.PLAYING = false;
	// Set the player's play index to 0. The play index is the 20 song segment that is played as a SFX.
	player.playIndex = 0;
	// Set the player's song id. The song id is song that is playing
	player.songId = 0;
	player.PAUSED = false;
	// Copy songs to the array bank.
	copySongsToBank(player.arrayBank);
	// Copy the first array of the arraybank at index song id, into the player's song array, making it the first song played.
	memcpy(&player.songArray,&player.arrayBank[player.songId],sizeof(player.arrayBank[player.songId]));
	// Played id is the current channel (0 - 31)
	player.id = 0;
	int channel_id;
	// Load the current sound from the player's song array at the index, player.playindex, into memory.
	NF_LoadRawSound(player.songArray[player.playIndex],player.id,TUNE_FREQ,SAMPLE_RATE);
	// Load the sfx into memory
	NF_LoadRawSound("SFX/menu_sound",1,TUNE_FREQ,0);
	NF_LoadRawSound("SFX/menu_sound_2",2,TUNE_FREQ,0);
	NF_LoadRawSound("SFX/collideside",3,TUNE_FREQ,0);

	// Set layer
	NF_CreateTextLayer16(0,1,0,"font");
	NF_CreateTextLayer16(0,2,0,"font");
	// Define text color
	NF_DefineTextColor(0,1,1,31,0,0);
	NF_DefineTextColor(0,2,2,31,0,0);
	// Set the timer state to stopped
	bool FADE_OUT = true;
	bool FADE_IN = false;
	bool BOOT = true;
	int splashTick = 0;
	int fadeAmt = 0;
	timerStates splashState = timerState_Stopped;
	timerStates state = timerState_Stopped;
	// Set the sprite state to not created
	SpriteState splash_state_0 = SPRITE_NOT_CREATED;
	SpriteState splash_state_1 = SPRITE_NOT_CREATED;
	SpriteState splash_state_2 = SPRITE_NOT_CREATED;
	SpriteState splash_state_3 = SPRITE_NOT_CREATED;
	// Create stylus object
	touchPosition stylus;
	// Create GameState object that signifies what mode the game is in
	int fadeIndex = 0;
	int arr_index = 3;
	int x = 0;
	int back_x1 = 0;
	int back_x2 = 0;
	// strncpy(&mode,&gamestates[0],sizeof(gamestates[0]));
	fifoSendValue32(FIFO_PM, PM_REQ_SLEEP_DISABLE);
	albumStates aState = ALBUM_NOT_PLAYING;
	int stopTime = 19;
	while (1) {	

			if (menuBkgState == SPRITE_NOT_CREATED) {
				if (gState == MENU_MODE) {
					NF_CreateTiledBg(0,3,"menu_template");
					NF_CreateTiledBg(1,0,"album");
					//NF_Create3dSprite(0,2,2,256-MENU_LOGO_WIDTH,192-96);
				}
				else if (gState == INSTRUCT_ALBUM_MODE) {
					NF_CreateTiledBg(0,3,"instruct_template");
				}
				//NF_CreateTiledBg(1,0,"album_art");
				//NF_CreateTiledBg(0,3,"background");
				//NF_CreateTiledBg(0,2,"pac_2");
				menuBkgState = SPRITE_CREATED;
			}
			back_x1+=2;
			back_x2+=1;

			NF_CreateTiledBg(1,1,"mback");
			NF_CreateTiledBg(1,2,"mback2");
			NF_CreateTiledBg(1,3,"white");
			NF_ScrollBg(1,1,back_x1,0);
			NF_ScrollBg(1,2,back_x2,0);
			// If the player is playing, we essentially start the timer, and start playing the song segment.
			if (player.PLAYING)
			{	
				// Timer state says the timer state is stopped, we set the channel id to the result of playing the raw sound segment located in the player id.
				if (state == timerState_Stopped)
				{	// Assign the channel id
					channel_id = NF_PlayRawSound(player.id,127,64,false,0);
					// Start timer 3 with no callback. It is essentially a stop watch
					timerStart(3,ClockDivider_1024,0,NULL);
					// Set the timer state to say that the timer stat is running
					state = timerState_Running;
				}
				// While the timer is running, the player time is set to the elapsed time of timer 3
				else if (state == timerState_Running)
					player.time += timerElapsed(3);
				
				// If the ticks divided by the timer speed is greater than 19 (20 seconds), stop the timer, and move onto the next song segment, and if the next segment is null
				// move onto the next song

				if (player.playIndex == getActualSize()) {
					stopTime = stopTimes[player.songId];
				}
				else {
					stopTime = 20;
				}
				if ((player.time/TIMER_SPEED) == stopTime)
				{
					NF_ClearTextLayer16(0,1);
					//NF_ClearTextLayer16(0,2);
					// Stop timer 3
					timerStop(3);
					// Reset player time to 0.
					player.time = 0;
					// Kill the channel at the channel_id
					soundKill(channel_id);
					// Unload the raw sound at the player.id
					NF_UnloadRawSound(player.id);
					// Increment the player's play index
					player.playIndex++;
					// Set the timer state to say the timer has stopped.
					state = timerState_Stopped;
					// If this segment at index play index in the song aray is null, we move on to the next song
					if (player.songArray[player.playIndex] == NULL) {
						// Reset play index
						player.playIndex = 0;
						// Increment the song id
						if (player.songId < MUSIC_SIZE - 1)
							player.songId++;
						else
							player.songId = 0;
						// Copy the array in the array bank at index song id to the player's song array.
						memcpy(&player.songArray,&player.arrayBank[player.songId],sizeof(player.arrayBank[player.songId]));
						// Set player PLAYING to false
						player.PLAYING = false;
					}
					else
						// If the above is not the case, then we load the raw segment from the song array at the player id.
						NF_LoadRawSound(player.songArray[player.playIndex],player.id,TUNE_FREQ,SAMPLE_RATE);
					
				}
			}
			if (keysDown() & KEY_DOWN) {
				if (gState == MENU_MODE) {
					if (cursor_index < INIT_MENU_OPTIONS - 1) {
						cursor_index++;
						NF_ClearTextLayer16(0,1);
						NF_PlayRawSound(1,127,64,false,0);
						NF_PlayRawSound(3,127,64,false,0);
					}
				}
			}

			else if (keysDown() & KEY_UP) {
				if (gState == MENU_MODE) {
					if (cursor_index > 0) {
						cursor_index--;
						NF_ClearTextLayer16(0,1);
						NF_PlayRawSound(1,127,64,false,0);
						NF_PlayRawSound(3,127,64,false,0);
					}
			
				}
			}

			if (keysDown() & KEY_A) {
				if (gState == MENU_MODE) {
					switch (cursor_index) {
						case 0:
							// NF_ClearTextLayer16(0,1);
							// NF_ClearTextLayer16(0,2);
							// NF_DeleteTiledBg(0,3);
							// NF_Delete3dSprite(0);
							// //menuBkgState = SPRITE_NOT_CREATED;
							// mode = INSTRUCT_ALBUM_MODE;
							FADE_OUT = true;
							fade(FADE_OUT,FADE_IN,fadeAmt);
							NF_PlayRawSound(2,127,64,false,0);
							NF_PlayRawSound(3,127,64,false,0);
							break;
						default:
							break;
					}
				}
				else if ((gState == INSTRUCT_ALBUM_MODE) || (gState == INSTRUCT_ALBUM_MODE_2)) {
					FADE_OUT = true;
					fade(FADE_OUT,FADE_IN,fadeAmt);
					NF_PlayRawSound(2,127,64,false,0);
					NF_PlayRawSound(3,127,64,false,0);
				}
			}
			if (keysDown() & KEY_B) {
				if (gState == ALBUM_MODE) {
					FADE_OUT = true;
					fade(FADE_OUT,FADE_IN,fadeAmt);
					NF_PlayRawSound(2,127,64,false,0);
					NF_PlayRawSound(3,127,64,false,0);
				}
			}
		// if (mode == SPLASH_MODE_1) {
		// 	if (splash_state_0 == SPRITE_NOT_CREATED) {
		// 		NF_CreateTiledBg(1,0,"credit");
		// 		splash_state_0 = SPRITE_CREATED;
		// 	}

		// 	if (splash_state_1 == SPRITE_NOT_CREATED) {
		// 		NF_CreateTiledBg(0,1,"interscope");
		// 		splash_state_1 = SPRITE_CREATED;
		// 	}
		// }
		// else if (mode == SPLASH_MODE_2) {
		// 	if (splash_state_2 == SPRITE_NOT_CREATED) {
		// 		NF_CreateTiledBg(0,1,"deathrow");
		// 		splash_state_2 = SPRITE_CREATED;
		// 	}
		// }
		// else if (mode == SPLASH_MODE_3) {
		// 	if (splash_state_3 == SPRITE_NOT_CREATED) {
		// 		NF_CreateTiledBg(0,1,"amaru");
		// 		splash_state_3 = SPRITE_CREATED;
		// 	}
		// }
		if (gState == MENU_MODE) {

			NF_SetTextColor(0,1,1);
			NF_WriteText16(0,1,4,2,"play");
			NF_WriteText16(0,1,4,4,"tracklist");
			NF_WriteText16(0,1,4,6,"digital booklet");
			NF_WriteText16(0,1,4,8,"credits");
			NF_WriteText16(0,1,cursor_y_pos[cursor_index][0],cursor_y_pos[cursor_index][1],"<");

			// if (checkStylusPosMenu(stylus,0,0) && (KEY_TOUCH & keysDown())) {
			// 	//NF_DeleteTiledBg(1,0);
			// 	//NF_DeleteTiledBg(0,2);
			// 	//NF_UnloadTiledBg("album_art");
			// 	//NF_UnloadTiledBg("pac_2");
			// 	//menuBkgState = SPRITE_NOT_CREATED;
			// 	NF_ClearTextLayer16(0,1);
			// 	mode = ALBUM_MODE;
			// }
		}
		if (gState == INSTRUCT_ALBUM_MODE) {
			NF_WriteText16(0,1,1,3,"Press LEFT - previous song");
			NF_WriteText16(0,1,1,4,"Press RIGHT - next song");
			NF_WriteText16(0,1,1,5,"Press L - previous song");
			NF_WriteText16(0,1,1,6,"Press R - next song");

			NF_WriteText16(0,1,12,9,"Page 1/2");
			NF_WriteText16(0,1,1,11,"-Press any button to continue-");

		}
		if (gState == INSTRUCT_ALBUM_MODE_2) {
			NF_WriteText16(0,1,1,3,"Touch LEFT - previous song");
			NF_WriteText16(0,1,1,4,"Touch RIGHT - next song");
			NF_WriteText16(0,1,1,5,"Press X to return to menu");
			NF_WriteText16(0,1,12,9,"Page 2/2");
			NF_WriteText16(0,1,1,11,"-Press any button to continue-");
		}
		if (gState == ALBUM_MODE) {
			x += 1;
			//NF_SetTextColor(0,1,1);
			NF_WriteText16(0,1,1,2,musicTitleList[player.songId][0]);
			NF_WriteText16(0,1,1,9,musicTitleList[player.songId][1]);
			NF_ScrollBg(0,1,x,0);
			// If the sprite state is saying that sprites are not created, we created the backgrounds for the top and bottom screen, we draw the icons, and the cover art.
			// We also set the sprite state to say that the sprites are created. They are loaded into memory, and do not need to be overwritten every vblank.
			if (sprState == SPRITE_NOT_CREATED) {
				//chooseCoverArtID(coverArt.x,coverArt.y,player.songId,coverArt.id);
				//NF_CreateTiledBg(1,1,"default_screen");
				//NF_CreateTiledBg(0,2,"makaveli");
				drawSprites(left.x,right.x,0,left.y,right.y,0,player.songId);
				drawCoverArt(coverArt.x,coverArt.y,coverArt.id);
				sprState = SPRITE_CREATED;
			}

			// If we check the stylus position, and it is on the right positon, and we touch the icon, we move to the next song.
			if ((checkStylusPos(stylus,right.x,right.y) && (KEY_TOUCH & keysDown())) || (keysDown() & KEY_RIGHT) || (keysDown() & KEY_R)) {
				NF_PlayRawSound(2,127,64,false,0);
				NF_PlayRawSound(3,127,64,false,0);
				// If the song id is less than the amount of songs in the album, we stop the timer used to play the song, and set the player.PLAYING flag into false.
				if (player.songId < MUSIC_SIZE - 1) {
					NF_ClearTextLayer16(0,1);
					//NF_ClearTextLayer16(0,2);
					player.PLAYING = false;
					// stop timer
					timerStop(3);
					// Unload the raw sound at the sound id.
					NF_UnloadRawSound(player.id);
					// Kill the sound at the channel_id temporarily.
					soundKill(channel_id);
					// Reset the play index to 0.
					player.playIndex = 0;
					// Increment the song id to move onto the next song.
					player.songId++;
					// Copy the array in the player's array bank at index player song id, and copy it to the player's song Array, overwriting previous contents.
					memcpy(&player.songArray,&player.arrayBank[player.songId],sizeof(player.arrayBank[player.songId]));
					// Set player tick to 0
					player.time = 0;
					// Stop timer state
					state = timerState_Stopped;
					// State that the sprite has not been created/
					sprState = SPRITE_NOT_CREATED;
				}
			}
			// If we check the stylus position, and it is on the left positon, and we touch the icon, we move to the previous song.
			else if ((checkStylusPos(stylus,left.x,left.y) && (KEY_TOUCH & keysDown())) || (keysDown() & KEY_LEFT) || (keysDown() & KEY_L)) {
				NF_PlayRawSound(2,127,64,false,0);
				NF_PlayRawSound(3,127,64,false,0);
				// If the song id is greater than o, we stop the timer used to play the song, and set the player.PLAYING flag into false.
				if (player.songId > 0) {
					NF_ClearTextLayer16(0,1);
					//NF_ClearTextLayer16(0,2);
					player.PLAYING = false;
					// stop timer
					timerStop(3);
					// Unload the raw sound at the sound id.
					NF_UnloadRawSound(player.id);
					// Kill the sound at the channel_id temporarily.
					soundKill(channel_id);
					// Reset the play index to 0.
					player.playIndex = 0;
					// Decrement the song id to move onto the previous song.
					player.songId--;
					// Copy the array in the player's array bank at index player song id, and copy it to the player's song Array, overwriting previous contents.
					memcpy(&player.songArray,&player.arrayBank[player.songId],sizeof(player.arrayBank[player.songId]));
					// Set player tick to 0
					player.time = 0;
					// Stop timer state
					state = timerState_Stopped;
					// State that the sprite has not been created
					sprState = SPRITE_NOT_CREATED;
				}
			}
			// If the player is not playing, we load the player's song array segment at index player.play Index, and set playing to true.
			if (!player.PLAYING)
			{
				if (aState == ALBUM_PLAYING) 
					NF_LoadRawSound(player.songArray[player.playIndex],player.id,TUNE_FREQ,SAMPLE_RATE);
				player.PLAYING = true;

				if (aState == ALBUM_NOT_PLAYING)
					aState = ALBUM_PLAYING;
			}
		}
		// Scan the keys
		scanKeys();
		// Read the touch screen with the stylus
		touchRead(&stylus);
		// Draw the 3D sprites
		NF_Draw3dSprites();
		glFlush(0);
		// Vblank interrupt
		swiWaitForVBlank();
		// Update 3D Sprites gfx.
		NF_Update3dSpritesGfx();
		// Update Text Layers
		NF_UpdateTextLayers();
	}
	
	return 0;
}
