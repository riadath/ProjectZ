#include "texture_timer.h"
#include "bucket_ball.h"
#include "pacman.h"
#include "dinorun.h"
#include "tower_game.h"

//function prototypes
bool init();
bool loadMedia();
bool checkCollision(SDL_Rect player, std::vector<SDL_Rect> objects);
void closeAll();
void renderMapObjects();
void gameInitialize();
void taskHandler();

//structure for coin animation handling
struct CoinAnimation
{
	Texture mCoinTexture;

	int mSPRITE_COUNT;
	int mSpriteLevel; //number of images in a row
	int mCoinWidth;
	int mCoinHeight;

	int mCurSprite = 0;
	const int mSPEED_DEC = 8;

	SDL_Rect *mCoinSprite;
	CoinAnimation(int tSpriteCount, int tCoinWidth, int tCoinHeight, int tSpriteLevel)
	{
		mSPRITE_COUNT = tSpriteCount;
		mCoinWidth = tCoinWidth;
		mCoinHeight = tCoinHeight;
		mSpriteLevel = tSpriteLevel;

		mCoinSprite = (SDL_Rect *)malloc(tSpriteCount * sizeof(SDL_Rect));

		for (int i = 0, xi = 0; i < mSPRITE_COUNT; i++, xi++)
		{
			mCoinSprite[i].w = mCoinWidth;
			mCoinSprite[i].h = mCoinHeight;
			mCoinSprite[i].x = xi * mCoinWidth;
			if (i >= mSpriteLevel)
				xi = 0;
			mCoinSprite[i].y = (i / mSpriteLevel) * mCoinHeight;
		}
	}
	void spriteChanger()
	{
		mCurSprite++;
		if (mCurSprite / mSPEED_DEC >= mSPRITE_COUNT)
			mCurSprite = 0;
	}
	void render(int x, int y)
	{
		mCoinTexture.render(x, y, &mCoinSprite[mCurSprite / mSPEED_DEC]);
		spriteChanger();
	}
	void free()
	{
		mCoinTexture.free();
	}
};
//structure for player
struct Character
{
	int mPosX, mPosY;
	int mCharVelX, mCharVelY;
	int mCurSprite;

	int CHAR_SPRITE_COUNT;
	int mCharWidth;
	int mCharHeight;

	const int CHAR_SPEED_DEC = 5;
	const int CHAR_VELOCITY = 4;

	SDL_Rect mCharShape;
	SDL_Rect *mCharacterSprite;
	SDL_RendererFlip mFlipType;
	Texture mCharTexture;
	Character(int tSpriteCount, int tCharWidth, int tCharHeight)
	{

		CHAR_SPRITE_COUNT = tSpriteCount;
		mCharWidth = tCharWidth;
		mCharHeight = tCharHeight;

		//allocating memory for character spritesheet rects
		mCharacterSprite = (SDL_Rect *)malloc(tSpriteCount * sizeof(SDL_Rect));

		mPosX = SCREEN_WIDTH / 2, mPosY = SCREEN_HEIGHT / 2;
		mCharVelX = mCharVelY = 0;
		mCharShape.x = mPosX, mCharShape.y = mPosY;
		mCharShape.w = mCharWidth, mCharShape.h = mCharHeight;
		mCurSprite = 1;
		mFlipType = SDL_FLIP_NONE;

		for (int i = 0; i < CHAR_SPRITE_COUNT; i++)
		{
			mCharacterSprite[i].w = mCharShape.w;
			mCharacterSprite[i].h = mCharShape.h;
			mCharacterSprite[i].x = i * mCharShape.w;
			mCharacterSprite[i].y = 0;
		}
	}
	void positionReset()
	{
		mPosX = SCREEN_WIDTH / 2, mPosY = SCREEN_HEIGHT / 2;
		mCharVelX = mCharVelY = 0;
	}
	void spriteChanger()
	{
		mCurSprite++;
		if (mCurSprite / CHAR_SPEED_DEC >= CHAR_SPRITE_COUNT)
			mCurSprite = 1;
	}
	void handleEvent(SDL_Event &e)
	{
		if (e.type == SDL_KEYDOWN & e.key.repeat == 0)
		{
			switch (e.ksym)
			{
			case SDLK_w:
				mCharVelY -= CHAR_VELOCITY;
				break;
			case SDLK_s:
				mCharVelY += CHAR_VELOCITY;
				break;
			case SDLK_a:
				mCharVelX -= CHAR_VELOCITY;
				break;
			case SDLK_d:
				mCharVelX += CHAR_VELOCITY;
				break;
			default:
				break;
			}
		}
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			switch (e.ksym)
			{
			case SDLK_w:
				mCharVelY += CHAR_VELOCITY;
				break;
			case SDLK_s:
				mCharVelY -= CHAR_VELOCITY;
				break;
			case SDLK_a:
				mCharVelX += CHAR_VELOCITY;
				break;
			case SDLK_d:
				mCharVelX -= CHAR_VELOCITY;
				break;
			default:
				break;
			}
		}
	}

	void move(std::vector<SDL_Rect> objects)
	{
		mPosX += mCharVelX;

		mCharShape.x = mPosX;
		if (mPosX < 0 || (mPosX + mCharWidth > LEVEL_WIDTH) || checkCollision(mCharShape, objects))
		{
			mPosX -= mCharVelX;
			mCharShape.x = mPosX;
		}
		mPosY += mCharVelY;
		mCharShape.y = mPosY;
		if (mPosY < 0 || (mPosY + mCharHeight > LEVEL_HEIGHT) || checkCollision(mCharShape, objects))
		{
			mPosY -= mCharVelY;
			mCharShape.y = mPosY;
		}
	}
	void render(int camX, int camY)
	{
		mCharTexture.render(mPosX - camX, mPosY - camY, &mCharacterSprite[mCurSprite / CHAR_SPEED_DEC], mFlipType);
	}
	void free()
	{
		mCharTexture.free();
	}
};

//Game state paused or initial menu
BUTTONS gameState = PLAY;

SDL_Rect gCamera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; //Dynamic map variables
SDL_Rect gTaskPosition[NUMBER_OF_TASKS];				//Task positions on map
SDL_Rect gButtonPosition[NUMBER_OF_BUTTONS];

bool gIfTaskComplete[NUMBER_OF_TASKS];
bool gIfResume = false; //Checks if the game is resumed or not
bool gIfMusic = false;

int gTaskScore[NUMBER_OF_TASKS];
int gRequiredTaskScore[NUMBER_OF_TASKS]; //required score in each game
int gCharCurPosX, gCharCurPosY;			 //Variable to check the change in characters position
int gCOIN_COUNT = 5;					 //5 coins for new users. (will be loaded from file for older users)

//struct variables
CoinAnimation gTaskCoinA(8, 40, 40, 4);
Character gMyCharacter(10, 32, 65);

const int BUILDING_COUNT = 4;
const int TREE_COUNT = 3;

//Texture variables
Texture gBackgroundTexture;
Texture gBuildingTexture[BUILDING_COUNT];
Texture gTreesTexture[TREE_COUNT];
Texture gTimeTexture;
Texture gUI_BackgroundTexture;
Texture gUI_ButtonsTexture[NUMBER_OF_BUTTONS];
Texture gUI_LoginBackgroundTexture;
Texture gUI_LoginEnterTexture;
Texture gUI_RegisterEnterTexture;
Texture gCoinCountTexture;
Texture gIdNameTexture;

std::vector<SDL_Rect> gMapObjects;					 //List of objects in the room for collision detection
std::vector<std::pair<int, int>> gBuildingPositions; //Coordinates for placing trees and buildings on the map
std::vector<std::tuple<int, int, int>> gTreePositions;
std::string gCurrentUsername = "NOT_LOGGED_IN";
//global timer
Timer gTimer;

bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ERROR;
		return false;
	}
	gWindow = SDL_CreateWindow("kms", SDL_WINDOWPOS_CENTERED,
							   SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		ERROR;
		return false;
	}
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Linear texture filtering not enabled\n");
	}
	gRender = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRender == NULL)
	{
		ERROR;
		return false;
	}
	SDL_SetRenderDrawColor(gRender, 0xFF, 0xFF, 0xFF, 0xFF);
	if (!IMG_Init(IMG_INIT_PNG))
	{
		ERROR_I;
		return false;
	}
	if (TTF_Init() == -1)
	{
		ERROR_T;
		return false;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		ERROR_M;
		return false;
	}
	return true;
}

//Checks collision between objects
bool checkCollision(SDL_Rect player, std::vector<SDL_Rect> objects)
{
	bool flag = false;
	//getting the relative position of player
	player.x -= gCamera.x;
	player.y -= gCamera.y;
	player.w -= 10;
	player.h -= 5;
	for (int i = 0; i < (int)objects.size(); i++)
	{
		bool tFlag = true;
		objects[i].x -= gCamera.x, objects[i].y -= gCamera.y;
		if (player.x + player.w <= objects[i].x)
			tFlag = false;
		if (player.x >= objects[i].x + objects[i].w)
			tFlag = false;
		if (player.y + player.h <= objects[i].y)
			tFlag = false;
		if (player.y >= objects[i].y + objects[i].h)
			tFlag = false;
		if (tFlag)
		{
			flag = true;
			break;
		}
	}
	return flag;
}

//checks collision between two objects
bool checkCollisionRect(SDL_Rect player, SDL_Rect object)
{
	bool tFlag = true;
	player.x -= gCamera.x, player.y -= gCamera.y;
	object.x -= gCamera.x, object.y -= gCamera.y;
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

void closeAll()
{
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRender);
	Mix_FreeMusic(gMusic);
	TTF_CloseFont(gFont);
	gTimer.stop();
	gMyCharacter.free();
	gBackgroundTexture.free();
	gTaskCoinA.free();
	gTimeTexture.free();

	for (int i = 0; i < BUILDING_COUNT; i++)
		gBuildingTexture[i].free();
	for (int i = 0; i < TREE_COUNT; i++)
		gTreesTexture[i].free();

	gMapObjects.clear();
	gBuildingPositions.clear();
	gTreePositions.clear();

	gWindow = NULL;
	gRender = NULL;
	gMusic = NULL;
	gFont = NULL;
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	Mix_Quit();
}

bool loadMedia()
{
	if (!gMyCharacter.mCharTexture.loadFile("images/main/walk1.png"))
		return false;
	if (!gBackgroundTexture.loadFile("images/main/background1.png"))
		return false;
	if (!gBuildingTexture[0].loadFile("images/main/arcade1.png"))
		return false;
	if (!gBuildingTexture[1].loadFile("images/main/arcade2.png"))
		return false;
	if (!gBuildingTexture[2].loadFile("images/main/arcade4.png"))
		return false;
	if (!gBuildingTexture[3].loadFile("images/main/arcade3.png"))
		return false;
	if (!gTreesTexture[0].loadFile("images/main/tree1.png"))
		return false;
	if (!gTreesTexture[1].loadFile("images/main/tree2.png"))
		return false;
	if (!gTreesTexture[2].loadFile("images/main/tree3.png"))
		return false;
	if (!gTaskCoinA.mCoinTexture.loadFile("images/main/coin.png"))
		return false;
	if (!gUI_BackgroundTexture.loadFile("images/main/ui_back.png"))
		return false;
	if (!gUI_ButtonsTexture[PLAY].loadFile("images/main/play_button.png"))
		return false;
	if (!gUI_ButtonsTexture[CONTINUE].loadFile("images/main/continue_button.png"))
		return false;
	if (!gUI_ButtonsTexture[EXIT].loadFile("images/main/exit_button.png"))
		return false;
	if (!gUI_ButtonsTexture[LOGIN].loadFile("images/main/login_button.png"))
		return false;
	if (!gUI_ButtonsTexture[REGISTER].loadFile("images/main/register_button.png"))
		return false;
	if (!gUI_ButtonsTexture[HELP].loadFile("images/main/help_button.png"))
		return false;
	if (!gUI_ButtonsTexture[BACK].loadFile("images/main/back_button.png"))
		return false;
	if (!gUI_LoginBackgroundTexture.loadFile("images/main/login_back.png"))
		return false;
	if (!gUI_LoginEnterTexture.loadFile("images/main/login_enter.png"))
		return false;
	if (!gUI_RegisterEnterTexture.loadFile("images/main/register_enter.png"))
		return false;
	if (!gUI_ButtonsTexture[VOLUME_ON].loadFile("images/main/music_on.png"))
		return false;
	if (!gUI_ButtonsTexture[VOLUME_OFF].loadFile("images/main/music_off.png"))
		return false;
	gFont = TTF_OpenFont("images/fonts/Oswald-Medium.ttf", 21);
	if (gFont == NULL)
	{
		ERROR_T;
		return false;
	}
	gMusic = Mix_LoadMUS("sounds/background.wav");
	if (gMusic == NULL)
	{
		ERROR_M;
		return false;
	}
	//chaning cursor inside the game
	std::string pathCursor = "images/main/cursor.png";
	SDL_Surface *myCursorSurface = IMG_Load(pathCursor.c_str());
	SDL_Cursor *myCursor = SDL_CreateColorCursor(myCursorSurface, 0, 0);
	SDL_SetCursor(myCursor);

	return true;
}

//Initializes object positions on map
void gameInitialize()
{
	//starting timer
	gTimer.start();

	SDL_Rect tRect;
	//initializing building positions

	tRect.x = 950, tRect.y = 550;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[0].getWidth() - gMyCharacter.mCharWidth, tRect.h = gBuildingTexture[0].getHeight() - gMyCharacter.mCharHeight;
	gMapObjects.push_back(tRect);

	tRect.x = 150, tRect.y = 500;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[1].getWidth() - gMyCharacter.mCharWidth, tRect.h = gBuildingTexture[1].getHeight() - gMyCharacter.mCharHeight;
	gMapObjects.push_back(tRect);

	tRect.x = 100, tRect.y = 200;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[2].getWidth() - gMyCharacter.mCharWidth, tRect.h = gBuildingTexture[2].getHeight() - 20;
	gMapObjects.push_back(tRect);

	tRect.x = 1120, tRect.y = 150;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[3].getWidth() - gMyCharacter.mCharWidth, tRect.h = gBuildingTexture[3].getHeight() - 20;
	gMapObjects.push_back(tRect);

	//Rendering trees on map

	int tempHeight[] = {30, 70, 97};
	int st = 240;
	for (int i = st, j = 0, k = 0; k <= 3; i += 210, j = (j + 1) % TREE_COUNT)
	{
		tRect.x = i, tRect.y = tempHeight[j];
		gTreePositions.push_back(std::make_tuple(j, tRect.x, tRect.y));
		tRect.w = gTreesTexture[j].getWidth() - 10, tRect.h = gTreesTexture[j].getHeight() - 10;
		gMapObjects.push_back(tRect);
		if (i >= LEVEL_WIDTH)
		{
			k++;
			if (k == 1)
				st = 430, tempHeight[0] = 240, tempHeight[1] = 280, tempHeight[2] = 330;
			if (k == 2)
				st = 411, tempHeight[0] = 450, tempHeight[1] = 500, tempHeight[2] = 450;
			if (k == 3)
				st = 40, tempHeight[0] = 790, tempHeight[1] = 700, tempHeight[2] = 680;
			i = st - 210;
		}
	}

	//initializing task positions on map
	gTaskPosition[NO_GAME] = {-1, -1, -1, -1};
	gIfTaskComplete[NO_GAME] = true;

	gTaskPosition[DINORUN].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[DINORUN].w = gTaskCoinA.mCoinHeight;
	gTaskPosition[DINORUN].x = gBuildingPositions[0].first + gBuildingTexture[0].getWidth() / 2 - gTaskCoinA.mCoinWidth / 2;
	gTaskPosition[DINORUN].y = gBuildingPositions[0].second + gBuildingTexture[0].getHeight() - gTaskCoinA.mCoinHeight;

	gTaskPosition[BUCKET_BALL].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[BUCKET_BALL].w = gTaskCoinA.mCoinHeight;
	gTaskPosition[BUCKET_BALL].x = gBuildingPositions[1].first + gBuildingTexture[1].getWidth() / 2 - gTaskCoinA.mCoinWidth / 2;
	gTaskPosition[BUCKET_BALL].y = gBuildingPositions[1].second + gBuildingTexture[1].getHeight() - gTaskCoinA.mCoinHeight;

	gTaskPosition[PACMAN].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[PACMAN].w = gTaskCoinA.mCoinHeight;
	gTaskPosition[PACMAN].x = gBuildingPositions[2].first + gBuildingTexture[2].getWidth() / 2 - gTaskCoinA.mCoinWidth / 2;
	gTaskPosition[PACMAN].y = gBuildingPositions[2].second + gBuildingTexture[2].getHeight() - gTaskCoinA.mCoinHeight;

	gTaskPosition[TOWERGAME].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[TOWERGAME].w = gTaskCoinA.mCoinHeight;
	gTaskPosition[TOWERGAME].x = gBuildingPositions[3].first + gBuildingTexture[3].getWidth() / 2 - gTaskCoinA.mCoinWidth / 2;
	gTaskPosition[TOWERGAME].y = gBuildingPositions[3].second + gBuildingTexture[3].getHeight() - gTaskCoinA.mCoinHeight;

	gRequiredTaskScore[BUCKET_BALL] = 100;
	gRequiredTaskScore[PACMAN] = 5;
	gRequiredTaskScore[TOWERGAME] = 400;
	gRequiredTaskScore[DINORUN] = 5000;

	//Initializing UI button positions
	for (int i = 0; i < NUMBER_OF_BUTTONS - 3; i++)
	{
		gButtonPosition[i].x = SCREEN_WIDTH / 2 - 309 / 2;
		gButtonPosition[i].y = (i - 1) * (i != 0) * 100 + 100;
		gButtonPosition[i].w = 309;
		gButtonPosition[i].h = 55;
	}
	gButtonPosition[CONTINUE] = gButtonPosition[PLAY];
	gButtonPosition[BACK].x = 0, gButtonPosition[BACK].y = 0;
	gButtonPosition[BACK].w = 50, gButtonPosition[BACK].h = 50;

	gButtonPosition[VOLUME_ON].x = SCREEN_WIDTH - gUI_ButtonsTexture[VOLUME_ON].getWidth();
	gButtonPosition[VOLUME_ON].y = 0;
	gButtonPosition[VOLUME_ON].w = gUI_ButtonsTexture[VOLUME_ON].getWidth();
	gButtonPosition[VOLUME_ON].h = gUI_ButtonsTexture[VOLUME_ON].getHeight();

	gButtonPosition[VOLUME_OFF] = gButtonPosition[VOLUME_ON];
}

//Rendering all the objects in the map
void renderMapObjects()
{
	//Set Camera position
	gCamera.x = (gMyCharacter.mPosX + gMyCharacter.mCharShape.w / 2) - SCREEN_WIDTH / 2;
	gCamera.y = (gMyCharacter.mPosY + gMyCharacter.mCharShape.h / 2) - SCREEN_HEIGHT / 2;
	if (gCamera.x < 0)
		gCamera.x = 0;
	if (gCamera.y < 0)
		gCamera.y = 0;
	if (gCamera.x > LEVEL_WIDTH - gCamera.w)
		gCamera.x = LEVEL_WIDTH - gCamera.w;
	if (gCamera.y > LEVEL_HEIGHT - gCamera.h)
		gCamera.y = LEVEL_HEIGHT - gCamera.h;

	SDL_SetRenderDrawColor(gRender, 127, 0, 127, 255);
	SDL_RenderClear(gRender);
	//render background
	gBackgroundTexture.render(0, 0, &gCamera);

	//building render
	for (int i = 0; i < BUILDING_COUNT; i++)
		gBuildingTexture[i].render(gBuildingPositions[i].first - gCamera.x, gBuildingPositions[i].second - gCamera.y);
	//Tree Render
	for (int i = 0; i < gTreePositions.size(); i++)
	{
		gTreesTexture[std::get<0>(gTreePositions[i])].render(std::get<1>(gTreePositions[i]) - gCamera.x, std::get<2>(gTreePositions[i]) - gCamera.y);
	}

	//rendering coin animation at task positions
	for (int i = 0; i < NUMBER_OF_TASKS; i++)
	{
		if (!gIfTaskComplete[i])
			gTaskCoinA.render(gTaskPosition[i].x - gCamera.x, gTaskPosition[i].y - gCamera.y);
	}

	//Checking if player has moved
	if (gCharCurPosX != gMyCharacter.mPosX || gCharCurPosY != gMyCharacter.mPosY)
	{
		gMyCharacter.spriteChanger();

		if (gCharCurPosX > gMyCharacter.mPosX)
			gMyCharacter.mFlipType = SDL_FLIP_HORIZONTAL;
		else if (gCharCurPosX < gMyCharacter.mPosX)
			gMyCharacter.mFlipType = SDL_FLIP_NONE;

		//update character position
		gCharCurPosX = gMyCharacter.mPosX;
		gCharCurPosY = gMyCharacter.mPosY;
	}
	else
	{
		gMyCharacter.mCurSprite = 0;
	}

	//rendering time prompt
	SDL_Color textColor = {105, 10, 0, 255};
	// std::stringstream timerText;
	// timerText << "Timer : " << gTimer.getTicks() / 1000;
	// gTimeTexture.loadFromText(timerText.str().c_str(), textColor);
	// gTimeTexture.render(SCREEN_WIDTH - 100, 0);

	std::stringstream tempText;
	// tempText << "Coins : " << gCOIN_COUNT;
	// gCoinCountTexture.loadFromText(tempText.str().c_str(), textColor);
	// gCoinCountTexture.render(0, 0);

	tempText.str("");
	tempText << "User :" << gCurrentUsername;
	gIdNameTexture.loadFromText(tempText.str().c_str(), textColor);
	gIdNameTexture.render(0, 0);

	gMyCharacter.render(gCamera.x, gCamera.y);
}

//runs a specific task depending on the players position
void taskHandler()
{
	TASK_NAME whichTask = NO_GAME;
	for (int i = BUCKET_BALL; i < NUMBER_OF_TASKS; i++)
	{
		if (checkCollisionRect(gMyCharacter.mCharShape, gTaskPosition[i]))
			whichTask = (TASK_NAME)i;
	}
	if (whichTask != NO_GAME && gCOIN_COUNT > 0)
	{
		if (gIfMusic)
		{
			Mix_HaltMusic();
		}
		gCOIN_COUNT -= 1;
	}
	int curTaskScore = 0;
	if (whichTask == BUCKET_BALL)
		curTaskScore = bucketBall(gCurrentUsername);
	if (whichTask == PACMAN)
		curTaskScore = pacmanLite(gCurrentUsername);
	if (whichTask == DINORUN)
		curTaskScore = dinoRun(gCurrentUsername);
	if (whichTask == TOWERGAME)
		curTaskScore = towerGame(gCurrentUsername);
	if (whichTask != NO_GAME)
		gMyCharacter.positionReset();
	gTaskScore[whichTask] = curTaskScore;

	if (gIfMusic && Mix_PlayingMusic() == 0)
	{
		Mix_PlayMusic(gMusic, 0);
	}
}

MENU_OPTIONS handleUI(SDL_Event &e)
{

	bool quit = false;
	int menuScroll = 0;
	while (!quit)
	{

		//Control Music
		if (Mix_PlayingMusic() == 0 && gIfMusic)
		{
			Mix_PlayMusic(gMusic, 0);
		}
		else if (!gIfMusic)
		{
			Mix_HaltMusic();
		}

		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		// Mouse hover animation on button

		for (int i = 0; i < NUMBER_OF_BUTTONS; i++)
		{
			gUI_ButtonsTexture[i].setAlpha(255);
			if (mouseX >= gButtonPosition[i].x && mouseX <= gButtonPosition[i].x + gButtonPosition[i].w && mouseY >= gButtonPosition[i].y && mouseY <= gButtonPosition[i].y + gButtonPosition[i].h)
			{
				gUI_ButtonsTexture[i].setAlpha(200);
			}
		}

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return FULL_EXIT;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{

				if (mouseX >= gButtonPosition[PLAY].x && mouseX <= gButtonPosition[PLAY].x + gButtonPosition[PLAY].w && mouseY >= gButtonPosition[PLAY].y && mouseY <= gButtonPosition[PLAY].y + gButtonPosition[PLAY].h)
					return START_GAME;

				if (mouseX >= gButtonPosition[EXIT].x && mouseX <= gButtonPosition[EXIT].x + gButtonPosition[EXIT].w && mouseY >= gButtonPosition[EXIT].y && mouseY <= gButtonPosition[EXIT].y + gButtonPosition[EXIT].h)
					return FULL_EXIT;

				if (mouseX >= gButtonPosition[LOGIN].x && mouseX <= gButtonPosition[LOGIN].x + gButtonPosition[LOGIN].w && mouseY >= gButtonPosition[LOGIN].y && mouseY <= gButtonPosition[LOGIN].y + gButtonPosition[LOGIN].h)
					return LOGIN_MENU;

				if (mouseX >= gButtonPosition[HELP].x && mouseX <= gButtonPosition[HELP].x + gButtonPosition[HELP].w && mouseY >= gButtonPosition[HELP].y && mouseY <= gButtonPosition[HELP].y + gButtonPosition[HELP].h)
					return HELP_MENU;

				if (mouseX >= gButtonPosition[VOLUME_ON].x && mouseX <= gButtonPosition[VOLUME_ON].x + gButtonPosition[VOLUME_ON].w && mouseY >= gButtonPosition[VOLUME_ON].y && mouseY <= gButtonPosition[VOLUME_ON].y + gButtonPosition[VOLUME_ON].h)
				{
					gIfMusic = !gIfMusic;
				}
				if (mouseX >= gButtonPosition[REGISTER].x && mouseX <= gButtonPosition[REGISTER].x + gButtonPosition[REGISTER].w && mouseY >= gButtonPosition[REGISTER].y && mouseY <= gButtonPosition[REGISTER].y + gButtonPosition[REGISTER].h)
				{
					return REGISTER_MENU;
				}
			}
		}
		//render UI
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);

		menuScroll -= 2;
		if (menuScroll < -SCREEN_WIDTH)
			menuScroll = 0;
		gUI_BackgroundTexture.render(menuScroll, 0);
		gUI_BackgroundTexture.render(menuScroll + SCREEN_WIDTH, 0);
		int st = 0, skip = -1;
		if (gIfResume)
		{
			st = 1;
		}
		else
		{
			skip = 1;
		}

		for (int i = st; i < NUMBER_OF_BUTTONS - 3; i++)
		{
			if (skip != -1 && i == skip)
				continue;
			int pos = i;
			if (i == 1)
				pos = i - st;
			gUI_ButtonsTexture[i].render(gButtonPosition[pos].x, gButtonPosition[pos].y);
		}

		//render volume button
		BUTTONS tButton;
		if (gIfMusic)
		{
			tButton = VOLUME_ON;
		}
		else
			tButton = VOLUME_OFF;
		gUI_ButtonsTexture[tButton].render(gButtonPosition[tButton].x, gButtonPosition[VOLUME_ON].y);

		SDL_RenderPresent(gRender);
	}
	return FULL_EXIT;
}

MENU_OPTIONS loginRegisterUI(SDL_Event &e, MENU_OPTIONS tMenu)
{
	bool quit = false;
	int invalidPromptDelay = 0;
	SDL_Color textColor = {255, 255, 255, 255};
	SDL_Rect loginEnter = {570, 200, 150, 27};
	Texture inputTexture, promptTexture, invalidTexture;
	std::string inputText;
	if (tMenu == LOGIN_MENU)
		inputText = "Username";
	else
		inputText = "Enter Username";
	promptTexture.loadFromText(inputText.c_str(), textColor);
	inputText = "Invalid Username";
	invalidTexture.loadFromText(inputText.c_str(), textColor);
	inputText = "";
	while (!quit)
	{
		bool ifRender = false;
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		if (mouseX >= loginEnter.x && mouseX <= loginEnter.x + loginEnter.w && mouseY >= loginEnter.y && mouseY <= loginEnter.y + loginEnter.h)
		{
			gUI_LoginEnterTexture.setAlpha(200);
			gUI_RegisterEnterTexture.setAlpha(200);
		}
		else
		{
			gUI_LoginEnterTexture.setAlpha(255);
			gUI_RegisterEnterTexture.setAlpha(255);
		}

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return FULL_EXIT;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (mouseX >= gButtonPosition[BACK].x && mouseX <= gButtonPosition[BACK].x + gButtonPosition[BACK].w && mouseY >= gButtonPosition[BACK].y && mouseY <= gButtonPosition[BACK].y + gButtonPosition[BACK].h)
				{
					return LOADING_SCREEN;
				}

				else if (mouseX >= loginEnter.x && mouseX <= loginEnter.x + loginEnter.w && mouseY >= loginEnter.y && mouseY <= loginEnter.y + loginEnter.h)
				{
					if (tMenu == LOGIN_MENU)
					{
						//check username
						bool flag = false;
						std::ifstream usernameFile;
						usernameFile.open("saved_files/username.names");
						if (!usernameFile)
						{
							printf("Could not open file\n");
						}
						std::string tStr;
						int tCoin;

						while (usernameFile.eof() == false)
						{
							usernameFile >> tStr >> tCoin;
							if (tStr == inputText)
							{
								flag = true;
								gCOIN_COUNT = tCoin;
								gCurrentUsername = tStr;
							}
							// std::cout<<tStr<<"----"<<tCoin<<"\n";
						}
						usernameFile.close();
						if (flag)
							return START_GAME;
						else
						{
							invalidPromptDelay = 50;
						}
					}
					else if (tMenu == REGISTER_MENU)
					{
						std::ofstream usernameFile;
						usernameFile.open("saved_files/username.names", std::ios_base::app);
						if (!usernameFile)
						{
							printf("Could not open file\n");
						}
						usernameFile<< inputText << " 5" <<"\n";
						gCOIN_COUNT = 5;
						gCurrentUsername = inputText;
						usernameFile.close();
						return START_GAME;
					}
				}
			}

			else if (e.type == SDL_KEYDOWN)
			{
				if (e.ksym == SDLK_ESCAPE)
				{
					return LOADING_SCREEN;
				}
				else if (e.ksym == SDLK_BACKSPACE && inputText.size() > 0)
				{
					inputText.pop_back();
					ifRender = true;
				}
				else if (e.ksym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
				{
					SDL_SetClipboardText(inputText.c_str());
				}
				else if (e.ksym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
				{
					inputText = SDL_GetClipboardText();
					ifRender = true;
				}
			}
			else if (e.type == SDL_TEXTINPUT)
			{
				if (!(SDL_GetModState() & KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
				{
					inputText += e.text.text;
					ifRender = true;
				}
			}
		}
		if (ifRender)
		{
			if (inputText != "")
				inputTexture.loadFromText(inputText.c_str(), textColor);
			else
				inputTexture.loadFromText(" ", textColor);
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);

		gUI_LoginBackgroundTexture.render(0, 0);
		// printf("%d\n",invalidPromptDelay);
		if (invalidPromptDelay > 0)
		{
			invalidTexture.render(540, 50);
			invalidPromptDelay -= 1;
		}
		else
		{
			promptTexture.render(600, 50);
		}
		SDL_Rect usernameBox = {450, 80, 400, 65};
		SDL_RenderDrawRect(gRender, &usernameBox);
		inputTexture.render(450, 80);
		if (tMenu == LOGIN_MENU)
			gUI_LoginEnterTexture.render(loginEnter.x, loginEnter.y);
		else if (tMenu == REGISTER_MENU)
			gUI_RegisterEnterTexture.render(loginEnter.x, loginEnter.y);
		gUI_ButtonsTexture[BACK].render(gButtonPosition[BACK].x, gButtonPosition[BACK].y);
		SDL_RenderPresent(gRender);
	}
	return LOGIN_MENU;
}

int main(int argc, char *argv[])
{
	srand(time(0));

	if (!init())
	{
		printf("Failed to initialize\n");
		return 0;
	}

	if (!loadMedia())
	{
		printf("Failed to load media\n");
		return 0;
	}

	//===================
	//  int t = towerGame(gCurrentUsername);

	//Loading map object position and shape
	gameInitialize();

	SDL_Event e;

	//initial character position to track movement
	gCharCurPosX = gMyCharacter.mPosX;
	gCharCurPosY = gMyCharacter.mPosY;

	bool game_quit = false;
	while (!game_quit)
	{

		bool quit = true;
		MENU_OPTIONS menuState = LOADING_SCREEN;

		if (menuState == LOADING_SCREEN)
		{
			menuState = handleUI(e);
		}
		if (menuState == LOGIN_MENU)
		{
			menuState = loginRegisterUI(e, menuState);
		}
		if (menuState == REGISTER_MENU)
		{
			menuState = loginRegisterUI(e, menuState);
		}
		if (menuState == FULL_EXIT)
			game_quit = true;
		if (menuState == START_GAME)
		{
			quit = false;
			gIfResume = true;
		}

		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
				{
					quit = true;
				}
				else if (e.type == SDL_QUIT)
				{
					quit = true;
					game_quit = true;
				}
				gMyCharacter.handleEvent(e);
			}
			gMyCharacter.move(gMapObjects);

			// printf("%d %d\n",gMyCharacter.mPosX,gMyCharacter.mPosY);

			//running a task if in correct position
			taskHandler();

			//rendering all the objects on map
			renderMapObjects();

			SDL_RenderPresent(gRender);
		}
	}
	closeAll();
	return 0;
}
