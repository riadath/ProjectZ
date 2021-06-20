#include "texture_timer.h"

//A vector that stores all the wall rectangles in the game

bool loadPacmanMedia();

void closePacman();

//Loads the layout of the game
void load_map();

void load_ghosts();

void initPacman();

void render_ghosts();

void render_food();

//calculates the score of the game
void load_Points();

//rectangualr collision checking function
bool checkCollision(SDL_Rect a, SDL_Rect b);

//checks if a randomly spawned food is valid or not
bool Is_Food_Valid(SDL_Rect a);

void If_Pacman_Collided_With_Food(SDL_Rect a);

bool If_Pacman_Collided_With_Ghosts(SDL_Rect a);

bool If_Pacman_Collided_With_Wall(SDL_Rect a);


std::vector<SDL_Rect> WALL;
//Scoring variables
int gPoint = 0;
int gPacmanLives = 7;
int gGhostMovementSpeed = 0;
int gCurrentTime;

const int PACMAN_ANIMATION_CLIPS = 3;
const int PACMAN_GHOSTS = 4;
const int PACMAN_FOOD = 5;

Texture gPacmanTexture;
Texture gPacmanFoodTexture;
Texture gPacmanGhostTexture;
Texture gPacmanScoreTexture;
Texture gPacmanRemainingLivesTexture;
Texture gPacmanHealthTexture;

SDL_Rect gPacmanAnimationClips[PACMAN_ANIMATION_CLIPS];
SDL_Rect gPacmanFood[PACMAN_GHOSTS];
SDL_Rect gPacmanGhosts[PACMAN_FOOD];



struct PacmanGhost
{
	static const int GHOST_WIDTH = 15;
	static const int GHOST_HEIGHT = 16;
	SDL_Rect *currentClip;

	int upperPos;
	int lowerPos;
	int mPosX, mPosY;
	int type;
	bool movementType;

	SDL_Rect mCollider;

	PacmanGhost()
	{
		movementType = 0;
		mCollider.w = GHOST_WIDTH;
		mCollider.h = GHOST_HEIGHT;
	}

	void render()
	{
		currentClip = &gPacmanGhosts[type];
		gPacmanGhostTexture.render(mPosX, mPosY, currentClip);
	}
};

struct PacmanFood
{
	static const int FOOD_WIDTH = 16;
	static const int FOOD_HEIGHT = 14;
	SDL_Rect *currentClip;

	int mPosX, mPosY;
	int type;
	int time;

	SDL_Rect mCollider;

	PacmanFood()
	{

		mPosX = rand() % 1100;
		mPosY = rand() % 650;

		mCollider.w = FOOD_WIDTH;
		mCollider.h = FOOD_HEIGHT;

		type = rand() % PACMAN_FOOD;
		time = SDL_GetTicks();
	}

	void render()
	{
		currentClip = &gPacmanFood[type];
		gPacmanFoodTexture.render(mPosX, mPosY, currentClip);
	}
};

struct Pacman
{
	SDL_RendererFlip mFlipType = SDL_FLIP_NONE;
	SDL_Rect *currentClip;
	static const int PACMAN_WIDTH = 32;
	static const int PACMAN_HEIGHT = 34;

	static const int DOT_VEL = 5;

	int mPosX, mPosY;

	int mVelX, mVelY;

	int frame = 0;

	int pacmanAnimationSpeed = 0;

	SDL_Rect mCollider;

	Pacman()
	{
		mPosX = 0;
		mPosY = 300;

		mCollider.w = PACMAN_WIDTH;
		mCollider.h = PACMAN_HEIGHT;

		mVelX = 0;
		mVelY = 0;
	}

	void handleEvent(SDL_Event &e)
	{
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				mVelY -= DOT_VEL;
				break;
			case SDLK_DOWN:
				mVelY += DOT_VEL;
				break;
			case SDLK_LEFT:
				mVelX -= DOT_VEL;
				mFlipType = SDL_FLIP_HORIZONTAL;
				break;
			case SDLK_RIGHT:
				mVelX += DOT_VEL;
				mFlipType = SDL_FLIP_NONE;
				break;
			}
		}
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				mVelY += DOT_VEL;
				break;
			case SDLK_DOWN:
				mVelY -= DOT_VEL;
				break;
			case SDLK_LEFT:
				mVelX += DOT_VEL;
				break;
			case SDLK_RIGHT:
				mVelX -= DOT_VEL;
				break;
			}
		}
	}
	void move()
	{
		mPosX += mVelX;
		mCollider.x = mPosX;

		if ((mPosX < 0) || (mPosX + PACMAN_WIDTH > SCREEN_WIDTH) || If_Pacman_Collided_With_Wall(mCollider))
		{
			mPosX -= mVelX;
			mCollider.x = mPosX;
		}

		mPosY += mVelY;
		mCollider.y = mPosY;

		if ((mPosY < 0) || (mPosY + PACMAN_HEIGHT > SCREEN_HEIGHT) || If_Pacman_Collided_With_Wall(mCollider))
		{
			mPosY -= mVelY;
			mCollider.y = mPosY;
		}
	}
	void render()
	{
		pacmanAnimationSpeed++;
		if (pacmanAnimationSpeed > 100)
			pacmanAnimationSpeed = 0;
		if (pacmanAnimationSpeed % 10 == 0)
			frame++;
		if (frame > 2)
			frame = 0;
		currentClip = &gPacmanAnimationClips[frame];
		gPacmanTexture.render(mPosX, mPosY, currentClip, mFlipType);
	}
};



std::stringstream PacmanScoreText;
std::stringstream PacmanLivesText;

PacmanGhost ghost_array[PACMAN_GHOSTS];

//A deque structure to keep all the currently spawned foods
std::deque<PacmanFood> Food_queue;

void load_map()
{
	int x = SCREEN_WIDTH / 15;
	int y = SCREEN_HEIGHT / 10;
	SDL_Rect fillRect;
	fillRect = {x, 0, x, 3 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {x, 7 * y, x, 3 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {13 * x, 0, x, 3 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {13 * x, 7 * y, x, 3 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {3 * x, 0, x, 2 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {4 * x, y, x, y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {3 * x, 8 * y, x, 2 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {4 * x, 8 * y, x, y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {10 * x, 0, x, 2 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {9 * x, 0, x, y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {10 * x, 8 * y, x, 2 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {9 * x, 8 * y, x, y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {3 * x, 4 * y, x, 2 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {4 * x, 3 * y, x, 4 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {10 * x, 3 * y, x, 4 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {11 * x, 4 * y, x, 2 * y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {7 * x, 2 * y, x, y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {6 * x, 3 * y, 3 * x, y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {6 * x, 6 * y, 3 * x, y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);

	fillRect = {7 * x, 7 * y, x, y};
	SDL_SetRenderDrawColor(gRender, 0, 0, 205, 0xFF);
	SDL_RenderFillRect(gRender, &fillRect);
	WALL.push_back(fillRect);
}

bool loadPacmanMedia()
{
	gPacmanTexture.loadFile("images/pacman/pacman_animation.png");
	gPacmanFoodTexture.loadFile("images/pacman/pacman_food.jpg");
	gPacmanGhostTexture.loadFile("images/pacman/pacman_ghosts.jpg");
	gPacmanHealthTexture.loadFile("images/health.png");

	for (int i = 0; i < 3; i++)
	{
		gPacmanAnimationClips[i].x = 32 * i;
		gPacmanAnimationClips[i].y = 0;
		gPacmanAnimationClips[i].w = 32;
		gPacmanAnimationClips[i].h = 34;
	}
	for (int i = 0; i < 5; i++)
	{
		gPacmanFood[i].x = 16 * i;
		gPacmanFood[i].y = 0;
		gPacmanFood[i].w = 16;
		gPacmanFood[i].h = 14;
	}
	for (int i = 0; i < 4; i++)
	{
		gPacmanGhosts[i].x = 0;
		gPacmanGhosts[i].y = 16 * i;
		gPacmanGhosts[i].w = 15;
		gPacmanGhosts[i].h = 16;
	}

	gFont = TTF_OpenFont("images/fonts/Oswald-BoldItalic.ttf", 24);
	if (gFont == NULL)
	{
		ERROR_T;
		return false;
	}
	return true;
}

void closePacman()
{
	gPacmanTexture.free();
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	return true;
}

void load_ghosts()
{
	int x = SCREEN_WIDTH / 15;
	int y = SCREEN_HEIGHT / 10;

	for (int i = 0; i < 4; i++)
		ghost_array[i].type = i;

	ghost_array[0].lowerPos = ghost_array[0].mPosX = 2 * x;
	ghost_array[0].upperPos = 6 * x;
	ghost_array[0].mPosY = 7 * y;

	ghost_array[1].lowerPos = ghost_array[1].mPosX = 5 * x;
	ghost_array[1].upperPos = 9 * x;
	ghost_array[1].mPosY = 4 * y;

	ghost_array[2].lowerPos = ghost_array[2].mPosY = 0;
	ghost_array[2].upperPos = 5 * y;
	ghost_array[2].mPosX = 12 * x;

	ghost_array[3].lowerPos = ghost_array[3].mPosY = y;
	ghost_array[3].upperPos = 5 * y;
	ghost_array[3].mPosX = 2 * x;
}

void render_ghosts()
{
	gGhostMovementSpeed++;
	if (gGhostMovementSpeed > 100)
		gGhostMovementSpeed = 0;

	for (int i = 0; i < 4; i++)
	{
		bool mType = ghost_array[i].movementType;
		if (i == 0 || i == 1)
		{
			if (mType == 0)
			{
				ghost_array[i].mPosX++;
				if (ghost_array[i].mPosX > ghost_array[i].upperPos)
					ghost_array[i].movementType = 1;
			}
			else
			{
				ghost_array[i].mPosX--;
				if (ghost_array[i].mPosX < ghost_array[i].lowerPos)
					ghost_array[i].movementType = 0;
			}
		}
		else
		{
			if (mType == 0)
			{
				ghost_array[i].mPosY++;
				if (ghost_array[i].mPosY > ghost_array[i].upperPos)
					ghost_array[i].movementType = 1;
			}
			else
			{
				ghost_array[i].mPosY--;
				if (ghost_array[i].mPosY < ghost_array[i].lowerPos)
					ghost_array[i].movementType = 0;
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		ghost_array[i].render();
	}
}

void load_Points()
{
	SDL_Color textColor = {255, 255, 255, 0xFF};
	PacmanScoreText.str("");
	PacmanScoreText << "Score : " << gPoint;
	if (!gPacmanScoreTexture.loadFromText(PacmanScoreText.str().c_str(), textColor))
	{
		printf("Unable to render score texture\n");
	}
	PacmanLivesText.str("");
	PacmanLivesText << "Lives : " << gPacmanLives;
	if (!gPacmanRemainingLivesTexture.loadFromText(PacmanLivesText.str().c_str(), textColor))
	{
		printf("Unable to render score texture\n");
	}
	for(int i = 0;i < gPacmanLives;i++){
		gPacmanHealthTexture.render(500 + i * gPacmanHealthTexture.getWidth(),0);
	}
	
	gPacmanScoreTexture.render(500, 20);
}

void render_food()
{
	int present_time = SDL_GetTicks() -gCurrentTime;
	int time_stamp = 15000;
	int food_no = present_time / time_stamp + 1;
	int food_remainingspawned_time = 5000;
	SDL_Rect Food_Rect;
	while (Food_queue.size() < food_no)
	{
		while (1)
		{
			PacmanFood temp;
			Food_Rect = {temp.mPosX, temp.mPosY, temp.FOOD_WIDTH, temp.FOOD_HEIGHT};
			if (Is_Food_Valid(Food_Rect) == 1)
			{
				Food_queue.push_back(temp);
				break;
			}
		}
	}
	for (int i = 0; i < Food_queue.size(); i++)
	{
		if (present_time - Food_queue[i].time + gCurrentTime > food_remainingspawned_time)
		{
			Food_queue.erase(Food_queue.begin() + i);
			gPacmanLives--;
		}
	}
	for (int i = 0; i < Food_queue.size(); i++)
	{
		Food_queue[i].render();
	}
}

bool If_Pacman_Collided_With_Wall(SDL_Rect a)
{
	for (int i = 0; i < WALL.size(); i++)
	{
		if (checkCollision(a, WALL[i]) == true)
			return true;
	}
	return false;
}

bool Is_Food_Valid(SDL_Rect a)
{
	for (int i = 0; i < WALL.size(); i++)
	{
		if (checkCollision(WALL[i], a) == 1)
			return 0;
	}
	return 1;
}

void If_Pacman_Collided_With_Food(SDL_Rect a)
{
	SDL_Rect temp;
	for (int i = 0; i < Food_queue.size(); i++)
	{
		temp = {Food_queue[i].mPosX, Food_queue[i].mPosY, Food_queue[i].FOOD_WIDTH, Food_queue[i].FOOD_HEIGHT};
		if (checkCollision(a, temp) == 1)
		{
			gPoint += Food_queue[i].type;
			Food_queue.erase(Food_queue.begin() + i);
			break;
		}
	}
}

bool If_Pacman_Collided_With_Ghosts(SDL_Rect a)
{
	SDL_Rect temp;
	for (int i = 0; i < 4; i++)
	{
		temp = {ghost_array[i].mPosX, ghost_array[i].mPosY, ghost_array[i].GHOST_WIDTH, ghost_array[i].GHOST_HEIGHT};
		if (checkCollision(a, temp) == 1)
			return 1;
	}
	return 0;
}

void initPacman()
{
	gPacmanLives = 7;
	gGhostMovementSpeed = 0;
	WALL.clear();
	Food_queue.clear();
	gCurrentTime = SDL_GetTicks();
}

int pacman()
{
	initPacman();
	loadPacmanMedia();
	load_ghosts();
	bool quit = false;

	SDL_Event e;

	Pacman pacman;

	SDL_Rect temp;

	while (!quit)
	{
		//if lives lost more than gPacmanLives then GAME OVER!
		if (gPacmanLives <= 0)
			quit = true;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			pacman.handleEvent(e);
		}

		pacman.move();

		SDL_SetRenderDrawColor(gRender, 0, 0, 0, 0);
		SDL_RenderClear(gRender);

		temp = {pacman.mPosX, pacman.mPosY, pacman.PACMAN_WIDTH, pacman.PACMAN_HEIGHT};

		load_map();
		render_ghosts();
		load_Points();
		render_food();
		If_Pacman_Collided_With_Food(temp);

		//If Pacman collided with any ghost it respawns at these coordinates
		if (If_Pacman_Collided_With_Ghosts(temp) == 1)
		{
			pacman.mPosX = 7 * (SCREEN_WIDTH / 15);
			pacman.mPosY = 5 * (SCREEN_HEIGHT / 10);
			gPacmanLives--;
		}
		pacman.render();
		SDL_RenderPresent(gRender);
	}

	closePacman();

	return gPoint;
}