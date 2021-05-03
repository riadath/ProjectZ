#include "texture_timer.h"

int TOTAL_POINTS = 0;
int TOTAL_LIVES = 5;
int spawnSpeed = 0,spawnInterval = 173;
int ballDropSpeed = 2;
Timer gTimerBB;
Mix_Chunk* gSplash = NULL;

void initVariable()
{
	TOTAL_LIVES = 5;
	TOTAL_POINTS = 0;
	spawnInterval = 173;
	spawnSpeed = 0;
	ballDropSpeed = 2;
	gTimerBB.start();
}

enum COLLISION_TYPE{
	NO_COLLISION,
	WALL_COLLISION,
	BUCKET_COLLSIION
};

struct Bucket{
	int mPosX,mVelX;
	int velocity;

	Texture mBucketTexture;
	SDL_Rect mBucketShape;
	Bucket(){
		mPosX = 0;
		mVelX = 0;
		velocity = 13;
		mBucketShape = {mPosX,SCREEN_HEIGHT - mBucketTexture.getHeight(),mBucketTexture.getWidth(),mBucketTexture.getHeight()};
	}
	void handleEvent(SDL_Event &e){
		if(e.type == SDL_KEYDOWN && e.key.repeat == 0){
			switch (e.ksym){
				case SDLK_RIGHT: mVelX += velocity;break;
				case SDLK_LEFT:	mVelX -= velocity;break;
				default:break;
			}
		}
		else if(e.type == SDL_KEYUP & e.key.repeat == 0){
			switch(e.ksym){
				case SDLK_RIGHT: mVelX -= velocity;break;
				case SDLK_LEFT:	mVelX += velocity;break;
				default:break; 
			}
		}
	}
	void move(){
		mPosX += mVelX;
		if(mPosX < 0 || (mPosX + mBucketTexture.getWidth() > SCREEN_WIDTH)){
			mPosX -= mVelX;
		}
		mBucketShape = {mPosX,SCREEN_HEIGHT - mBucketTexture.getHeight(),mBucketTexture.getWidth(),mBucketTexture.getHeight()};
	}
	void render(){
		mBucketTexture.render(mPosX,SCREEN_HEIGHT - mBucketTexture.getHeight());
	}
	void free(){
		mBucketTexture.free();
	}
};

struct Ball{
	std::vector<int>mBallSpawnPosition;
	const int mSPRITE_COUNT = 8;
	SDL_Rect mBallShape,mBallSprite[8];
	const int mBallWidth = 21, mBallHeight = 21;
	Texture mBallTexture;
	std::vector<SDL_Rect>mBallRects;
	std::vector<int>mSpriteNumber;
	Ball(){
		for(int i = 50;i <= SCREEN_WIDTH;i += 80){
			mBallSpawnPosition.push_back(i);
		}
		for(int i = 0;i < mSPRITE_COUNT;i++){
			mBallSprite[i].w = mBallWidth;
			mBallSprite[i].h = mBallHeight;
			mBallSprite[i].x = i * mBallWidth;
			mBallSprite[i].y = 0;
		}
	}
	COLLISION_TYPE checkCollision(SDL_Rect bucketShape,SDL_Rect ballShape){
		if(ballShape.y + ballShape.h < SCREEN_HEIGHT - bucketShape.h)return NO_COLLISION;
		bool ifBucketCol = true;
		if(ballShape.x + ballShape.w < bucketShape.x)ifBucketCol = false;
		if(ballShape.x > bucketShape.x + bucketShape.w)ifBucketCol = false;
		if(ballShape.y + ballShape.h < bucketShape.y)ifBucketCol = false;
		if(ballShape.y > bucketShape.y + bucketShape.h)ifBucketCol = false;

		if(ifBucketCol){
			Mix_PlayChannel(-1,gSplash,0);
			return BUCKET_COLLSIION;
		}

		else{
			if(ballShape.y + ballShape.h > SCREEN_HEIGHT)return WALL_COLLISION;
			else return NO_COLLISION;
		}	
		
	}
	void onScreen(SDL_Rect bucketShape){
		if(spawnSpeed++%spawnInterval == 0){
			int tX = mBallSpawnPosition[rand()%mBallSpawnPosition.size()];
			int tY = 0;
			SDL_Rect tRect = {tX,tY,mBallWidth,mBallHeight};
			mBallRects.push_back(tRect);
			mSpriteNumber.push_back(rand()%mSPRITE_COUNT);
		}
		std::vector<SDL_Rect>tempRect;
		std::vector<int>tempSpriteNumber;
		for(int i = 0;i < mBallRects.size();i++){
			COLLISION_TYPE whatCol = checkCollision(bucketShape,mBallRects[i]);
			if(whatCol == WALL_COLLISION || whatCol == BUCKET_COLLSIION){
				mBallRects[i].w = -1,mBallRects[i].h = -1;
				if(whatCol == BUCKET_COLLSIION){
					TOTAL_POINTS += 100;
				}
				else TOTAL_LIVES -= 1;
			}else tempRect.push_back(mBallRects[i]),tempSpriteNumber.push_back(mSpriteNumber[i]);
		}
		mBallRects = tempRect;
		mSpriteNumber =tempSpriteNumber;
	}
	void render(SDL_Rect bucketShape){
		onScreen(bucketShape);
		for(int i = 0;i < mBallRects.size();i++){
			mBallTexture.render(mBallRects[i].x,mBallRects[i].y,&mBallSprite[mSpriteNumber[i]]);
			mBallRects[i].y += ballDropSpeed;
		}
	}
	
	void free(){
		mBallRects.clear();
		mSpriteNumber.clear();
	}
};

std::stringstream scoreText,liveText,timeText;
//background texture
Texture gBucketBallTexture;

//score and health texture
Texture gLiveTexture,gScoreTexture;
Bucket gBallCatcher;
Ball gBallDrop;


void closeBB(){
	SDL_RenderClear(gRender);
    gBucketBallTexture.free();
	gBallDrop.free();
    gLiveTexture.free();
    gScoreTexture.free();
	gTimerBB.stop();
}

bool loadBucketBallMedia(){
	if(!gBucketBallTexture.loadFile("images/png/room1f.png"))return false;
	if(!gBallCatcher.mBucketTexture.loadFile("images/png/bucketShort.png"))return false;
	if(!gBallDrop.mBallTexture.loadFile("images/png/ball.png"))return false;
	gFont = TTF_OpenFont("images/fonts/Oswald-BoldItalic.ttf",24);
	if(gFont == NULL){
		ERROR_T;
		return false;
	}
	gSplash = Mix_LoadWAV("sounds/splish.wav");
	if(gSplash == NULL){
		ERROR_M;
		return false;
	}
	return true;
}

void renderRoomBB(){
    SDL_Color textColor = {0,0,0,255};
    scoreText.str("");
    scoreText<<"Score : "<<TOTAL_POINTS;
    if(!gScoreTexture.loadFromText(scoreText.str().c_str(),textColor)){
        printf("Unable to render score texture\n");
    }
    liveText.str("");
    liveText<<"Lives : "<<TOTAL_LIVES;
    if(!gLiveTexture.loadFromText(liveText.str().c_str(),textColor)){
        printf("Unable to render health texture\n");
    }

	SDL_SetRenderDrawColor(gRender,255,255,255,255);
    SDL_RenderClear(gRender);
    gBucketBallTexture.render(0,0);
	gScoreTexture.render(1050,0);
	gLiveTexture.render(0,0);
	gBallDrop.render(gBallCatcher.mBucketShape);
	gBallCatcher.render();
}

int bucketBall(){
	initVariable();
	if(!loadBucketBallMedia()){
		printf("Failed to load Media\n");
		return -1;
	}
	bool quit = false;
	SDL_Event e;
	
	Uint32 prevTime = gTimerBB.getTicks()/1000;
	Uint32 prevTime1 = gTimerBB.getTicks()/1000;
	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT)quit = true;
			gBallCatcher.handleEvent(e);
		}

		if(TOTAL_LIVES <= 0)quit = true;

		Uint32 BBTime = gTimerBB.getTicks()/1000;
		if(BBTime%3 == 0 && BBTime != prevTime)spawnInterval = std::max(53,spawnInterval - 10),prevTime = BBTime;
		if(BBTime%15 == 0 && BBTime != prevTime1)ballDropSpeed += 1,prevTime1 = BBTime;
		gBallCatcher.move();
		renderRoomBB();
		SDL_RenderPresent(gRender);
	}
	gTimerBB.stop();
    closeBB();
	return TOTAL_POINTS;
}