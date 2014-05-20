/*
Author: Luke Pederson
Title: Main
Abstract: Side scrolling game
ID: 9786
Date: ????
*/

#include <allegro.h>
#include "mappyal.h"

#define WHITE makecol(255,255,255)
#define BLACK makecol(0,0,0)
#define LTGREEN makecol(192,255,192)
#define LTRED makecol(255,192,192)
#define LTBLUE makecol(192,192,255)
#define PINK makecol(255,0,255)

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

typedef struct ITEM{
	int x,y;
	int alive;
	BITMAP * image;
}ITEM;
typedef struct BULLET{
	int alive,dir;
	int x,y;
	int xspd;
	int curframe,maxframe,animdir;
	int framecount,framedelay;
	int width,height;
	int owner;
	int level;
	int damage;

	BITMAP * bullet;
}BULLET;
typedef struct PLAYER{
	int dir, alive, state; //1 - idle, 2 - run, 3 - jump, 4 - shoot, 5 - run/shoot, 6 - jump/shoot, 7 - slide, 8 - slide/shoot
    int x,y;
    int width,height;
    int xspeed,yspeed;
	int xspd_run;
	int jumpcur,jumpmax,jumpcheck;
    int xdelay,ydelay;
    int xcount,ycount;
    int curframe,maxframe,animdir;
    int framecount,framedelay;
	int numbullets;
	int bulletdelay,bulletdelay_counter;
	int charging,charge1,charge2;
	int bulletspeed;
	int charge_isplaying;
	int health, health_max;
	int lives;
	int slidespeed,slidecounter,slidemax;
	int slidedelay,slidedelaycounter;
	int shootcounter,shootdelay;

	BITMAP * image;
	BITMAP * idle[3];
	BITMAP * run[10];
	BITMAP * jump[7];
	BITMAP * slide[2];
	BITMAP * slide_shoot[2];
	BITMAP * idle_shoot[2];
	BITMAP * run_shoot[9];
	BITMAP * jump_shoot[7];
	BITMAP * bullet_1;
	BITMAP * bullet_2[7];
	BITMAP * bullet_3[11];
	BITMAP * death[11];
	BITMAP * healthbar;
	BITMAP * livesbar;

	BULLET * bullets[10];

	int level1_spawnx;
	int level1_spawny;
}PLAYER;
typedef struct ENEMY{
	int dir, alive;
    int x,y;
    int width,height;
    int xspeed,yspeed;
    int xdelay,ydelay;
    int xcount,ycount;
    int curframe,maxframe,animdir;
    int framecount,framedelay;
	int movedir,movespeed;
	int movedelay,movedelaymax,movecounter;
	int numbullets;
	int dropspeed;
	int bulletdelay,bulletdelay_counter;
	int bulletdelay_max;
	int bulletspeed;
	int health;

	BITMAP * image;
	BITMAP * bmp[10];
	BULLET * bullets[5];

}ENEMY;
typedef struct BOSS{
	int dir, alive;
    int x,y;
    int width,height;
    int xspeed,yspeed;
    int xdelay,ydelay;
    int xcount,ycount;
    int curframe,maxframe,animdir;
    int framecount,framedelay;
	int movedir,movespeed;
	int movedelay,movedelaymax,movecounter;
	int numbullets;
	int dropspeed;
	int bulletdelay,bulletdelay_counter;
	int bulletdelay_max;
	int bulletspeed;
	int health;

	BITMAP * image;
	BITMAP * bmp[10];
	BITMAP * bullet1[3];
	BULLET * bullets[5];

}BOSS;

//Game Variables
int scrollx;
int scrolly;
int oldpx, oldpy, olds, oldf;
int i,j,k; //Random counters
int showgameinfo = -1;
int showgameinfo_delay = 10, showgameinfo_counter = 0;
int pause_delay = 10, pause_counter = 0;
volatile int clock_count = 0;
volatile int fps = 0;
BITMAP * buffer;
BITMAP * player_sheet;
BITMAP * enemy_sheet;
BITMAP * title_screen;
BITMAP * temp;
PLAYER * player;
ITEM * healthpickup[5];
ITEM * extralifepickup[3];
BLKSTR * blockdata;
SAMPLE * blaster1;
SAMPLE * blaster2;
SAMPLE * blaster3;
SAMPLE * blaster_charge;
SAMPLE * level1_bg;
SAMPLE * level2_bg;
SAMPLE * playerdeath;
SAMPLE * credits;
SAMPLE * itempickedup;
SAMPLE * win;

//Level 1 enemies
ENEMY * enemy_level1_group1[10];
ENEMY * enemy_level1_group2[10];
ENEMY * enemy_level1_group3[20];
ENEMY * enemy_level1_bossgroup[20];
BOSS * level1_boss;
int enemy_level1_flag1;
int enemy_level1_flag2;
int enemy_level1_flag3;
int enemy_level1_bossflag;

//Functions
void loadFiles(int action);
void loadAnimations();
void loadLevel1Boss();
void updateEnemies();
void updateBoss(int level);
void loadL1Enemies();
void getInput();
void playershoot();
int playerdie();
void pauseGame();
void pickupItems();
void enemyshoot(ENEMY * e);
void loadenemy(ENEMY * e, int type, int x, int y);
void checkBullets();
void checkLevel1BossBullets();
void playCredits();
BITMAP *grabframe(BITMAP *source, int width, int height, int startx, int starty, int columns, int frame){
    BITMAP *temp = create_bitmap(width,height);
    int x = startx + (frame % columns) * width;
    int y = starty + (frame / columns) * height;
    blit(source,temp,x,y,0,0,width,height);
    return temp;
}
int collided(int x, int y){
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return (blockdata->tl);
}
int inside(int x, int y,int x1,int y1,int x2, int y2){
	if(x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return 1;
	return 0;
}
void clock_handle(void){
	clock_count++;
}END_OF_FUNCTION(clock_handle);

int main(){
	int start = clock(),ticks=0,fps=0;
	// Initialize Allegro
	allegro_init();
	set_color_depth(16);
	install_keyboard();
	install_joystick(JOY_TYPE_AUTODETECT);
	install_timer();
	srand(time(NULL));
	set_gfx_mode(GFX_SAFE,SCREEN_WIDTH,SCREEN_HEIGHT,0,0);
	
	//install a digital sound driver
    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "") != 0) 
    {
        allegro_message("Error initializing sound system");
        return 1;
    }

	//Initialize Game
	loadFiles(1);
	MapLoad("level1.FMP");
	loadAnimations();
	loadL1Enemies();
	loadLevel1Boss();
	play_sample(level1_bg,128,128,1000,1);
	LOCK_VARIABLE(clock_count);
	LOCK_FUNCTION(clock_handle);
	install_int(clock_handle,1000);

	//Title Screen
	while(!keypressed()){
		stretch_blit(title_screen,buffer,0,0,title_screen->w,title_screen->h,0,0,buffer->w,buffer->h);
		blit(buffer,screen,0,0,0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1);

		poll_joystick();
		if(joy[0].button[11].b)
			break;
	}
	//player->x = 400 * 16;
	//player->y = 52 * 16;
	// Run Game
	while(!key[KEY_ESC] && player->lives > 0)
	{  
		poll_joystick();
		//calculate framerate once per second
		ticks++;
        if (clock() > start + CLOCKS_PER_SEC){
            start = clock();
            fps = ticks;
            ticks = 0;
        }

		//Check for pause
		pauseGame();

		//update the map scroll position
		scrollx = player->x + player->width/2 - SCREEN_WIDTH/4;
		scrolly = player->y + player->height/2 - SCREEN_HEIGHT + 130;

		//avoid moving beyond the map edge
		if (scrollx < 0) scrollx = 0;
		if (scrollx > (mapwidth * mapblockwidth - SCREEN_WIDTH))
            scrollx = mapwidth * mapblockwidth - SCREEN_WIDTH;
		if (scrolly < 0) scrolly = 0;
		if (scrolly > (mapheight * mapblockheight - SCREEN_HEIGHT)) 
            scrolly = mapheight * mapblockheight - SCREEN_HEIGHT;
		
		MapDrawBG(buffer,scrollx,scrolly,0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1);
		MapDrawFG(buffer,scrollx,scrolly,0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1,0);
		
		if (player->dir) 
            draw_sprite(buffer, player->image, player->x-scrollx, player->y-scrolly+1);
		else 
            draw_sprite_h_flip(buffer, player->image, player->x-scrollx, player->y-scrolly+1);
		
		if(!playerdie()){
			getInput();
			playershoot();
			checkBullets();

			//Checkpoint
			if(player->x > 262 * 16){
				player->level1_spawnx = 262 * 16;
				player->level1_spawny = 57 * 16;
			}
			//Checkpoint 2 - Boss
			if(player->x > 400 * 16){
				player->level1_spawnx = 400 * 16;
				player->level1_spawny = 52 * 16;
			}
			if(player->x > 428 * 16 && player->y > 65 * 16){ //Boss area
				//player->health = 100;
				updateBoss(1);
				checkLevel1BossBullets();
				j = 1;
				for(i=0;i<20;i++)
					if(enemy_level1_bossgroup[i]->alive)
						j = 0;
				
				if(j == 1){
					play_sample(win,128,128,1000,0);
					stop_sample(level1_bg);
					rest(500);
					break;
				}
			}
		}
		updateEnemies(1);
		pickupItems();
		
		//Game Info
		textprintf(buffer,font,5,5,WHITE,"Time: %d",clock_count);
		textprintf(buffer,font,5,20,WHITE,"Health:");
		for(i=0;i<player->health/10;i++)
			draw_sprite(buffer,player->healthbar,70+(i*10),18);
		textprintf(buffer,font,80,5,WHITE,"Lives:");
		for(i=0;i<player->lives;i++)
			draw_sprite(buffer,player->livesbar,130+(i*10),3);

		//Debugging Info
		if(showgameinfo > 0){
			textprintf(buffer,font,5,15,WHITE,"X: %d Y: %d --- Bx: %d By: %d",player->x,player->y,player->bullets[0]->x,player->bullets[0]->y);
			textprintf(buffer,font,5,25,WHITE,"JUMP: %d --- Charge: %d",player->jumpcur,player->charging); 
			textprintf(buffer,font,5,35,WHITE,"FPS: %d",fps);
			textprintf(buffer,font,100,35,WHITE,"Scrollx: %i Scrolly: %i",scrollx,scrolly);
			textprintf(buffer,font,100,45,WHITE,"Health: %d State: %d",player->health,player->state);
			textprintf(buffer,font,100,55,WHITE,"Lives: %d",player->lives);
			textprintf(buffer,font,300,5,WHITE,"%d %d",player->slidecounter,player->slidemax);
		}

		//Update Screen
		acquire_screen();
		blit(buffer,screen, 0, 0, 0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
		release_screen();
		rest(30);
	}
	rest(400);
	clear_bitmap(buffer);
	playCredits();
	loadFiles(0);
	allegro_exit();
	return 0;
}
END_OF_MAIN()

void loadFiles(int action){
	int i;
	if(action){
		//Bitmaps
		player_sheet = load_bitmap("playersheet.bmp",NULL);
		if(!player_sheet){
			allegro_message("Unable to load playersheet");
			exit(1);
		}
		enemy_sheet = load_bitmap("enemysheet.bmp",NULL);
		if(!enemy_sheet){
			allegro_message("Unable to load enemysheet");
			exit(1);
		}
		title_screen = load_bitmap("titlescreen.bmp",NULL);
		if(!title_screen){
			allegro_message("Unable to load titlescreen");
			exit(1);
		}
		buffer = create_bitmap(SCREEN_WIDTH,SCREEN_HEIGHT);
		if(!buffer){
			allegro_message("Unable to create buffer bitmap");
			exit(1);
		}
		clear(buffer);

		//Sounds
		blaster1 = load_sample("blaster1.wav");
		if(!blaster1){
			allegro_message("Unable to load blaster1.wav");
			exit(1);
		}
		blaster2 = load_sample("button-7.wav");
		if(!blaster2){
			allegro_message("Unable to load blaster wav");
			exit(1);
		}
		blaster3 = load_sample("button-7.wav");
		if(!blaster3){
			allegro_message("Unable to load blaster wav");
			exit(1);
		}
		blaster_charge = load_sample("blaster_charge.wav");
		if(!blaster_charge){
			allegro_message("Unable to load blaster charge");
			exit(1);
		}

		level1_bg = load_sample("flame-mammoth.wav");
		if(!level1_bg){
			allegro_message("Unable to load flame-mammoth.wav");
			exit(1);
		}
		playerdeath = load_sample("playerdeath.wav");
		if(!playerdeath){
			allegro_message("Unable to load playerdeath.wav");
			exit(1);
		}
		itempickedup = load_sample("itempickedup.wav");
		if(!itempickedup){
			allegro_message("Unable to load itempickedup.wav");
			exit(1);
		}
		win = load_sample("win.wav");
		if(!win){
			allegro_message("Unable to load win.wav");
			exit(1);
		}
	}
	else
	{
		//Player
		for(i=0;i<3;i++)
			destroy_bitmap(player->idle[i]);
		for(i=0;i<2;i++)
			destroy_bitmap(player->idle_shoot[i]);
		for(i=0;i<10;i++)
			destroy_bitmap(player->run[i]);
		for(i=0;i<10;i++)
			destroy_bitmap(player->run_shoot[i]);
		for(i=0;i<7;i++)
			destroy_bitmap(player->jump[i]);exit(1);
		for(i=0;i<7;i++)
			destroy_bitmap(player->jump_shoot[i]);
		allegro_message("Here");
		for(i=0;i<10;i++){
			destroy_bitmap(player->bullets[i]->bullet);	
			free(player->bullets[i]);
		}
		destroy_bitmap(player->image);
		destroy_bitmap(player->bullet_1);
		for(i=0;i<7;i++)
			destroy_bitmap(player->bullet_2[i]);
		for(i=0;i<11;i++)
			destroy_bitmap(player->bullet_3[i]);
		free(player);

		//Enemies
		for(i=0;i<sizeof(enemy_level1_group1)/sizeof(enemy_level1_group1[0]);i++){
			for(j=0;j<sizeof(enemy_level1_group1[i]->bullets)/sizeof(enemy_level1_group1[i]->bullets[0]);j++){
				destroy_bitmap(enemy_level1_group1[i]->bullets[j]->bullet);
				free(enemy_level1_group1[i]->bullets[j]);
			}
			destroy_bitmap(enemy_level1_group1[i]->image);
			for(j=0;j<10;j++)
				destroy_bitmap(enemy_level1_group1[i]->bmp[j]);
			free(enemy_level1_group1[i]);
		}
		for(i=0;i<sizeof(enemy_level1_group2)/sizeof(enemy_level1_group2[0]);i++){
			for(j=0;j<sizeof(enemy_level1_group2[i]->bullets)/sizeof(enemy_level1_group2[i]->bullets[0]);j++)
				free(enemy_level1_group2[i]->bullets[j]);
			free(enemy_level1_group2[i]);
		}
		for(i=0;i<sizeof(enemy_level1_group3)/sizeof(enemy_level1_group3[0]);i++){
			for(j=0;j<sizeof(enemy_level1_group3[i]->bullets)/sizeof(enemy_level1_group3[i]->bullets[0]);j++)
				free(enemy_level1_group3[i]->bullets[j]);
			free(enemy_level1_group3[i]);
		}

		destroy_bitmap(enemy_sheet);
		destroy_bitmap(title_screen);
		destroy_bitmap(buffer);
		MapFreeMem();
		
		//Sound
		destroy_sample(blaster1);
		destroy_sample(blaster2);
		destroy_sample(blaster3);
		destroy_sample(level1_bg);
		destroy_sample(blaster_charge);
		destroy_sample(itempickedup);

		//Items
		for(i=0;i<5;i++){
			destroy_bitmap(healthpickup[i]->image);
			free(healthpickup[i]);
		}
		for(i=0;i<3;i++){
			destroy_bitmap(extralifepickup[i]->image);
			free(extralifepickup[i]);
		}
	}
}
void loadAnimations(){
	player = malloc(sizeof(PLAYER));

	//Run
	player->run[0] = grabframe(player_sheet,32,50,318,3,1,0);
	player->run[1] = grabframe(player_sheet,21,50,350,3,1,0);
	player->run[2] = grabframe(player_sheet,24,50,370,3,1,0);
	player->run[3] = grabframe(player_sheet,32,50,393,3,1,0);
	player->run[3] = grabframe(player_sheet,34,50,426,3,1,0);
	player->run[4] = grabframe(player_sheet,25,50,460,3,1,0);
	player->run[5] = grabframe(player_sheet,25,50,486,3,1,0);
	player->run[6] = grabframe(player_sheet,25,50,512,3,1,0);
	player->run[7] = grabframe(player_sheet,32,50,537,3,1,0);
	player->run[8] = grabframe(player_sheet,33,50,570,3,1,0);
	player->run[9] = grabframe(player_sheet,30,50,605,3,1,0);

	//Run + Shoot
	player->run_shoot[0] = grabframe(player_sheet,33,50,286,56,1,0);
	player->run_shoot[1] = grabframe(player_sheet,32,50,319,56,1,0);
	player->run_shoot[2] = grabframe(player_sheet,35,50,351,56,1,0);
	player->run_shoot[3] = grabframe(player_sheet,39,50,386,56,1,0);
	player->run_shoot[3] = grabframe(player_sheet,35,50,425,56,1,0);
	player->run_shoot[4] = grabframe(player_sheet,32,50,460,56,1,0);
	player->run_shoot[5] = grabframe(player_sheet,32,50,492,56,1,0);
	player->run_shoot[6] = grabframe(player_sheet,35,50,524,56,1,0);
	player->run_shoot[7] = grabframe(player_sheet,37,50,559,56,1,0);
	player->run_shoot[8] = grabframe(player_sheet,36,50,596,56,1,0);

	//Jump
	player->jump[0] = grabframe(player_sheet,25,50,4,60,1,0);
	player->jump[1] = grabframe(player_sheet,21,50,32,60,1,0);
	player->jump[2] = grabframe(player_sheet,21,50,52,60,1,0);
	player->jump[3] = grabframe(player_sheet,25,50,75,60,1,0);
	player->jump[4] = grabframe(player_sheet,31,50,100,60,1,0);
	player->jump[5] = grabframe(player_sheet,30,50,130,60,1,0);
	player->jump[6] = grabframe(player_sheet,32,50,160,60,1,0);

	//Jump + Shoot
	player->jump_shoot[0] = grabframe(player_sheet,31,50,4,110,1,0);
	player->jump_shoot[1] = grabframe(player_sheet,25,50,35,110,1,0);
	player->jump_shoot[2] = grabframe(player_sheet,27,50,60,110,1,0);
	player->jump_shoot[3] = grabframe(player_sheet,33,50,87,110,1,0);
	player->jump_shoot[4] = grabframe(player_sheet,31,50,120,110,1,0);
	player->jump_shoot[5] = grabframe(player_sheet,30,50,151,110,1,0);
	player->jump_shoot[6] = grabframe(player_sheet,39,50,181,110,1,0);

	//Slide
	player->slide[0] = grabframe(player_sheet,36,50,280,110,1,0);
	player->slide[1] = grabframe(player_sheet,40,50,317,110,1,0);
	
	//Slide + Shoot
	player->slide_shoot[0] = grabframe(player_sheet,40,50,365,110,1,0);
	player->slide_shoot[1] = grabframe(player_sheet,52,50,408,110,1,0);

	//Idle
	player->idle[0] = grabframe(player_sheet,30,50,215,3,1,0);
	player->idle[1] = grabframe(player_sheet,30,50,249,3,1,0);
	player->idle[2] = grabframe(player_sheet,30,50,283,3,1,0);

	//Idle + Shoot
	player->idle_shoot[0] = grabframe(player_sheet,30,50,5,152,1,0);
	player->idle_shoot[1] = grabframe(player_sheet,34,50,38,152,1,0);
	//Fix player_sheet lameness
	rectfill(player->idle_shoot[0],0,0,player->idle_shoot[0]->w,15,PINK);
	rectfill(player->idle_shoot[1],0,0,player->idle_shoot[1]->w,15,PINK);

	//Death
	player->death[0] = grabframe(player_sheet,30,50,7,306,1,0);
	player->death[1] = grabframe(player_sheet,31,50,38,306,1,0);
	player->death[2] = grabframe(player_sheet,32,50,70,306,1,0);
	player->death[3] = grabframe(player_sheet,35,50,103,306,1,0);
	player->death[4] = grabframe(player_sheet,30,50,138,306,1,0);
	player->death[5] = grabframe(player_sheet,34,50,170,306,1,0);
	player->death[6] = grabframe(player_sheet,33,50,205,306,1,0);
	player->death[7] = grabframe(player_sheet,35,50,238,306,1,0);
	player->death[8] = grabframe(player_sheet,34,50,273,306,1,0);
	player->death[9] = grabframe(player_sheet,30,50,308,306,1,0);
	player->death[10] = grabframe(player_sheet,28,50,340,306,1,0);

	//Bullet - Level 1
	player->bullet_1 = grabframe(player_sheet,12,12,3,375,1,0);

	player->bullet_2[0] = grabframe(player_sheet,15,30,25,365,1,0);
	player->bullet_2[1] = grabframe(player_sheet,30,30,45,365,1,0);
	player->bullet_2[2] = grabframe(player_sheet,30,30,80,365,1,0);
	player->bullet_2[3] = grabframe(player_sheet,40,30,115,365,1,0);
	player->bullet_2[4] = grabframe(player_sheet,40,30,160,365,1,0);
	player->bullet_2[5] = grabframe(player_sheet,50,30,210,365,1,0);
	player->bullet_2[6] = grabframe(player_sheet,45,30,260,365,1,0);

	player->bullet_3[0] = grabframe(player_sheet,20,35,2,400,1,0);
	player->bullet_3[1] = grabframe(player_sheet,30,35,25,400,1,0);
	player->bullet_3[2] = grabframe(player_sheet,30,35,60,400,1,0);
	player->bullet_3[3] = grabframe(player_sheet,45,35,100,400,1,0);
	player->bullet_3[4] = grabframe(player_sheet,35,35,150,400,1,0);
	player->bullet_3[5] = grabframe(player_sheet,53,38,22,440,1,0);
	player->bullet_3[6] = grabframe(player_sheet,40,38,80,440,1,0);
	player->bullet_3[7] = grabframe(player_sheet,50,38,130,440,1,0);
	player->bullet_3[8] = grabframe(player_sheet,47,40,34,490,1,0);
	player->bullet_3[9] = grabframe(player_sheet,50,40,85,490,1,0);
	player->bullet_3[10] = grabframe(player_sheet,45,40,145,490,1,0);

	for(i=0;i<5;i++){
		healthpickup[i] = malloc(sizeof(ITEM));
		healthpickup[i]->image = load_bitmap("healthbar.bmp",NULL);
		if(!healthpickup[i]->image){
			allegro_message("Unable to load healthbar.bmp");
			exit(1);
		}
		healthpickup[i]->alive = 0;
	}
	player->healthbar = create_bitmap(12,12);
	stretch_blit(healthpickup[0]->image,player->healthbar,0,0,16,16,0,0,12,12);

	for(i=0;i<3;i++){
		extralifepickup[i] = malloc(sizeof(ITEM));
		extralifepickup[i]->image = load_bitmap("extralife.bmp",NULL);
		if(!extralifepickup[i]->image){
			allegro_message("Unable to load extralife.bmp");
			exit(1);
		}
		extralifepickup[i]->alive = 0;
	}
	player->livesbar = create_bitmap(12,12);
	stretch_blit(extralifepickup[0]->image,player->livesbar,0,0,16,16,0,0,12,12);

	//Init pickups
	healthpickup[0]->x = 259 *16;
	healthpickup[0]->y = 59 *16;
	healthpickup[0]->alive = 1;
	healthpickup[1]->x = 389 *16;
	healthpickup[1]->y = 31 *16;
	healthpickup[1]->alive = 1;
	extralifepickup[0]->x = 162*16;
	extralifepickup[0]->y = 67 *16;
	extralifepickup[0]->alive = 1;
	extralifepickup[1]->x = 252 * 16;
	extralifepickup[1]->y = 31 * 16;
	extralifepickup[1]->alive = 1;
	extralifepickup[2]->x = 391 * 16;
	extralifepickup[2]->y = 13 * 16;
	extralifepickup[2]->alive = 1;

	//Init player
	player->image = player->idle[0];
	player->level1_spawnx = 50;
	player->level1_spawny = 57 * 16;
	player->x = player->level1_spawnx;
	player->y = player->level1_spawny;
	player->bulletdelay = 4;
	player->bulletdelay_counter = 0;
	player->charging = 0;
	player->charge_isplaying = 0;
	player->charge1 = 60; player->charge2 = 140;
	player->bulletspeed = 8;
	player->numbullets = 9;
	player->dir = 1;
	player->xspd_run = 3;
	player->xspeed = player->xspd_run;
	player->jumpcur = 0;
	player->jumpmax = 17;
	player->jumpcheck = player->jumpmax + 1;
    player->curframe=0;
    player->framecount=0;
    player->framedelay=3;
    player->maxframe=2;
    player->width=player->image->w;
    player->height=player->image->h;
	player->health = player->health_max = 100;
	player->lives = 3;
	player->slidecounter = 0;
	player->slidemax = 15;
	player->slidespeed = 6;
	player->slidedelaycounter = 0;
	player->slidedelay = 15;
	player->shootcounter = 0;
	player->shootdelay = 25;

	for(i=0;i<player->numbullets;i++){
		player->bullets[i] = malloc(sizeof(BULLET));
		player->bullets[i]->alive = 0;
		player->bullets[i]->x = 0;
		player->bullets[i]->y = 0;
	}

	destroy_bitmap(player_sheet);
	//allegro_message("Size of Bullet %d: %d",i,sizeof(player->bullets[i]));
}
void loadL1Enemies(){
	for(i=0;i<10;i++){
		enemy_level1_group1[i] = malloc(sizeof(ENEMY));
		enemy_level1_group1[i]->alive = 0;
		for(j=0;j<sizeof(enemy_level1_group1[i]->bullets)/sizeof(enemy_level1_group1[0]->bullets[0]);j++){
			enemy_level1_group1[i]->bullets[j] = malloc(sizeof(BULLET));
		}
	}
	for(i=0;i<10;i++){
		enemy_level1_group2[i] = malloc(sizeof(ENEMY));
		enemy_level1_group2[i]->alive = 0;
		for(j=0;j<sizeof(enemy_level1_group2[i]->bullets)/sizeof(enemy_level1_group2[0]->bullets[0]);j++){
			enemy_level1_group2[i]->bullets[j] = malloc(sizeof(BULLET));
		}
	}
	for(i=0;i<20;i++){
		enemy_level1_group3[i] = malloc(sizeof(ENEMY));
		enemy_level1_group3[i]->alive = 0;
		for(j=0;j<sizeof(enemy_level1_group3[i]->bullets)/sizeof(enemy_level1_group3[0]->bullets[0]);j++){
			enemy_level1_group3[i]->bullets[j] = malloc(sizeof(BULLET));
		}
	}

	//Place by mappy block x - y

	//Top - flyers
	loadenemy(enemy_level1_group1[0],1,87,53);
	loadenemy(enemy_level1_group1[1],1,91,50);
	loadenemy(enemy_level1_group1[2],2,118,35);
	loadenemy(enemy_level1_group1[3],1,125,39);
	loadenemy(enemy_level1_group1[4],1,164,30);
	loadenemy(enemy_level1_group1[5],3,185,128);
	loadenemy(enemy_level1_group1[6],1,149,33);
	loadenemy(enemy_level1_group1[7],1,205,31);
	loadenemy(enemy_level1_group1[8],1,185,48);
	loadenemy(enemy_level1_group1[9],2,179,44);

	//Bottom - grounded
	loadenemy(enemy_level1_group2[0],1,71,87);
	loadenemy(enemy_level1_group2[1],1,81,87);
	loadenemy(enemy_level1_group2[2],2,127,77);
	loadenemy(enemy_level1_group2[3],3,98,82);
	loadenemy(enemy_level1_group2[4],1,173,79);
	loadenemy(enemy_level1_group2[5],1,131,85);
	loadenemy(enemy_level1_group2[6],1,193,79);
	loadenemy(enemy_level1_group2[7],1,236,79);
	loadenemy(enemy_level1_group2[8],3,237,67);
	loadenemy(enemy_level1_group2[9],1,218,63);

	//Flag3 --- Checkpoint
	loadenemy(enemy_level1_group3[0],1,292,51);
	loadenemy(enemy_level1_group3[1],3,300,49);
	loadenemy(enemy_level1_group3[2],1,302,53);
	loadenemy(enemy_level1_group3[3],1,320,59);
	loadenemy(enemy_level1_group3[4],1,338,59);
	loadenemy(enemy_level1_group3[5],2,333,57);
	loadenemy(enemy_level1_group3[6],1,346,56);
	loadenemy(enemy_level1_group3[7],1,356,59);
	loadenemy(enemy_level1_group3[8],3,346,47);
	loadenemy(enemy_level1_group3[9],1,360,46);
	loadenemy(enemy_level1_group3[10],1,350,40);
	loadenemy(enemy_level1_group3[11],1,380,31);
	loadenemy(enemy_level1_group3[12],3,378,29);
	loadenemy(enemy_level1_group3[13],2,384,30);
	loadenemy(enemy_level1_group3[14],3,313,50);
	loadenemy(enemy_level1_group3[15],1,280,36);
	loadenemy(enemy_level1_group3[16],1,149,33);
	loadenemy(enemy_level1_group3[17],1,205,31);
	loadenemy(enemy_level1_group3[18],1,185,48);
	loadenemy(enemy_level1_group3[19],2,179,44);

	//allegro_message("%d %d %d",enemy_level1_group1[0]->bullets[0]->alive,enemy_level1_group1[0]->bullets[0]->framedelay);
}
void loadLevel1Boss(){
	for(i=0;i<20;i++){
		enemy_level1_bossgroup[i] = malloc(sizeof(ENEMY));
		enemy_level1_bossgroup[i]->alive = 0;
		for(j=0;j<sizeof(enemy_level1_bossgroup[i]->bullets)/sizeof(enemy_level1_bossgroup[0]->bullets[0]);j++){
			enemy_level1_bossgroup[i]->bullets[j] = malloc(sizeof(BULLET));
		}
	}
	
	//Spawn Crapload of enemies - DIE!!!!
	loadenemy(enemy_level1_bossgroup[0],1,441,42);
	loadenemy(enemy_level1_bossgroup[1],1,445,42);
	loadenemy(enemy_level1_bossgroup[2],1,437,44);
	loadenemy(enemy_level1_bossgroup[3],1,449,44);
	loadenemy(enemy_level1_bossgroup[4],1,435,47);
	loadenemy(enemy_level1_bossgroup[5],1,453,47);
	loadenemy(enemy_level1_bossgroup[6],1,437,50);
	loadenemy(enemy_level1_bossgroup[7],1,450,50);
	loadenemy(enemy_level1_bossgroup[8],1,444,47);
	loadenemy(enemy_level1_bossgroup[9],1,440,56);
	loadenemy(enemy_level1_bossgroup[10],1,450,58);
	loadenemy(enemy_level1_bossgroup[11],3,431,68);
	loadenemy(enemy_level1_bossgroup[12],2,432,70);
	loadenemy(enemy_level1_bossgroup[13],2,454,70);
	loadenemy(enemy_level1_bossgroup[14],3,431,68);
	loadenemy(enemy_level1_bossgroup[15],1,435,72);
	loadenemy(enemy_level1_bossgroup[16],1,452,72);
	loadenemy(enemy_level1_bossgroup[17],1,440,52);
	loadenemy(enemy_level1_bossgroup[18],1,446,54);
	loadenemy(enemy_level1_bossgroup[19],2,448,51);

	/*
	level1_boss = malloc(sizeof(BOSS));

	level1_boss->bmp[0] = grabframe(enemy_sheet,27,74,0,377,1,0);
	level1_boss->bmp[1] = grabframe(enemy_sheet,28,74,27,377,1,0);
	level1_boss->bmp[2] = grabframe(enemy_sheet,26,74,55,377,1,0);
	level1_boss->bmp[3] = grabframe(enemy_sheet,25,74,81,377,1,0);
	level1_boss->bmp[4] = grabframe(enemy_sheet,31,74,106,377,1,0);
	level1_boss->bmp[5] = grabframe(enemy_sheet,33,74,137,377,1,0);

	level1_boss->bullets[0]->bullet = grabframe(enemy_sheet,15,15,170,385,1,0);
	level1_boss->bullets[1]->bullet = grabframe(enemy_sheet,15,15,185,385,1,0);
	level1_boss->bullets[2]->bullet = grabframe(enemy_sheet,15,15,200,385,1,0);

	level1_boss->x = 456 * 16;
	level1_boss->y = 71 * 16;
	level1_boss->curframe = 0;
	level1_boss->framecount = 0;
	level1_boss->framedelay = 3;
	level1_boss->maxframe = 4;
	level1_boss->movecounter = 0;
	level1_boss->movedelay = 30;
	level1_boss->movedelaymax = level1_boss->movedelay;
	level1_boss->movedir = 0;
	level1_boss->movespeed = 1;
	level1_boss->numbullets = 5;
	level1_boss->bulletdelay = 20;
	level1_boss->bulletdelay_max = 20;
	level1_boss->bulletdelay_counter = 0;
	level1_boss->bulletspeed = 6;
	level1_boss->health = 1000;
	level1_boss->alive = 1;
	level1_boss->dropspeed = 2;
	level1_boss->image = level1_boss->bmp[0];
	*/
}
void updateEnemies(int level){
	if(enemy_level1_flag1 == 0 && player->x > 65 * 16 && player->y < 65 * 16 && player->x < 262*16)
		enemy_level1_flag1 = 1;
	if( player->x > 262*16)
		enemy_level1_flag2 = 3;

	if(enemy_level1_flag2 == 0 && player->x > 57 * 16 && player->y > 65 * 16 && player->x < 262*16)
		enemy_level1_flag2 = 1;
	if( player->x > 262*16)
		enemy_level1_flag2 = 3;

	if(enemy_level1_flag3 == 0 && player->x > 262*16)
		enemy_level1_flag3 = 1;
	if( player->x > 394*49)
		enemy_level1_flag3 = 3;

	if(enemy_level1_flag1 == 1){
		//int i;
		for(i=0;i<10;i++){
			if(enemy_level1_group1[i]->alive && inside(enemy_level1_group1[i]->x,enemy_level1_group1[i]->y,scrollx,scrolly,scrollx+SCREEN_WIDTH,scrolly+SCREEN_HEIGHT)){
				if(player->x < enemy_level1_group1[i]->x)
					enemy_level1_group1[i]->dir = 0;
				else
					enemy_level1_group1[i]->dir = 1;
				
				if (++enemy_level1_group1[i]->framecount > enemy_level1_group1[i]->framedelay){
					enemy_level1_group1[i]->framecount=0;
					if (++enemy_level1_group1[i]->curframe > enemy_level1_group1[i]->maxframe)
						enemy_level1_group1[i]->curframe=0;
				}
				
				//Move
				if(enemy_level1_group1[i]->movecounter < enemy_level1_group1[i]->movedelay){
					enemy_level1_group1[i]->movecounter++;
					if(enemy_level1_group1[i]->movedir)
						enemy_level1_group1[i]->x += enemy_level1_group1[i]->movespeed;
					else
						enemy_level1_group1[i]->x -= enemy_level1_group1[i]->movespeed;
					
					if(enemy_level1_group1[i]->movedir){
						if(collided(enemy_level1_group1[i]->x + enemy_level1_group1[i]->image->w,enemy_level1_group1[i]->y) ||
							collided(enemy_level1_group1[i]->x + enemy_level1_group1[i]->image->w,enemy_level1_group1[i]->y + enemy_level1_group1[i]->image->h))
							enemy_level1_group1[i]->x -= enemy_level1_group1[i]->movespeed * 2;
					}else{
						if(collided(enemy_level1_group1[i]->x,enemy_level1_group1[i]->y) ||
							collided(enemy_level1_group1[i]->x,enemy_level1_group1[i]->y + enemy_level1_group1[i]->image->h))
							enemy_level1_group1[i]->x += enemy_level1_group1[i]->movespeed * 2;
					}

					if(enemy_level1_group1[i]->movecounter >= enemy_level1_group1[i]->movedelay){
						enemy_level1_group1[i]->movecounter = 0;
						enemy_level1_group1[i]->movedelay = rand() % enemy_level1_group1[i]->movedelaymax + 1;
						enemy_level1_group1[i]->movedir = rand() % 2;
						enemy_level1_group1[i]->movespeed = (rand() % 2) + 1;
					}
				}
				
				enemy_level1_group1[i]->image = enemy_level1_group1[i]->bmp[enemy_level1_group1[i]->curframe];
				enemy_level1_group1[i]->width = enemy_level1_group1[i]->image->w;
				enemy_level1_group1[i]->height = enemy_level1_group1[i]->image->h;
				
				if(!collided(enemy_level1_group1[i]->x + enemy_level1_group1[i]->image->w/2,enemy_level1_group1[i]->y + enemy_level1_group1[i]->image->h + enemy_level1_group1[i]->dropspeed))
					enemy_level1_group1[i]->y += enemy_level1_group1[i]->dropspeed;
				
				if(enemy_level1_group1[i]->x > scrollx - 20 && enemy_level1_group1[i]->x < scrollx + SCREEN_WIDTH + 20){
					if(!enemy_level1_group1[i]->dir)
						draw_sprite(buffer,enemy_level1_group1[i]->image,enemy_level1_group1[i]->x-scrollx,enemy_level1_group1[i]->y-scrolly);
					else
						draw_sprite_h_flip(buffer,enemy_level1_group1[i]->image,enemy_level1_group1[i]->x-scrollx,enemy_level1_group1[i]->y-scrolly);
				}//allegro_message("%d",i);
				enemyshoot(enemy_level1_group1[i]);
				
			}
		}
		j = 1;
		for(i=0;i<10;i++)
			if(enemy_level1_group1[i]->alive)
				j = 0;
		if(j == 1)
			enemy_level1_flag1 = 2;
	}
	if(enemy_level1_flag2){
		//int i;
		for(i=0;i<10;i++){
			if(enemy_level1_group2[i]->alive){
				if(player->x < enemy_level1_group2[i]->x)
					enemy_level1_group2[i]->dir = 0;
				else
					enemy_level1_group2[i]->dir = 1;
				
				if (++enemy_level1_group2[i]->framecount > enemy_level1_group2[i]->framedelay){
					enemy_level1_group2[i]->framecount=0;
					if (++enemy_level1_group2[i]->curframe > enemy_level1_group2[i]->maxframe)
						enemy_level1_group2[i]->curframe=0;
				}
			
				//Move
				if(enemy_level1_group2[i]->movecounter < enemy_level1_group2[i]->movedelay){
					enemy_level1_group2[i]->movecounter++;
					if(enemy_level1_group2[i]->movedir)
						enemy_level1_group2[i]->x += enemy_level1_group2[i]->movespeed;
					else
						enemy_level1_group2[i]->x -= enemy_level1_group2[i]->movespeed;
					
					if(enemy_level1_group2[i]->movedir){
						if(collided(enemy_level1_group2[i]->x + enemy_level1_group2[i]->image->w,enemy_level1_group2[i]->y) ||
							collided(enemy_level1_group2[i]->x + enemy_level1_group2[i]->image->w,enemy_level1_group2[i]->y + enemy_level1_group2[i]->image->h))
							enemy_level1_group2[i]->x -= enemy_level1_group2[i]->movespeed * 2;
					}else{
						if(collided(enemy_level1_group2[i]->x,enemy_level1_group2[i]->y) ||
							collided(enemy_level1_group2[i]->x,enemy_level1_group2[i]->y + enemy_level1_group2[i]->image->h))
							enemy_level1_group2[i]->x += enemy_level1_group2[i]->movespeed * 2;
					}

					if(enemy_level1_group2[i]->movecounter >= enemy_level1_group2[i]->movedelay){
						enemy_level1_group2[i]->movecounter = 0;
						enemy_level1_group2[i]->movedelay = rand() % enemy_level1_group2[i]->movedelaymax + 1;
						enemy_level1_group2[i]->movedir = rand() % 2;
						enemy_level1_group2[i]->movespeed = (rand() % 2) + 1;
					}
				}

				enemy_level1_group2[i]->image = enemy_level1_group2[i]->bmp[enemy_level1_group2[i]->curframe];
				enemy_level1_group2[i]->width = enemy_level1_group2[i]->image->w;
				enemy_level1_group2[i]->height = enemy_level1_group2[i]->image->h;
				
				if(!collided(enemy_level1_group2[i]->x + enemy_level1_group2[i]->image->w/2,enemy_level1_group2[i]->y + enemy_level1_group2[i]->image->h + enemy_level1_group2[i]->dropspeed))
					enemy_level1_group2[i]->y += enemy_level1_group2[i]->dropspeed;
				
				if(enemy_level1_group2[i]->x > scrollx - 20 && enemy_level1_group2[i]->x < scrollx + SCREEN_WIDTH + 20){
					if(!enemy_level1_group2[i]->dir)
						draw_sprite(buffer,enemy_level1_group2[i]->image,enemy_level1_group2[i]->x-scrollx,enemy_level1_group2[i]->y-scrolly);
					else
						draw_sprite_h_flip(buffer,enemy_level1_group2[i]->image,enemy_level1_group2[i]->x-scrollx,enemy_level1_group2[i]->y-scrolly);
				}
				enemyshoot(enemy_level1_group2[i]);
			}
		}
		j = 1;
		for(i=0;i<10;i++)
			if(enemy_level1_group2[i]->alive)
				j = 0;
		if(j == 1)
			enemy_level1_flag2 = 2;
	}
	if(enemy_level1_flag3){
		//int i;
		for(i=0;i<20;i++){
			if(enemy_level1_group3[i]->alive){
				if(player->x < enemy_level1_group3[i]->x)
					enemy_level1_group3[i]->dir = 0;
				else
					enemy_level1_group3[i]->dir = 1;
				
				if (++enemy_level1_group3[i]->framecount > enemy_level1_group3[i]->framedelay){
					enemy_level1_group3[i]->framecount=0;
					if (++enemy_level1_group3[i]->curframe > enemy_level1_group3[i]->maxframe)
						enemy_level1_group3[i]->curframe=0;
				}
			
				//Move
				if(enemy_level1_group3[i]->movecounter < enemy_level1_group3[i]->movedelay){
					enemy_level1_group3[i]->movecounter++;
					if(enemy_level1_group3[i]->movedir)
						enemy_level1_group3[i]->x += enemy_level1_group3[i]->movespeed;
					else
						enemy_level1_group3[i]->x -= enemy_level1_group3[i]->movespeed;
					
					if(enemy_level1_group3[i]->movedir){
						if(collided(enemy_level1_group3[i]->x + enemy_level1_group3[i]->image->w,enemy_level1_group3[i]->y) ||
							collided(enemy_level1_group3[i]->x + enemy_level1_group3[i]->image->w,enemy_level1_group3[i]->y + enemy_level1_group3[i]->image->h))
							enemy_level1_group3[i]->x -= enemy_level1_group3[i]->movespeed * 2;
					}else{
						if(collided(enemy_level1_group3[i]->x,enemy_level1_group3[i]->y) ||
							collided(enemy_level1_group3[i]->x,enemy_level1_group3[i]->y + enemy_level1_group3[i]->image->h))
							enemy_level1_group3[i]->x += enemy_level1_group3[i]->movespeed * 2;
					}

					if(enemy_level1_group3[i]->movecounter >= enemy_level1_group3[i]->movedelay){
						enemy_level1_group3[i]->movecounter = 0;
						enemy_level1_group3[i]->movedelay = rand() % enemy_level1_group3[i]->movedelaymax + 1;
						enemy_level1_group3[i]->movedir = rand() % 2;
						enemy_level1_group3[i]->movespeed = (rand() % 2) + 1;
					}
				}

				enemy_level1_group3[i]->image = enemy_level1_group3[i]->bmp[enemy_level1_group3[i]->curframe];
				enemy_level1_group3[i]->width = enemy_level1_group3[i]->image->w;
				enemy_level1_group3[i]->height = enemy_level1_group3[i]->image->h;
				
				if(!collided(enemy_level1_group3[i]->x + enemy_level1_group3[i]->image->w/2,enemy_level1_group3[i]->y + enemy_level1_group3[i]->image->h + enemy_level1_group3[i]->dropspeed))
					enemy_level1_group3[i]->y += enemy_level1_group3[i]->dropspeed;
				
				if(enemy_level1_group3[i]->x > scrollx - 20 && enemy_level1_group3[i]->x < scrollx + SCREEN_WIDTH + 20){
					if(!enemy_level1_group3[i]->dir)
						draw_sprite(buffer,enemy_level1_group3[i]->image,enemy_level1_group3[i]->x-scrollx,enemy_level1_group3[i]->y-scrolly);
					else
						draw_sprite_h_flip(buffer,enemy_level1_group3[i]->image,enemy_level1_group3[i]->x-scrollx,enemy_level1_group3[i]->y-scrolly);
				}
				enemyshoot(enemy_level1_group3[i]);
				//allegro_message("%d",i);
			}
		}
		j = 1;
		for(i=0;i<10;i++)
			if(enemy_level1_group2[i]->alive)
				j = 0;
		if(j == 1)
			enemy_level1_flag3 = 2;
	}
}
void updateBoss(int level){
	if(level == 1){
		for(i=0;i<20;i++){
			if(enemy_level1_bossgroup[i]->alive){
				if(player->x < enemy_level1_bossgroup[i]->x)
					enemy_level1_bossgroup[i]->dir = 0;
				else
					enemy_level1_bossgroup[i]->dir = 1;
				
				if (++enemy_level1_bossgroup[i]->framecount > enemy_level1_bossgroup[i]->framedelay){
					enemy_level1_bossgroup[i]->framecount=0;
					if (++enemy_level1_bossgroup[i]->curframe > enemy_level1_bossgroup[i]->maxframe)
						enemy_level1_bossgroup[i]->curframe=0;
				}
			
				//Move
				if(enemy_level1_bossgroup[i]->movecounter < enemy_level1_bossgroup[i]->movedelay){
					enemy_level1_bossgroup[i]->movecounter++;
					if(enemy_level1_bossgroup[i]->movedir)
						enemy_level1_bossgroup[i]->x += enemy_level1_bossgroup[i]->movespeed;
					else
						enemy_level1_bossgroup[i]->x -= enemy_level1_bossgroup[i]->movespeed;
					
					if(enemy_level1_bossgroup[i]->movedir){
						if(collided(enemy_level1_bossgroup[i]->x + enemy_level1_bossgroup[i]->image->w,enemy_level1_bossgroup[i]->y) ||
							collided(enemy_level1_bossgroup[i]->x + enemy_level1_bossgroup[i]->image->w,enemy_level1_bossgroup[i]->y + enemy_level1_bossgroup[i]->image->h))
							enemy_level1_bossgroup[i]->x -= enemy_level1_bossgroup[i]->movespeed * 2;
					}else{
						if(collided(enemy_level1_bossgroup[i]->x,enemy_level1_bossgroup[i]->y) ||
							collided(enemy_level1_bossgroup[i]->x,enemy_level1_bossgroup[i]->y + enemy_level1_bossgroup[i]->image->h))
							enemy_level1_bossgroup[i]->x += enemy_level1_bossgroup[i]->movespeed * 2;
					}

					if(enemy_level1_bossgroup[i]->movecounter >= enemy_level1_bossgroup[i]->movedelay){
						enemy_level1_bossgroup[i]->movecounter = 0;
						enemy_level1_bossgroup[i]->movedelay = rand() % enemy_level1_bossgroup[i]->movedelaymax + 1;
						enemy_level1_bossgroup[i]->movedir = rand() % 2;
						enemy_level1_bossgroup[i]->movespeed = (rand() % 2) + 1;
					}
				}

				enemy_level1_bossgroup[i]->image = enemy_level1_bossgroup[i]->bmp[enemy_level1_bossgroup[i]->curframe];
				enemy_level1_bossgroup[i]->width = enemy_level1_bossgroup[i]->image->w;
				enemy_level1_bossgroup[i]->height = enemy_level1_bossgroup[i]->image->h;
				
				if(!collided(enemy_level1_bossgroup[i]->x + enemy_level1_bossgroup[i]->image->w/2,enemy_level1_bossgroup[i]->y + enemy_level1_bossgroup[i]->image->h + enemy_level1_bossgroup[i]->dropspeed))
					enemy_level1_bossgroup[i]->y += enemy_level1_bossgroup[i]->dropspeed;
				
				if(enemy_level1_bossgroup[i]->x > scrollx - 20 && enemy_level1_bossgroup[i]->x < scrollx + SCREEN_WIDTH + 20){
					if(!enemy_level1_bossgroup[i]->dir)
						draw_sprite(buffer,enemy_level1_bossgroup[i]->image,enemy_level1_bossgroup[i]->x-scrollx,enemy_level1_bossgroup[i]->y-scrolly);
					else
						draw_sprite_h_flip(buffer,enemy_level1_bossgroup[i]->image,enemy_level1_bossgroup[i]->x-scrollx,enemy_level1_bossgroup[i]->y-scrolly);
				}
				enemyshoot(enemy_level1_bossgroup[i]);
				//allegro_message("%d",i);
			}
		}
	}
}
void getInput(){
	if(!player->alive)
		return;

	oldpx = player->x;
	oldpy = player->y;
	olds = player->state;
	oldf = player->curframe;
	
	if(showgameinfo_counter < showgameinfo_delay)
		showgameinfo_counter++;
	else{
		if(key[KEY_I] || joy[0].button[0].b){
			showgameinfo *= -1;
			showgameinfo_counter = 0;
		}
	}

	if(key[KEY_RIGHT] || joy[0].button[13].b){
		player->dir = 1;
		if(key[KEY_X] || joy[0].button[1].b){
			if(player->state != 3 && player->state != 2 && player->jumpcur > player->jumpcheck - 5){
				//Start Slide
				player->framecount = 0;
				player->state = 3;
				player->slidecounter = 0;
			}else{
				if(player->slidecounter < player->slidemax){
					//Keep sliding
					player->slidecounter++;
					player->x += player->slidespeed;
					player->xspd_run = player->slidespeed;
				}else{
					//End slide
					player->x += player->xspd_run;
					player->state = 1;
					player->framecount = 0;					
				}
			}
		}else{
			//Just running , no slide
			player->xspd_run = player->xspeed;
			player->x += player->xspd_run;
			if(player->state != 1){
				player->state = 1;
				player->framecount = 0;
			}
		}
	}else if(key[KEY_LEFT] || joy[0].button[15].b){
		player->dir = 0;
		if(key[KEY_X] || joy[0].button[1].b){
			if(player->state != 3 && player->state != 2 && player->jumpcur > player->jumpcheck -5){
				//Start Slide
				player->framecount = 0;
				player->state = 3;
				player->slidecounter = 0;
			}else{
				if(player->slidecounter < player->slidemax){
					player->slidecounter++;
					player->x -= player->slidespeed;
					player->xspd_run = player->slidespeed;
				}else{
					player->x -= player->xspd_run;
					player->state = 1;
					player->framecount = 0;					
				}

			}
		}else{
			player->xspd_run = player->xspeed;
			player->x -= player->xspd_run;
			if(player->state != 1){
				player->state = 1;
				player->framecount = 0;
			}
		}
	}else
		player->state = 0;
	

	//handle jumping
	if (player->jumpcur == player->jumpcheck){ 
        if (!collided(player->x + player->width/2, player->y + player->height+2))
			player->jumpcur = 0; 
		if (key[KEY_UP] || joy[0].button[2].b){
			player->jumpcur += player->jumpmax/2;
		}
    }else{
        player->y -= player->jumpcur/3; 
        player->jumpcur--; 
	}
	if (player->jumpcur < 0) {
        if (collided(player->x + player->width/2, player->y + player->height)){
			player->jumpcur = player->jumpcheck; 
            while (collided(player->x + player->width/2, player->y + player->height))
                player->y -= 2; 
        }
    }
	if(player->jumpcur < player->jumpmax){
		if(!collided(player->x + player->width/2, player->y + player->height + 5)){
			player->state = 2;
		}
		while(collided(player->x + player->width/2,player->y + player->height/5))
			player->y += 2;
	}
	
	if(player->state == 0){ //Idling
		if(player->shootcounter < player->shootdelay){ //Shoot bitmap
			player->shootcounter++;
			player->maxframe = 1; player->framedelay = 5;
		}
		else //Not shooting bitmap
			player->maxframe = 2; player->framedelay= 10;
	}
	
	//If player shot recently [0 through shootdelay] -> assign shooting bitmap
	if(player->shootcounter < player->shootdelay)
			player->shootcounter++;

	if (player->state == 1){ //Running
		if(player->shootcounter < player->shootdelay){
			player->maxframe = 8; player->framedelay = 3;
		}
		else{
			player->maxframe = 9; player->framedelay = 2;
		}
	}
	if (player->state == 2){ //Jumping/falling
		if(player->shootcounter < player->shootdelay){
			player->maxframe = 6; player->framedelay = 10;
		}
		else{
			player->maxframe = 6; player->framedelay = 10;
		}
	}
	if (player->state == 3){ //Sliding
		if(player->shootcounter < player->shootdelay){
			player->maxframe = 1; player->framedelay = 3;
		}
		else{
			player->maxframe = 1; player->framedelay = 3;
		}
	}

	if (++player->framecount > player->framedelay){
        player->framecount=0;
        if (++player->curframe > player->maxframe)
            player->curframe=0;
    }
	if(player->state == 0)
		if(player->shootcounter < player->shootdelay)
			player->image = player->idle_shoot[player->curframe];
		else
			player->image = player->idle[player->curframe];
	if(player->state == 1)
		if(player->shootcounter < player->shootdelay)
			player->image = player->run_shoot[player->curframe];
		else
			player->image = player->run[player->curframe];
	if(player->state == 2){
		if(player->shootcounter < player->shootdelay)
			if(player->jumpcur > 5)
				player->image = player->jump_shoot[2];
			else if(player->jumpcur > -5)
				player->image = player->jump_shoot[3];
			else
				player->image = player->jump_shoot[4];
		else
			if(player->jumpcur > 5)
				player->image = player->jump[2];
			else if(player->jumpcur > -5)
				player->image = player->jump[3];
			else
				player->image = player->jump[4];
	}
	if(player->state == 3)
		if(player->shootcounter < player->shootdelay)
			if(player->slidecounter > 2)
				player->image = player->slide_shoot[1];
			else
				player->image = player->slide_shoot[0];
		else
			if(player->slidecounter > 2)
				player->image = player->slide[1];
			else
				player->image = player->slide[0];

	player->width = player->image->w;
	player->height = player->image->h;
	
	//check for collided with foreground tiles
	if (!player->dir) { 
        if (collided(player->x, player->y + player->height - player->height/4) || collided(player->x, player->y + player->height/2) || collided(player->x, player->y + player->height-5)){
            player->x = oldpx;
			//player->y = oldpx;
		}
    }
	else if (collided(player->x + player->width, player->y + player->height - player->height/4) || collided(player->x + player->width, player->y + player->height/2) || collided(player->x + player->width, player->y + player->height - 5)){
            player->x = oldpx; 
			//player->y = oldpx;
	}
}
void playershoot(){
	if(++player->bulletdelay_counter > player->bulletdelay)
		player->bulletdelay_counter = player->bulletdelay + 1;
	if(key[KEY_SPACE] || joy[0].button[3].b){
		if(player->charging > 10 && !player->charge_isplaying){
			player->charge_isplaying = 1;
			if(!play_sample(blaster_charge,128,128,1000,0)){
				allegro_message("Unable to play blaster charge");
				exit(1);
			}
		}
		player->charging += 2;
	
	}else if(player->charging > 0) {//Spacebar released to activate shot
			if(player->bulletdelay_counter > player->bulletdelay){
				player->bulletdelay_counter = 0;
				for(i=0;i<player->numbullets;i++){
					if(!player->bullets[i]->alive){ //Start this bullet
						if(player->charging < player->charge1){
							player->bullets[i]->bullet = &player->bullet_1[0];
							player->bullets[i]->maxframe = 0;
							player->bullets[i]->framedelay = 1;
							player->bullets[i]->level = 1;
							player->bullets[i]->damage = 10;

							if(!play_sample(blaster1,128,128,2000,0)){
								allegro_message("Unable to play blaster1");
								exit(1);
							}
						}
						else if(player->charging < player->charge2){
							player->bullets[i]->bullet = player->bullet_2[0];
							player->bullets[i]->maxframe = 6;
							player->bullets[i]->framedelay = 1;
							player->bullets[i]->level = 2;
							player->bullets[i]->damage = 30;

							if(!play_sample(blaster2,128,128,1000,0)){
								allegro_message("Unable to play blaster1");
								exit(1);
							}
						}
						else{
							player->bullets[i]->bullet = player->bullet_3[0];
							player->bullets[i]->maxframe = 10;
							player->bullets[i]->framedelay = 1;
							player->bullets[i]->level = 3;
							player->bullets[i]->damage = 100;

							if(!play_sample(blaster3,128,128,1000,0)){
								allegro_message("Unable to play blaster1");
								exit(1);
							}
						}
						player->shootcounter = 0;
						player->bullets[i]->alive = 1;
						player->bullets[i]->owner = 1;
						player->bullets[i]->dir = player->dir;
						player->bullets[i]->framecount = 0;
						player->bullets[i]->curframe = 0;
						player->bullets[i]->width = player->bullets[i]->bullet->w;
						player->bullets[i]->height = player->bullets[i]->bullet->h;
						player->bullets[i]->x = player->x + player->width/2;
						if(player->bullets[i]->level == 1)
							player->bullets[i]->y = player->y + player->height/2;
						if(player->bullets[i]->level == 2)
							player->bullets[i]->y = player->y + player->height/3;
						if(player->bullets[i]->level == 3)
							player->bullets[i]->y = player->y + player->height/4;
						if( (key[KEY_DOWN] || joy[0].button[14].b) && player->jumpcur >= player->jumpcheck - 5) //Crouch shot
							player->bullets[i]->y += player->height/5;
						break;
					}
				}		
			}
		player->charging = 0;
		player->charge_isplaying = 0;
		stop_sample(blaster_charge);
	}
	for(i=0;i<player->numbullets;i++){
		if(player->bullets[i]->alive){// Move this active bullet

			//Update Animation
			if (++player->bullets[i]->framecount > player->bullets[i]->framedelay){
				player->bullets[i]->framecount=0;
				if (++player->bullets[i]->curframe > player->bullets[i]->maxframe)
					player->bullets[i]->curframe=1;
			}

			if(player->bullets[i]->level == 1)
				player->bullets[i]->bullet = &player->bullet_1[0];
			else if(player->bullets[i]->level == 2)
				player->bullets[i]->bullet = player->bullet_2[player->bullets[i]->curframe];
			else if(player->bullets[i]->level == 3)
				player->bullets[i]->bullet = player->bullet_3[player->bullets[i]->curframe];

			//Move
			if(player->bullets[i]->dir)
				player->bullets[i]->x += player->bulletspeed;
			else
				player->bullets[i]->x -= player->bulletspeed;

			if(player->bullets[i]->x/* + player->bullets[i]->width*/ > scrollx + SCREEN_WIDTH
				|| player->bullets[i]->x + player->bullets[i]->width < scrollx-10 || collided(player->bullets[i]->x,player->bullets[i]->y))
				player->bullets[i]->alive = 0;
			if(player->bullets[i]->alive)
				if(player->bullets[i]->dir)
					draw_sprite(buffer,player->bullets[i]->bullet,player->bullets[i]->x-scrollx,player->bullets[i]->y-scrolly);
				else
					draw_sprite_h_flip(buffer,player->bullets[i]->bullet,player->bullets[i]->x-scrollx,player->bullets[i]->y-scrolly);
		}
	}	
}
void enemyshoot(ENEMY * e){
	//int k;
	//Debugging: allegro_message("here");
	if(++e->bulletdelay_counter > e->bulletdelay)
		e->bulletdelay_counter = e->bulletdelay + 1;
	for(j=0;j<e->numbullets;j++){
		if(e->bullets[j]->alive){ //Update bullet
			//allegro_message("Here");
			if(!e->bullets[j]->dir)
				e->bullets[j]->x -= e->bulletspeed;
			else
				e->bullets[j]->x += e->bulletspeed;
			if(collided(e->bullets[j]->x,e->bullets[j]->y))
				e->bullets[j]->alive = 0;
			if(e->bullets[j]->x < scrollx || e->bullets[j]->x + e->bullets[j]->bullet->w > scrollx + SCREEN_WIDTH)
				e->bullets[j]->alive = 0;
		}else{ //Start bullet
			//allegro_message("%d %d",e->bulletdelay,e->bulletdelay_counter);
			if(e->bulletdelay_counter > e->bulletdelay){
				e->bullets[j]->alive = 1;
				e->bullets[j]->owner = 0;
				e->bullets[j]->dir = e->dir;
				e->bullets[j]->x = e->x + e->image->w/2;
				e->bullets[j]->y = e->y + e->image->h/2;
				e->bullets[j]->bullet = player->bullet_1;
				e->bullets[j]->width = e->bullets[j]->bullet->w;
				e->bullets[j]->height = e->bullets[j]->bullet->h;
				e->bulletdelay_counter = 0;
				e->bulletdelay = rand() % (e->bulletdelay_max * 5) + 10;
			}
		}
		if(e->bullets[j]->alive){
			if(!e->dir)
				draw_sprite(buffer,e->bullets[j]->bullet,e->bullets[j]->x-scrollx,e->bullets[j]->y-scrolly+1);
			else
				draw_sprite_h_flip(buffer,e->bullets[j]->bullet,e->bullets[j]->x-scrollx,e->bullets[j]->y-scrolly+1);
		}
		//allegro_message("X: %i Y: %i Alive: %i Dir: %i",e->bullets[i]->x,e->bullets[i]->y,e->bullets[i]->alive,e->dir);
	}
}
void checkLevel1BossBullets(){
	int i;
	for(i=0;i<player->numbullets;i++){
		for(j=0;j<20;j++){
			if(player->bullets[i]->alive && enemy_level1_bossgroup[j]->alive){
				if(inside(player->bullets[i]->x + (player->bullets[i]->width/2),
					player->bullets[i]->y + (player->bullets[i]->height/2),
					enemy_level1_bossgroup[j]->x,enemy_level1_bossgroup[j]->y,
					enemy_level1_bossgroup[j]->x + enemy_level1_bossgroup[j]->image->w,
					enemy_level1_bossgroup[j]->y + enemy_level1_bossgroup[j]->image->h)){
						//allegro_message("%d",enemy_level1_bossgroup[i]->bullets[j]->damage);
						enemy_level1_bossgroup[j]->health -= player->bullets[i]->damage;
						player->bullets[i]->alive = 0;
						if(enemy_level1_bossgroup[j]->health <= 0)
							enemy_level1_bossgroup[j]->alive = 0;
				}
			}
		}
	}
	for(i=0;i<20;i++){
		if(enemy_level1_bossgroup[i]->alive){
			for(j=0;j<enemy_level1_bossgroup[i]->numbullets;j++){
				if(enemy_level1_bossgroup[i]->bullets[j]->alive){
					if(inside(enemy_level1_bossgroup[i]->bullets[j]->x + enemy_level1_bossgroup[i]->bullets[j]->width/2,
						enemy_level1_bossgroup[i]->bullets[j]->y + enemy_level1_bossgroup[i]->bullets[j]->height/2,
						player->x, player->y + 10,player->x + player->width, player->y + player->height)){
						player->health -= enemy_level1_bossgroup[i]->bullets[j]->damage;
						enemy_level1_bossgroup[i]->bullets[j]->alive = 0;
					}
				}
			}
		}
	}
}
void checkBullets(){
	if(enemy_level1_flag1 == 1){
		int i;
		for(i=0;i<player->numbullets;i++){
			for(j=0;j<sizeof(enemy_level1_group1)/sizeof(enemy_level1_group1[0]);j++){
				if(player->bullets[i]->alive && enemy_level1_group1[j]->alive){
					if(inside(player->bullets[i]->x + (player->bullets[i]->width/2),
						player->bullets[i]->y + (player->bullets[i]->height/2),
						enemy_level1_group1[j]->x,enemy_level1_group1[j]->y,
						enemy_level1_group1[j]->x + enemy_level1_group1[j]->width,
						enemy_level1_group1[j]->y + enemy_level1_group1[j]->height)){
							//allegro_message("%d",enemy_level1_group1[i]->bullets[j]->damage);
							enemy_level1_group1[j]->health -= player->bullets[i]->damage;
							player->bullets[i]->alive = 0;
							if(enemy_level1_group1[j]->health <= 0)
								enemy_level1_group1[j]->alive = 0;
					}
				}
			}
		}
		for(i=0;i<sizeof(enemy_level1_group1)/sizeof(enemy_level1_group1[0]);i++){
			if(enemy_level1_group1[i]->alive){
				for(j=0;j<enemy_level1_group1[i]->numbullets;j++){
					if(enemy_level1_group1[i]->bullets[j]->alive){
						if(inside(enemy_level1_group1[i]->bullets[j]->x + enemy_level1_group1[i]->bullets[j]->width/2,
							enemy_level1_group1[i]->bullets[j]->y + enemy_level1_group1[i]->bullets[j]->height/2,
							player->x, player->y + 10,player->x + player->width, player->y + player->height)){
							player->health -= enemy_level1_group1[i]->bullets[j]->damage;
							enemy_level1_group1[i]->bullets[j]->alive = 0;
						}
					}
				}
			}
		}
	}
	if(enemy_level1_flag2 == 1){
		for(i=0;i<player->numbullets;i++){
			for(j=0;j<sizeof(enemy_level1_group2)/sizeof(enemy_level1_group2[0]);j++){
				if(player->bullets[i]->alive && enemy_level1_group2[j]->alive){
					if(inside(player->bullets[i]->x + (player->bullets[i]->width/2),
						player->bullets[i]->y + (player->bullets[i]->height/2),
						enemy_level1_group2[j]->x,enemy_level1_group2[j]->y,
						enemy_level1_group2[j]->x + enemy_level1_group2[j]->width,
						enemy_level1_group2[j]->y + enemy_level1_group2[j]->height)){
							enemy_level1_group2[j]->health -= player->bullets[i]->damage;
							player->bullets[i]->alive = 0;
							if(enemy_level1_group2[j]->health <= 0)
								enemy_level1_group2[j]->alive = 0;
					}
				}
			}
		}
		for(i=0;i<sizeof(enemy_level1_group2)/sizeof(enemy_level1_group2[0]);i++){
			if(enemy_level1_group2[i]->alive){
				for(j=0;j<enemy_level1_group2[i]->numbullets;j++){
					if(enemy_level1_group2[i]->bullets[j]->alive){
						if(inside(enemy_level1_group2[i]->bullets[j]->x + enemy_level1_group2[i]->bullets[j]->width/2,
							enemy_level1_group2[i]->bullets[j]->y + enemy_level1_group2[i]->bullets[j]->height/2,
							player->x, player->y + 10,player->x + player->width, player->y + player->height)){
							player->health -= enemy_level1_group2[i]->bullets[j]->damage;
							enemy_level1_group2[i]->bullets[j]->alive = 0;
						}
					}
				}
			}
		}
	}
	if(enemy_level1_flag3 == 1){
		for(i=0;i<player->numbullets;i++){
			for(j=0;j<sizeof(enemy_level1_group3)/sizeof(enemy_level1_group3[0]);j++){
				if(player->bullets[i]->alive && enemy_level1_group3[j]->alive){
					if(inside(player->bullets[i]->x + (player->bullets[i]->width/2),
						player->bullets[i]->y + (player->bullets[i]->height/2),
						enemy_level1_group3[j]->x,enemy_level1_group3[j]->y,
						enemy_level1_group3[j]->x + enemy_level1_group3[j]->width,
						enemy_level1_group3[j]->y + enemy_level1_group3[j]->height)){
							enemy_level1_group3[j]->health -= player->bullets[i]->damage;
							player->bullets[i]->alive = 0;
							if(enemy_level1_group3[j]->health <= 0)
								enemy_level1_group3[j]->alive = 0;
					}
				}
			}
		}
		for(i=0;i<sizeof(enemy_level1_group3)/sizeof(enemy_level1_group3[0]);i++){
			if(enemy_level1_group3[i]->alive){
				for(j=0;j<enemy_level1_group3[i]->numbullets;j++){
					if(enemy_level1_group3[i]->bullets[j]->alive){
						if(inside(enemy_level1_group3[i]->bullets[j]->x + enemy_level1_group3[i]->bullets[j]->width/2,
							enemy_level1_group3[i]->bullets[j]->y + enemy_level1_group3[i]->bullets[j]->height/2,
							player->x, player->y + 10,player->x + player->width, player->y + player->height)){
							player->health -= enemy_level1_group3[i]->bullets[j]->damage;
							enemy_level1_group3[i]->bullets[j]->alive = 0;
						}
					}
				}
			}
		}
	}
}
void loadenemy(ENEMY * e, int type, int x, int y){
	x *= 16;
	y *= 16;
	//ENEMY * e = (ENEMY *) malloc(sizeof(ENEMY)); //Here
	if(type == 1){ //Little helmet roller dude
		//Little helmet roller guy
		for(i=0;i<5;i++)
			e->bmp[i] = grabframe(enemy_sheet,19,22,97,0,10,i);

		e->x = x;
		e->y = y;
		e->curframe = 0;
		e->framecount = 0;
		e->framedelay = 3;
		e->maxframe = 4;
		e->movecounter = 0;
		e->movedelay = 30;
		e->movedelaymax = e->movedelay;
		e->movedir = 0;
		e->movespeed = 1;
		e->numbullets = 1;
		e->bulletdelay = 20;
		e->bulletdelay_max = 20;
		e->bulletdelay_counter = 0;
		e->bulletspeed = 6;
		e->health = 40;
		e->alive = 1;
		e->dropspeed = 2;
		e->image = e->bmp[0];

		for(i=0;i<e->numbullets;i++){
			e->bullets[i]->alive = 0;
			e->bullets[i]->damage = 20;
		}

		return;
	}
	if(type == 2){
		//Little flying helmet guy
		for(i=0;i<3;i++)
			e->bmp[i] = grabframe(enemy_sheet,29,25,0,52,3,i);
		e->bmp[3] = grabframe(enemy_sheet,29,25,0,77,1,0);
		e->bmp[4] = grabframe(enemy_sheet,29,25,29,77,1,0);
		e->bmp[5] = grabframe(enemy_sheet,29,25,58,77,1,0);

		e->x = x;
		e->y = y;
		e->curframe = 0;
		e->framecount = 0;
		e->framedelay = 3;
		e->maxframe = 5;
		e->movecounter = 0;
		e->movedelay = 30;
		e->movedelaymax = e->movedelay;
		e->movedir = 0;
		e->movespeed = 1;
		e->numbullets = 2;
		e->bulletdelay = 10;
		e->bulletdelay_max = 10;
		e->bulletdelay_counter = 0;
		e->bulletspeed = 6;
		e->health = 60;
		e->alive = 1;
		e->dropspeed = 0;
		e->image = e->bmp[0];

		for(i=0;i<e->numbullets;i++){
			e->bullets[i]->alive = 0;
			e->bullets[i]->damage = 20;
		}

		
		return;
	}
	if(type == 3){
		//Flying heli
		e->bmp[0] = grabframe(enemy_sheet,41,40,170,102,1,0);
		e->bmp[1] = grabframe(enemy_sheet,41,40,211,102,1,0);

		e->x = x;
		e->y = y;
		e->curframe = 0;
		e->framecount = 0;
		e->framedelay = 3;
		e->maxframe = 1;
		e->movecounter = 0;
		e->movedelay = 30;
		e->movedir = 0;
		e->movespeed = 1;
		e->movedelaymax = e->movedelay;
		e->numbullets = 5;
		e->bulletdelay = 10;
		e->bulletdelay_max = 10;
		e->bulletdelay_counter = 0;
		e->bulletspeed = 6;
		e->health = 60;
		e->alive = 1;
		e->dropspeed = 0;
		e->image = e->bmp[0];
		
		for(i=0;i<e->numbullets;i++){
			e->bullets[i]->alive = 0;
			e->bullets[i]->damage = 20;
		}

		return;
	}
}
int playerdie(){
	if(player->health <= 0){
		if(player->state != 10){ //Start dying
			player->state = 10;
			player->alive = 0;
			stop_sample(playerdeath);
			play_sample(playerdeath,128,128,1000,0);
			player->framecount = 0;
			player->framedelay = 2;
			player->curframe = 0;
			player->maxframe = 10;
		}else{ //Continue dying
			if(player->curframe >= player->maxframe){ //Reset level if player dies
				player->x = player->level1_spawnx;
				player->y = player->level1_spawny;
				player->state = 1;
				player->alive = 1;
				player->lives--;
				player->image = player->idle[0];
				player->health = player->health_max;
				loadL1Enemies();
				loadLevel1Boss();
			}

			if (++player->framecount > player->framedelay){
				player->framecount=0;
				player->curframe++;
			}
			player->image = player->death[player->curframe];
			if(player->dir)
				draw_sprite(buffer,player->image,player->x,player->y);
			else
				draw_sprite_h_flip(buffer,player->image,player->x,player->y);
			
			//Slow down..
			//rest(10);
		}
		return 1;
	}
	return 0;
}
void pauseGame(){
	//Pause
	if(pause_counter < pause_delay)
		pause_counter++;
	else{
		if(key[KEY_ENTER] || joy[0].button[11].b){
			pause_counter = 0;
			while(1){
				clear_bitmap(buffer);
				textout_centre_ex(buffer,font,"Paused",SCREEN_WIDTH/2,SCREEN_HEIGHT/2,WHITE,-1);
				blit(buffer,screen,0,0,0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1);
				if(pause_counter < pause_delay)
					pause_counter++;
				if(pause_counter >= pause_delay)
					if(key[KEY_ENTER] || joy[0].button[11].b)
						break;
				rest(10);
				clear_keybuf();
				poll_joystick();
			}
			pause_counter = 0;
		}
	}
}
void pickupItems(){
	for(i=0;i<5;i++){
		if(healthpickup[i]->alive){
			draw_sprite(buffer,healthpickup[i]->image,healthpickup[i]->x-scrollx,healthpickup[i]->y-scrolly);
			if(inside(healthpickup[i]->x+healthpickup[i]->image->w/2,healthpickup[i]->y+healthpickup[i]->image->h/2,player->x,player->y,player->x+player->width,player->x+player->height)){
				player->health += 30;
				if(player->health > player->health_max)
					player->health = player->health_max;
				healthpickup[i]->alive = 0;
				play_sample(itempickedup,128,128,1000,0);
			}
		}
	}
	for(i=0;i<3;i++){
		if(extralifepickup[i]->alive){
			draw_sprite(buffer,extralifepickup[i]->image,extralifepickup[i]->x-scrollx,extralifepickup[i]->y-scrolly);
			if(inside(extralifepickup[i]->x+extralifepickup[i]->image->w/2,extralifepickup[i]->y+extralifepickup[i]->image->h/2,player->x,player->y,player->x+player->width,player->x+player->height)){
				player->lives++;
				extralifepickup[i]->alive = 0;
				play_sample(itempickedup,128,128,1000,0);
			}
		}
	}
}
void playCredits(){
	int y = SCREEN_HEIGHT + 10;
	clear_bitmap(buffer);
	stop_sample(level1_bg);
	stop_sample(level2_bg);
	credits = load_sample("all_shall_perish_so_far_away.wav");
	if(!credits)
		play_sample(level1_bg,128,128,1000,1);
	else
		play_sample(credits,128,128,1000,1);
	while(!key[KEY_ESC]){
		if(y > -6000){
			textout_centre_ex(buffer,font,"GAME OVER",SCREEN_WIDTH/2,y,WHITE,-1);
			textout_centre_ex(buffer,font,"Thanks for playing!",SCREEN_WIDTH/2,y+10,WHITE,-1);
			textout_centre_ex(buffer,font,"by Luke Pederson",SCREEN_WIDTH/2,y+200,WHITE,-1);
			textout_centre_ex(buffer,font,"for CST 306 - Game Engine Programming",SCREEN_WIDTH/2,y+210,WHITE,-1);
			textout_centre_ex(buffer,font,"Dr. Byun rocks!",SCREEN_WIDTH/2,y+300,WHITE,-1);
			textout_centre_ex(buffer,font,"Credits song",SCREEN_WIDTH/2,y+400,WHITE,-1);
			textout_centre_ex(buffer,font,"All Shall Perish - So Far Away",SCREEN_WIDTH/2,y+410,WHITE,-1);
			textout_centre_ex(buffer,font,"Hope you enjoy John :)",SCREEN_WIDTH/2,y+550,WHITE,-1);
			textout_centre_ex(buffer,font,"Wait for it..",SCREEN_WIDTH/2,y+700,WHITE,-1);
			textout_centre_ex(buffer,font,"Not even the solo",SCREEN_WIDTH/2,y+850,WHITE,-1);
			textout_centre_ex(buffer,font,"Here it comes...",SCREEN_WIDTH/2,y+1200,WHITE,-1);
			textout_centre_ex(buffer,font,"Suh. Weet.",SCREEN_WIDTH/2,y+1500,WHITE,-1);
			textout_centre_ex(buffer,font,"SO F***ING FAST!",SCREEN_WIDTH/2,y+1800,WHITE,-1);
			textout_centre_ex(buffer,font,"MMhmmm",SCREEN_WIDTH/2,y+2100,WHITE,-1);
			blit(buffer,screen,0,0,0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1);

			rest(40);
			y--;
			clear_bitmap(buffer);
		}
	}
	destroy_sample(credits);
}