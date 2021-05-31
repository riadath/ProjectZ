
#ifndef DINORUN_H
#define DINORUN_H

#include "texture_timer.h"

bool loadDinoMedia();
void renderMapDino();
void spawnBlocks();
bool checkCollisionDino(SDL_Rect player, SDL_Rect object);
void initVariableDino();
void freeDino();

struct Player
{
	Texture mPlayerTexture;

	const int PLAYER_WIDTH = 59;
	const int PLAYER_HEIGHT = 65;
	const int PLAYER_VEL = 80;
	const int PLAYER_SETP = 10;
	const int JUMP_VEL = 240;//use 200
	const int JUMP_STEP = 14;
	const int GRAVITY = 8;
	const int SPRITE_COUNT = 6;
	const int SPRITE_SPEED = 8;
	const int BASE_HEIGHT = 570;

	int mPosX, mPosY;
	int mVelX, mVelY;
	int mCurSprite = 0;

	SDL_Rect *mPlayerSprite;

	SDL_RendererFlip mFlipType = SDL_FLIP_NONE;

	bool mJumping = false;

	Player()
	{
		mPosX = 300, mPosY = BASE_HEIGHT - PLAYER_HEIGHT;
		mVelX = 0, mVelY = 0;
		mPlayerSprite = (SDL_Rect *)malloc(SPRITE_COUNT * sizeof(SDL_Rect));

		for (int i = 0; i < SPRITE_COUNT; i++)
		{
			mPlayerSprite[i].w = PLAYER_WIDTH;
			mPlayerSprite[i].h = PLAYER_HEIGHT;
			mPlayerSprite[i].x = i * PLAYER_WIDTH;
			mPlayerSprite[i].y = 0;
		}
	}

	void handleEvent(SDL_Event &e)
	{
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			switch (e.ksym)
			{
			case SDLK_w:
				if (mPosY < BASE_HEIGHT - PLAYER_HEIGHT - 120)
					return;
				mVelY -= JUMP_VEL;
				mFlipType = SDL_FLIP_NONE;
				mJumping = true;
				break;
			case SDLK_a:
				mVelX -= PLAYER_VEL;
				mFlipType = SDL_FLIP_HORIZONTAL;
				break;
			case SDLK_d:
				mVelX += PLAYER_VEL;
				mFlipType = SDL_FLIP_NONE;
				break;
			}
		}
	}
	void gravity()
	{
		if (!mJumping)
		{
			mPosY = std::min(BASE_HEIGHT - PLAYER_HEIGHT, mPosY + GRAVITY);
		}
	}
	void move()
	{
		if (mJumping)
		{
			mPosY -= JUMP_STEP;
			mVelY += JUMP_STEP;
			if (mVelY >= 0)
				mVelY = 0, mJumping = false;
		}
		if (mVelX > 0)
		{
			mPosX += PLAYER_SETP;
			mVelX -= PLAYER_SETP;
			if (mPosX < 0 || mPosX + PLAYER_WIDTH > SCREEN_WIDTH)
				mPosX -= PLAYER_SETP;
		}
		else if (mVelX < 0)
		{
			mPosX -= PLAYER_SETP;
			mVelX += PLAYER_SETP;
			if (mPosX < 0 || mPosX + PLAYER_WIDTH > SCREEN_WIDTH)
				mPosX += PLAYER_SETP;
		}
	}

	void spriteChanger()
	{
		mCurSprite++;
		if (mCurSprite / SPRITE_SPEED >= SPRITE_COUNT)
			mCurSprite = 0;
	}

	void render()
	{
		gravity();
		mPlayerTexture.render(mPosX, mPosY, &mPlayerSprite[mCurSprite / SPRITE_SPEED], mFlipType);
		if (mPosY == BASE_HEIGHT - PLAYER_HEIGHT)
			spriteChanger();
	}
	void free()
	{
		mPlayerTexture.free();
	}
};

int TOTAL_SCORE = 0;
int TOTAL_HEALTH = 2;

enum BACKGROUND_LAYER
{
	BUSH_LAYER_1,
	GROUND_LAYER_2,
	TREE1_LAYER_3,
	TREE2_LAYER_4,
	TREE3_LAYER_5,
	STAR_LAYER_6,
	MOON_LAYER_7,
	LAYER_COUNT
};
enum BLOCK_TYPE{
	BLOCK_T1,
	BLOCK_T2,
	BLOCK_T3,
	BLOCK_COUNT
};

Player gMyPlayer; //Player object

Texture gBgTextureDino[LAYER_COUNT];
Texture gBlockTexture[BLOCK_COUNT];
Texture gHealthTexture;

int gGroundScroll = 0;
int gTree1Scroll = 0;
int gTree2Scroll = 0;
int gTree3Scroll = 0;
int gStarScroll = 0;
int gStarDecrease = 1;
int mSpawnInterval = 0;
int mSpawnGap = 173;

const int gGroundSpeed = 6;
const int gTree1Speed = 5;
const int gTree2Speed = 4;
const int gTree3Speed = 2;
const int gStarSpeed = 1;

std::vector<std::pair<int,BLOCK_TYPE>>gBlockPos;//storing block x and block type
std::vector<bool>gBlockIfCollieded;

void freeDino(){

}

bool loadDinoMedia()
{
	if (!gMyPlayer.mPlayerTexture.loadFile("images/dino/dino_runner.png"))return false;
	if (!gBgTextureDino[BUSH_LAYER_1].loadFile("images/dino/dino_bush.png"))return false;
	if (!gBgTextureDino[GROUND_LAYER_2].loadFile("images/dino/dino_ground.png"))return false;
	if (!gBgTextureDino[TREE1_LAYER_3].loadFile("images/dino/dino_tree1.png"))return false;
	if (!gBgTextureDino[TREE2_LAYER_4].loadFile("images/dino/dino_tree3.png"))return false;
	if (!gBgTextureDino[TREE3_LAYER_5].loadFile("images/dino/dino_tree3.png"))return false;
	if(!gBgTextureDino[STAR_LAYER_6].loadFile("images/dino/dino_star.png"))return false;
	if(!gBgTextureDino[MOON_LAYER_7].loadFile("images/dino/dino_moon.png"))return false;
	if(!gBlockTexture[BLOCK_T1].loadFile("images/dino/block1.png"))return false;
	if(!gBlockTexture[BLOCK_T2].loadFile("images/dino/block2.png"))return false;
	if(!gBlockTexture[BLOCK_T3].loadFile("images/dino/block3.png"))return false;
	gFont = TTF_OpenFont("images/fonts/Oswald-BoldItalic.ttf", 24);
	if (gFont == NULL)
	{
		ERROR_T;
		return false;
	}

	return true;
}

//checks collision between two objects
bool checkCollisionDino(SDL_Rect player, SDL_Rect object)
{
	bool tFlag = true;
	player.w -= 10,player.h -= 10;
	if (player.x + player.w <= object.x)
		tFlag = false;
	if (player.x >= object.x + object.w)
		tFlag = false;
	if (player.y + player.h <= object.y)
		tFlag = false;
	if (player.y >= object.y + object.h)
		tFlag = false;
	return tFlag;
}



void spawnBlocks(){
	//clear blocks out of screen
	std::vector<std::pair<int,BLOCK_TYPE>> tBlocks;
	std::vector<bool>tIfCollieded;
	for(int i = 0;i < gBlockPos.size();i++){
		if(gBlockPos[i].first < -gBlockTexture[gBlockPos[i].second].getWidth()){
			gBlockPos[i].first = -1;
		}else tBlocks.push_back(gBlockPos[i]),tIfCollieded.push_back(gBlockIfCollieded[i]);
	}
	int random_intervals[] = {187,173,273,1823,753};
	gBlockPos = tBlocks;
	gBlockIfCollieded = tIfCollieded;
	int divCount = 0;
	if(mSpawnInterval++%random_intervals[rand()%5] == 0){
		gBlockPos.push_back(std::make_pair(SCREEN_WIDTH + 400,(BLOCK_TYPE)(rand()%BLOCK_COUNT)));
		gBlockIfCollieded.push_back(false);
	}
	// if(mSpawnInterval % 12563)mSpawnGap = std::max(53,mSpawnGap - 10);
}


void renderMapDino()
{
	spawnBlocks(); //spawing new blocks

	SDL_SetRenderDrawColor(gRender, 0, 0, 0, 155);
	SDL_RenderClear(gRender);

	gGroundScroll -= gGroundSpeed;
	if (gGroundScroll < -SCREEN_WIDTH)gGroundScroll = 0;
	gTree1Scroll -= gTree1Speed;
	if(gTree1Scroll < -SCREEN_WIDTH)gTree1Scroll = 0;
	gTree2Scroll -= gTree2Speed;
	if(gTree2Scroll < -SCREEN_WIDTH)gTree2Scroll = 0;
	gTree3Scroll -= gTree3Speed;
	if(gTree3Scroll < -SCREEN_WIDTH)gTree3Scroll = 0;
	gStarScroll -= gStarSpeed;
	if(gStarSpeed < -SCREEN_WIDTH)gStarScroll = 0;

	gBgTextureDino[MOON_LAYER_7].render(0,0);
	gBgTextureDino[STAR_LAYER_6].render(gStarScroll,0);
	gBgTextureDino[STAR_LAYER_6].render(gStarScroll + SCREEN_WIDTH,0);
	gBgTextureDino[TREE3_LAYER_5].render(gTree3Scroll,0);
	gBgTextureDino[TREE3_LAYER_5].render(gTree3Scroll + SCREEN_WIDTH,0);
	gBgTextureDino[TREE2_LAYER_4].render(gTree2Scroll,0);
	gBgTextureDino[TREE2_LAYER_4].render(gTree2Scroll + SCREEN_WIDTH,0);
	gBgTextureDino[TREE1_LAYER_3].render(gTree1Scroll,0);
	gBgTextureDino[TREE1_LAYER_3].render(gTree1Scroll + SCREEN_WIDTH,0);
	gBgTextureDino[GROUND_LAYER_2].render(gGroundScroll, 0);
	gBgTextureDino[GROUND_LAYER_2].render(gGroundScroll + SCREEN_WIDTH, 0);
	gBgTextureDino[BUSH_LAYER_1].render(gGroundScroll, 0);
	gBgTextureDino[BUSH_LAYER_1].render(gGroundScroll + SCREEN_WIDTH, 0);

	std::stringstream liveText;
	SDL_Color textColor = {255,200,0};
	liveText.str("");
	liveText << "Lives : " << TOTAL_HEALTH;
	gHealthTexture.loadFromText(liveText.str().c_str(), textColor);
	gHealthTexture.render(0,0);

	//Rendering blocks on map
	for(int i = 0;i < gBlockPos.size();i++){
		gBlockPos[i].first -= gGroundSpeed;
		BLOCK_TYPE block = gBlockPos[i].second;
		int posY = gMyPlayer.BASE_HEIGHT - gBlockTexture[block].getHeight();
		int posX = gBlockPos[i].first;
		gBlockTexture[block].render(posX ,posY);

		SDL_Rect player = {gMyPlayer.mPosX,gMyPlayer.mPosY,gMyPlayer.PLAYER_HEIGHT,gMyPlayer.PLAYER_WIDTH};
		SDL_Rect object = {gBlockPos[i].first,posY,gBlockTexture[block].getWidth(),gBlockTexture[block].getHeight()};
		
		if(checkCollisionDino(player,object) && !gBlockIfCollieded[i]){
			TOTAL_HEALTH--;
			gBlockIfCollieded[i] = true;
			printf("%d Collided\n",TOTAL_HEALTH);
		}
	}
	
	gMyPlayer.render();
}

void initVariableDino(){
	TOTAL_SCORE = 0;
	TOTAL_HEALTH = 2;
	gBlockPos.clear();
}

int dinoRun()
{
	if (!loadDinoMedia())
	{
		printf("Failed to load Media(dinorun)\n");
		return -1;
	}
	initVariableDino();

	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			gMyPlayer.handleEvent(e);
		}
		// if(TOTAL_HEALTH <= 0)quit = true;
		gMyPlayer.move();
		// printf("%d %d\n",gMyPlayer.mPosX,gMyPlayer.mPosY);
		renderMapDino();
		SDL_RenderPresent(gRender);
	}

	return TOTAL_SCORE;
}

#endif