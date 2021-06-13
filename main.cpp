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

enum TASK_NAME
{
	NO_GAME,
	BUCKET_BALL,
	PACMAN,
	DINORUN,
	TOWERGAME,
	NUMBER_OF_TASKS
};
enum BUTTONS
{
	PLAY,
	CONTINUE,
	LOGIN,
	REGISTER,
	HELP,
	EXIT,
	NUMBER_OF_BUTTONS
};

enum MENU_OPTIONS
{
	FULL_EXIT,
	START_GAME,
	LOGIN_MENU,
	REGISTER_MENU,
	HELP_MENU
};
//Game state paused or initial menu
BUTTONS gameState = PLAY;

SDL_Rect gCamera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; //Dynamic map variables
SDL_Rect gTaskPosition[NUMBER_OF_TASKS];//Task positions on map
SDL_Rect gButtonPosition[NUMBER_OF_BUTTONS];

bool gIfTaskComplete[NUMBER_OF_TASKS];

int gTaskScore[NUMBER_OF_TASKS]; 
int gRequiredTaskScore[NUMBER_OF_TASKS]; //required score in each game
int gCharCurPosX, gCharCurPosY; //Variable to check the change in characters position

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
Texture gUI_Background;
Texture gUI_Buttons[NUMBER_OF_BUTTONS];

std::vector<SDL_Rect> mapObjects;//List of objects in the room for collision detection
std::vector<std::pair<int, int>> gBuildingPositions;//Coordinates for placing trees and buildings on the map
std::vector<std::tuple<int, int, int>> gTreePositions;

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

bool loadMedia()
{
	if (!gMyCharacter.mCharTexture.loadFile("images/main/walk1.png"))return false;
	if (!gBackgroundTexture.loadFile("images/main/background1.png"))return false;
	if (!gBuildingTexture[0].loadFile("images/main/arcade1.png"))return false;
	if (!gBuildingTexture[1].loadFile("images/main/arcade2.png"))return false;
	if (!gBuildingTexture[2].loadFile("images/main/arcade4.png"))return false;
	if (!gBuildingTexture[3].loadFile("images/main/arcade3.png"))return false;
	if (!gTreesTexture[0].loadFile("images/main/tree1.png"))return false;
	if (!gTreesTexture[1].loadFile("images/main/tree2.png"))return false;
	if (!gTreesTexture[2].loadFile("images/main/tree3.png"))return false;
	if (!gTaskCoinA.mCoinTexture.loadFile("images/main/coin.png"))return false;
	if(!gUI_Background.loadFile("images/main/ui_back.png"))return false;
	if(!gUI_Buttons[PLAY].loadFile("images/main/play_button.png"))return false;
	if(!gUI_Buttons[CONTINUE].loadFile("images/main/continue_button.png"))return false;
	if(!gUI_Buttons[EXIT].loadFile("images/main/exit_button.png"))return false;
	if(!gUI_Buttons[LOGIN].loadFile("images/main/login_button.png"))return false;
	if(!gUI_Buttons[REGISTER].loadFile("images/main/register_button.png"))return false;
	gFont = TTF_OpenFont("images/fonts/Oswald-RegularItalic.ttf", 24);
	if (gFont == NULL)
	{
		ERROR_T;
		return false;
	}
	//chaning cursor inside the game
	std::string pathCursor = "images/main/cursor.png";
	SDL_Surface *myCursorSurface = IMG_Load(pathCursor.c_str());
	SDL_Cursor *myCursor = SDL_CreateColorCursor(myCursorSurface, 0, 0);
	SDL_SetCursor(myCursor);

	return true;
}

//Initializes object positions on map and some global variables
void gameInitialize()
{
	//starting timer
	gTimer.start();

	SDL_Rect tRect;
	//initializing building positions

	tRect.x = 950, tRect.y = 550;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[0].getWidth() - gMyCharacter.mCharWidth , tRect.h = gBuildingTexture[0].getHeight() - gMyCharacter.mCharHeight;
	mapObjects.push_back(tRect);

	tRect.x = 150, tRect.y = 500;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[1].getWidth() - gMyCharacter.mCharWidth, tRect.h = gBuildingTexture[1].getHeight() - gMyCharacter.mCharHeight;
	mapObjects.push_back(tRect);

	tRect.x = 100, tRect.y = 200;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[2].getWidth() - gMyCharacter.mCharWidth, tRect.h = gBuildingTexture[2].getHeight() - 20;
	mapObjects.push_back(tRect);

	tRect.x = 700, tRect.y = 200;
	gBuildingPositions.push_back(std::make_pair(tRect.x, tRect.y));
	tRect.w = gBuildingTexture[3].getWidth() - gMyCharacter.mCharWidth, tRect.h = gBuildingTexture[3].getHeight() - 20;
	mapObjects.push_back(tRect);


	//Rendering trees on map

	int tempHeight[] = {30, 70, 97};
	int st = 240;
	for (int i = st, j = 0,k = 0; k <= 3; i += 210, j = (j + 1) % TREE_COUNT)
	{
		tRect.x = i, tRect.y = tempHeight[j];
		gTreePositions.push_back(std::make_tuple(j, tRect.x, tRect.y));
		tRect.w = gTreesTexture[j].getWidth() - 10, tRect.h = gTreesTexture[j].getHeight() - 10;
		mapObjects.push_back(tRect);
		if(i >= LEVEL_WIDTH){
			k++;
			if(k == 1)st = 430,tempHeight[0] = 240, tempHeight[1] = 280, tempHeight[2] = 330;
			if(k == 2)st = 411,tempHeight[0] = 450, tempHeight[1] = 500, tempHeight[2] = 450;
			if(k == 3)st = 40,tempHeight[0] = 790, tempHeight[1] = 700, tempHeight[2] = 680;
			i = st - 210;
		}
	}
	
	//initializing task positions on map
	gTaskPosition[NO_GAME] = {-1, -1, -1, -1};
	gIfTaskComplete[NO_GAME] = true;

	gTaskPosition[DINORUN].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[DINORUN].w = gTaskCoinA.mCoinHeight;
	gTaskPosition[DINORUN].x = gBuildingPositions[0].first + gBuildingTexture[0].getWidth()/2 - gTaskCoinA.mCoinWidth/2;
	gTaskPosition[DINORUN].y = gBuildingPositions[0].second + gBuildingTexture[0].getHeight() - gTaskCoinA.mCoinHeight;

	gTaskPosition[BUCKET_BALL].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[BUCKET_BALL].w = gTaskCoinA.mCoinHeight;
	gTaskPosition[BUCKET_BALL].x = gBuildingPositions[1].first + gBuildingTexture[1].getWidth()/2 - gTaskCoinA.mCoinWidth/2;
	gTaskPosition[BUCKET_BALL].y = gBuildingPositions[1].second + gBuildingTexture[1].getHeight() - gTaskCoinA.mCoinHeight;

	gTaskPosition[PACMAN].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[PACMAN].w = gTaskCoinA.mCoinHeight;
	gTaskPosition[PACMAN].x = gBuildingPositions[2].first + gBuildingTexture[2].getWidth()/2 - gTaskCoinA.mCoinWidth/2;
	gTaskPosition[PACMAN].y = gBuildingPositions[2].second + gBuildingTexture[2].getHeight() - gTaskCoinA.mCoinHeight;
	
	gTaskPosition[TOWERGAME].h = gTaskCoinA.mCoinWidth;
	gTaskPosition[TOWERGAME].w = gTaskCoinA.mCoinHeight;
	gTaskPosition[TOWERGAME].x = gBuildingPositions[3].first + gBuildingTexture[3].getWidth()/2 - gTaskCoinA.mCoinWidth/2;
	gTaskPosition[TOWERGAME].y = gBuildingPositions[3].second + gBuildingTexture[3].getHeight() - gTaskCoinA.mCoinHeight;


	gRequiredTaskScore[BUCKET_BALL] = 100;
	gRequiredTaskScore[PACMAN] = 5;
	gRequiredTaskScore[TOWERGAME] = 400;
	gRequiredTaskScore[DINORUN] = 5000;

	//Initializing UI button positions
	for(int i = 0;i < NUMBER_OF_BUTTONS;i++){
		BUTTONS idx = (BUTTONS)i;
		gButtonPosition[idx].x = SCREEN_WIDTH/2 - 120;
		gButtonPosition[idx].y = i * 100 + 100;
		gButtonPosition[idx].w = 150;
		gButtonPosition[idx].h = 50;
	}

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

	for (int i = 0; i < BUILDING_COUNT; i++)gBuildingTexture[i].free();
	for (int i = 0; i < TREE_COUNT; i++)gTreesTexture[i].free();

	mapObjects.clear();
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
	else{
		gMyCharacter.mCurSprite = 0;
	}

	//rendering time prompt
	SDL_Color textColor = {0, 0, 0, 255};
	std::stringstream gTimerText;
	gTimerText << "Timer : " << gTimer.getTicks() / 1000;
	gTimeTexture.loadFromText(gTimerText.str().c_str(), textColor);
	gTimeTexture.render(SCREEN_WIDTH - 100, 0);

	gMyCharacter.render(gCamera.x, gCamera.y);
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

//runs a specific task depending on the players position
void taskHandler()
{
	TASK_NAME whichTask = NO_GAME;
	for (int i = BUCKET_BALL; i < NUMBER_OF_TASKS; i++)
	{
		if (checkCollisionRect(gMyCharacter.mCharShape, gTaskPosition[i]))
			whichTask = (TASK_NAME)i;
	}

	//Run the task if it's not completed yet
	// if (!gIfTaskComplete[whichTask])
	// {
		int curTaskScore = 0;
		if (whichTask == BUCKET_BALL)
			curTaskScore = bucketBall();
		if (whichTask == PACMAN)
			curTaskScore = pacman();
		if(whichTask == DINORUN)
			curTaskScore = dinoRun();
		if(whichTask == TOWERGAME)
			curTaskScore = towerGame();
		if(whichTask != NO_GAME)gMyCharacter.positionReset();
		gTaskScore[whichTask] = curTaskScore;
		// if (gTaskScore[whichTask] >= gRequiredTaskScore[whichTask] && curTaskScore != 0)
		// {
		// 	gIfTaskComplete[whichTask] = true;
		// }
		// else
		// 	gTaskScore[whichTask] = 0;
		
	// }
}

bool ifResume = false;
//UI HANDLING
void renderUI(){

}

MENU_OPTIONS handleUI(SDL_Event &e){
	bool quit = false;
	while(!quit){
		int mouseX,mouseY;
		SDL_GetMouseState(&mouseX,&mouseY);

		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT){
				quit = true;
				return FULL_EXIT;
			}
			else if(e.type == SDL_MOUSEBUTTONDOWN){
				if(mouseX >= gButtonPosition[PLAY].x && mouseX <= gButtonPosition[PLAY].x + gButtonPosition[PLAY].w
				&& mouseY >= gButtonPosition[PLAY].y && mouseY <= gButtonPosition[PLAY].y + gButtonPosition[PLAY].h)return START_GAME;

				if(mouseX >= gButtonPosition[EXIT].x && mouseX <= gButtonPosition[EXIT].x + gButtonPosition[EXIT].w
				&& mouseY >= gButtonPosition[EXIT].y && mouseY <= gButtonPosition[EXIT].y + gButtonPosition[EXIT].h)return FULL_EXIT;

				if(mouseX >= gButtonPosition[LOGIN].x && mouseX <= gButtonPosition[LOGIN].x + gButtonPosition[LOGIN].w
				&& mouseY >= gButtonPosition[LOGIN].y && mouseY <= gButtonPosition[LOGIN].y + gButtonPosition[LOGIN].h)return LOGIN_MENU;
			}
		}
		//render UI
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);
		gUI_Background.render(0,0);
		int st = 0,skip = -1;
		if(ifResume){
			st = 1;
		}else{
			skip = 1;
		}
		for(int i = st;i < NUMBER_OF_BUTTONS;i++){
			if(skip != -1 && i == skip)continue;
			int pos = i;
			if(i == 1)pos = i - st;
			gUI_Buttons[i].render(gButtonPosition[pos].x,gButtonPosition[pos].y);
		}
		SDL_RenderPresent(gRender);
		
	}
	return FULL_EXIT;
}

void loginUI(){

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

	//====================================
	// int pp = towerGame();


	//Loading map object position and shape
	gameInitialize();

	
	SDL_Event e;

	//initial character position to track movement
	gCharCurPosX = gMyCharacter.mPosX;
	gCharCurPosY = gMyCharacter.mPosY;

	bool game_quit = false;
	while(!game_quit){
		bool quit = true;
		MENU_OPTIONS menuState = handleUI(e);
		if(menuState == FULL_EXIT)game_quit = true;
		if(menuState == START_GAME){
			quit = false;
			ifResume = true;
		}
		if(menuState == LOGIN_MENU){

		}

		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE){
					quit = true;
				}	
				gMyCharacter.handleEvent(e);
			}
			gMyCharacter.move(mapObjects);
			
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
