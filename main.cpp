#include "texture_timer.h"
#include "bucket_ball.h"
#include "pacman.h"

enum TASK_NAME
{
	NO_GAME,
	BUCKET_BALL,
	PACMAN,
	NUMBER_OF_TASKS
};

//function prototypes
bool init();
bool loadMedia();
bool checkCollision(SDL_Rect player, std::vector<SDL_Rect> objects);
void closeAll();
void renderRoom1Objects();
void gameInitialize();
void taskHandler();

//structure for coin animation handling
struct CoinAnimation
{
	Texture mCoinTexture;

	int mSPRITE_COUNT;
	int mSpriteLevel; //number of images in a row
	int mCoinWidth;
	int mCointHeight;

	int mCurSprite = 0;
	const int mSPEED_DEC = 6;

	SDL_Rect *mCoinSprite;
	CoinAnimation(int tSpriteCount, int tCoinWidth, int tCoinHeight, int tSpriteLevel)
	{
		mSPRITE_COUNT = tSpriteCount;
		mCoinWidth = tCoinWidth;
		mCointHeight = tCoinHeight;
		mSpriteLevel = tSpriteLevel;

		mCoinSprite = (SDL_Rect *)malloc(tSpriteCount * sizeof(SDL_Rect));

		for (int i = 0, xi = 0; i < mSPRITE_COUNT; i++, xi++)
		{
			mCoinSprite[i].w = mCoinWidth;
			mCoinSprite[i].h = mCointHeight;
			mCoinSprite[i].x = xi * mCoinWidth;
			if (i >= mSpriteLevel)
				xi = 0;
			mCoinSprite[i].y = (i / mSpriteLevel) * mCointHeight;
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
	int mCharPosX, mCharPosY;
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

		mCharPosX = SCREEN_WIDTH / 2, mCharPosY = SCREEN_HEIGHT / 2;
		mCharVelX = mCharVelY = 0;
		mCharShape.x = mCharPosX, mCharShape.y = mCharPosY;
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
		mCharPosX = SCREEN_WIDTH / 2, mCharPosY = SCREEN_HEIGHT / 2;
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
		mCharPosX += mCharVelX;

		mCharShape.x = mCharPosX;
		if (mCharPosX < 0 || (mCharPosX + mCharWidth > SCREEN_WIDTH) || checkCollision(mCharShape, objects))
		{
			mCharPosX -= mCharVelX;
			mCharShape.x = mCharPosX;
		}
		mCharPosY += mCharVelY;
		mCharShape.y = mCharPosY;
		if (mCharPosY < 0 || (mCharPosY + mCharHeight > SCREEN_HEIGHT) || checkCollision(mCharShape, objects))
		{
			mCharPosY -= mCharVelY;
			mCharShape.y = mCharPosY;
		}
	}
	void render()
	{
		mCharTexture.render(mCharPosX, mCharPosY, &mCharacterSprite[mCurSprite / CHAR_SPEED_DEC], mFlipType);
	}
	void free()
	{
		mCharTexture.free();
	}
};

//varaibles for task management
SDL_Rect gTaskPosition[NUMBER_OF_TASKS];
bool gIfTaskComplete[NUMBER_OF_TASKS];
int gTaskScore[NUMBER_OF_TASKS];
int gRequiredTaskScore[NUMBER_OF_TASKS];

//struct variables
CoinAnimation gTaskCoinA(8, 40, 40, 4);
Character gMyCharacter(10, 32, 65);

const int gBuildingCount = 3;
const int gTreeCount = 3;

//Variable to check the change in characters position
int charCurPosX, charCurPosY;

//Texture variables
//Texture files (background and decorative objects)
Texture gBackgroundTexture;
Texture gBushTexture;
Texture gBuildingTexture[gBuildingCount];
Texture gTreesTexture[gTreeCount];
Texture gTimeTexture;

//List of objects in the room for collision detection
std::vector<SDL_Rect> roomOneObjects;
std::vector<SDL_Rect> gBushTextureSprite;

//Cordinates for placing trees and buildings on the map
std::vector<std::pair<int, int>> gBuildingPositions;
std::vector<std::tuple<int, int, int>> gTreePositions;

//global timer
Timer gTimer;

bool loadMedia()
{
	if (!gMyCharacter.mCharTexture.loadFile("images/png/walk1.png"))
		return false;
	if (!gBackgroundTexture.loadFile("images/png/background1.png"))
		return false;
	if (!gBuildingTexture[0].loadFile("images/png/house2.png", true, 0, 64, 128))
		return false;
	if (!gBuildingTexture[1].loadFile("images/png/house1_5.png"))
		return false;
	if (!gBuildingTexture[2].loadFile("images/png/house1_4.png"))
		return false;
	if (!gTreesTexture[0].loadFile("images/png/tree1.png"))
		return false;
	if (!gTreesTexture[1].loadFile("images/png/tree2.png"))
		return false;
	if (!gTreesTexture[2].loadFile("images/png/tree3.png"))
		return false;
	if (!gBushTexture.loadFile("images/png/bushAll.png"))
		return false;
	if (!gTaskCoinA.mCoinTexture.loadFile("images/png/coin.png"))
		return false;
	gFont = TTF_OpenFont("images/fonts/Oswald-BoldItalic.ttf", 24);
	if (gFont == NULL)
	{
		ERROR_T;
		return false;
	}
	return true;
}

void gameInitialize()
{
	//starting timer
	gTimer.start();

	SDL_Rect tRect;
	//initializing building positions

	tRect.x = 900, tRect.y = 450;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[0].getWidth() - 20, tRect.h = gBuildingTexture[0].getHeight() - gMyCharacter.mCharHeight;
	roomOneObjects.push_back(tRect);

	tRect.x = 120, tRect.y = 500;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[1].getWidth() - 20, tRect.h = gBuildingTexture[1].getHeight() - gMyCharacter.mCharHeight;
	roomOneObjects.push_back(tRect);

	tRect.x = 100, tRect.y = 180;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[2].getWidth() - 20, tRect.h = gBuildingTexture[2].getHeight() - 20;
	roomOneObjects.push_back(tRect);

	//initializing bush positions on map

	for (int i = 0, xi = 0; i < 6; i++, xi++)
	{
		tRect.x = xi * gBushTexture.getWidth() / 3;
		if (i >= 3)
			xi = 0;
		tRect.y = (i / 3) * gBushTexture.getHeight() / 3;
		tRect.w = gBushTexture.getWidth() / 3, tRect.h = gBushTexture.getHeight() / 3;
		gBushTextureSprite.push_back(tRect);
	}
	for (int i = 50; i <= 1000; i += 190)
	{
		tRect.x = i, tRect.y = 30;
		gTreePositions.push_back(std::make_tuple(i % gTreeCount, tRect.x, tRect.y));
		tRect.w = gTreesTexture[i % gTreeCount].getWidth(), tRect.h = gTreesTexture[i % gTreeCount].getHeight();
		roomOneObjects.push_back(tRect);
	}
	for (int i = 311; i <= 1100; i += 211)
	{
		tRect.x = i, tRect.y = 350;
		gTreePositions.push_back(std::make_tuple(i % gTreeCount, tRect.x, tRect.y));
		tRect.w = gTreesTexture[i % gTreeCount].getWidth(), tRect.h = gTreesTexture[i % gTreeCount].getHeight();
		roomOneObjects.push_back(tRect);
	}

	//initializing task positons on map
	gTaskPosition[NO_GAME] = {-1, -1, -1, -1};
	gIfTaskComplete[NO_GAME] = true;

	gTaskPosition[BUCKET_BALL].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[BUCKET_BALL].w = gTaskCoinA.mCointHeight;
	gTaskPosition[BUCKET_BALL].x = gBuildingPositions[1].first + 41;
	gTaskPosition[BUCKET_BALL].y = gBuildingPositions[1].second + 120;

	gTaskPosition[PACMAN].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[PACMAN].w = gTaskCoinA.mCointHeight;
	gTaskPosition[PACMAN].x = gBuildingPositions[2].first + 33;
	gTaskPosition[PACMAN].y = gBuildingPositions[2].second + 160;

	gRequiredTaskScore[BUCKET_BALL] = 100;
	gRequiredTaskScore[PACMAN] = 5;
}

bool checkCollision(SDL_Rect player, std::vector<SDL_Rect> objects)
{
	bool flag = false;
	for (int i = 0; i < (int)objects.size(); i++)
	{
		bool tFlag = true;
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

void closeAll()
{
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRender);
	Mix_FreeMusic(gMusic);
	gTimer.stop();
	gMyCharacter.free();
	gBackgroundTexture.free();
	gTaskCoinA.free();
	gTimeTexture.free();

	for (int i = 0; i < gBuildingCount; i++)
		gBuildingTexture[i].free();
	for (int i = 0; i < gTreeCount; i++)
		gTreesTexture[i].free();
	gBushTexture.free();

	roomOneObjects.clear();
	gBushTextureSprite.clear();
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

void renderRoom1Objects()
{
	SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
	SDL_RenderClear(gRender);

	gBackgroundTexture.render(0, 0);

	//bush rendering
	int bushHeigt1 = 80, bushHeigt2 = 400;
	for (int i = 100; i <= 200; i += 41)
		gBushTexture.render(i, bushHeigt1, &gBushTextureSprite[i % 5]);
	for (int i = 302; i <= 400; i += 41)
		gBushTexture.render(i, bushHeigt1, &gBushTextureSprite[i % 5]);
	for (int i = 470; i <= 570; i += 43)
		gBushTexture.render(i, bushHeigt1, &gBushTextureSprite[i % 5]);
	for (int i = 700; i <= 1000; i += 43)
		gBushTexture.render(i, bushHeigt1, &gBushTextureSprite[i % 5]);
	for (int i = 150; i <= 300; i += 41)
		gBushTexture.render(i, bushHeigt2, &gBushTextureSprite[i % 5]);
	for (int i = 350; i <= 500; i += 41)
		gBushTexture.render(i, bushHeigt2, &gBushTextureSprite[i % 5]);
	for (int i = 800; i <= 1030; i += 41)
		gBushTexture.render(i, bushHeigt2, &gBushTextureSprite[i % 5]);

	//building render
	for (int i = 0; i < gBuildingCount; i++)
		gBuildingTexture[i].render(gBuildingPositions[i].first, gBuildingPositions[i].second);
	for (int i = 0; i < gTreePositions.size(); i++)
	{
		gTreesTexture[std::get<0>(gTreePositions[i])].render(std::get<1>(gTreePositions[i]), std::get<2>(gTreePositions[i]));
	}

	//rendering task position objects
	for (int i = 0; i < NUMBER_OF_TASKS; i++)
	{
		if (!gIfTaskComplete[i])
			gTaskCoinA.render(gTaskPosition[i].x, gTaskPosition[i].y);
	}

	//Checking if player has moved
	if (charCurPosX != gMyCharacter.mCharPosX || charCurPosY != gMyCharacter.mCharPosY)
	{
		gMyCharacter.spriteChanger();

		if (charCurPosX > gMyCharacter.mCharPosX)
			gMyCharacter.mFlipType = SDL_FLIP_HORIZONTAL;
		else if (charCurPosX < gMyCharacter.mCharPosX)
			gMyCharacter.mFlipType = SDL_FLIP_NONE;

		//update character position
		charCurPosX = gMyCharacter.mCharPosX;
		charCurPosY = gMyCharacter.mCharPosY;
	}
	else
		gMyCharacter.mCurSprite = 0;

	//rendering time prompt
	SDL_Color textColor = {0, 0, 0, 255};
	std::stringstream gTimerText;
	gTimerText << "Timer : " << gTimer.getTicks() / 1000;
	gTimeTexture.loadFromText(gTimerText.str().c_str(), textColor);
	gTimeTexture.render(SCREEN_WIDTH - 100, 0);

	gMyCharacter.render();
}

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

bool checkCollisionRect(SDL_Rect player, SDL_Rect object)
{
	bool tFlag = true;
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

//runs a specific task

void taskHandler()
{
	TASK_NAME whichTask = NO_GAME;
	for(int i = BUCKET_BALL;i < NUMBER_OF_TASKS;i++){
	if (checkCollisionRect(gMyCharacter.mCharShape, gTaskPosition[i]))
		whichTask = (TASK_NAME)i;
	}
	if (!gIfTaskComplete[whichTask])
	{
		int curTaskScore = 0;
		if(whichTask == BUCKET_BALL)curTaskScore = bucketBall();
		if(whichTask == PACMAN)curTaskScore = pacman();

		gTaskScore[whichTask] = curTaskScore;
		if (gTaskScore[whichTask] >= gRequiredTaskScore[whichTask])
		{
			gIfTaskComplete[whichTask] = true;
		}
		else
			gTaskScore[whichTask] = 0;
		gMyCharacter.positionReset();
	}
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

	//Loading map object position and shape
	gameInitialize();

	//chaning cursor inside the game
	std::string pathCursor = "images/png/cursor.png";
	SDL_Surface *myCursorSurface = IMG_Load(pathCursor.c_str());
	SDL_Cursor *myCursor = SDL_CreateColorCursor(myCursorSurface, 0, 0);
	SDL_SetCursor(myCursor);

	bool quit = false;
	SDL_Event e;

	//initial character position to track movement
	charCurPosX = gMyCharacter.mCharPosX;
	charCurPosY = gMyCharacter.mCharPosY;

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE))
				quit = true;
			gMyCharacter.handleEvent(e);
		}
		gMyCharacter.move(roomOneObjects);

		//running a task if in correct position
		taskHandler();

		//rendering all the objects on map
		renderRoom1Objects();

		SDL_RenderPresent(gRender);
	}
	closeAll();
	return 0;
}
