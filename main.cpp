#include "bucket_ball.h"
#include "texture_timer.h"

//function prototypes
bool loadMedia();
bool checkCollision(SDL_Rect player,std::vector<SDL_Rect>objects);
void closeAll();
void renderRomm1();

//structure for player

struct Character{
		int mCharPosX,mCharPosY;
		int mCharVelX,mCharVelY;
		int mCurSprite;

		const int CHAR_SPEED_DEC = 5;
		const int CHAR_SPRITE_COUNT = 10;
		const int charWidth = 32,charHeight = 65; 
		const int CHAR_VELOCITY = 4;

		SDL_Rect mCharShape,mCharacterSprite[10];
		SDL_RendererFlip mFlipType;
		Texture mCharTexture;
		Character(){
			mCharPosX = SCREEN_WIDTH/2,mCharPosY = SCREEN_HEIGHT/2;
			mCharVelX = mCharVelY = 0;
			mCharShape.x = mCharPosX,mCharShape.y = mCharPosY;
			mCharShape.w = charWidth,mCharShape.h = charHeight;
			mCurSprite = 1;
			mFlipType = SDL_FLIP_NONE;
			for(int i = 0;i < CHAR_SPRITE_COUNT;i++){
				mCharacterSprite[i].w = mCharShape.w;
				mCharacterSprite[i].h = mCharShape.h;
				mCharacterSprite[i].x = i * mCharShape.w;
				mCharacterSprite[i].y = 0;
			}
		}
		void positionReset(){
			mCharPosX = SCREEN_WIDTH/2,mCharPosY = SCREEN_HEIGHT/2;
			mCharVelX = mCharVelY = 0;
		}
		void spriteChanger(){
			mCurSprite++;
			if(mCurSprite/CHAR_SPEED_DEC >= CHAR_SPRITE_COUNT)mCurSprite = 1;
		}
		void handleEvent(SDL_Event& e){
			if(e.type == SDL_KEYDOWN & e.key.repeat == 0){	
				switch (e.ksym){
					case SDLK_w: mCharVelY -= CHAR_VELOCITY;break;
					case SDLK_s: mCharVelY += CHAR_VELOCITY;break;
					case SDLK_a: mCharVelX -= CHAR_VELOCITY;break;
					case SDLK_d: mCharVelX += CHAR_VELOCITY;break;
					default:break;
				}
			}
			else if(e.type == SDL_KEYUP && e.key.repeat == 0){
				switch (e.ksym){
					case SDLK_w: mCharVelY += CHAR_VELOCITY;break;
					case SDLK_s: mCharVelY -= CHAR_VELOCITY;break;
					case SDLK_a: mCharVelX += CHAR_VELOCITY;break;
					case SDLK_d: mCharVelX -= CHAR_VELOCITY;break;
					default:break;
				}
			}
		}

		void move(std::vector<SDL_Rect>objects){
			mCharPosX += mCharVelX;
			
			mCharShape.x = mCharPosX;
			if(mCharPosX < 0 || (mCharPosX + charWidth > SCREEN_WIDTH) || checkCollision(mCharShape,objects)){
				mCharPosX -= mCharVelX;
				mCharShape.x = mCharPosX;
			}
			mCharPosY += mCharVelY;
			mCharShape.y = mCharPosY;
			if(mCharPosY < 0 || (mCharPosY + charHeight > SCREEN_HEIGHT) || checkCollision(mCharShape,objects)){
				mCharPosY -= mCharVelY;
				mCharShape.y = mCharPosY;
			}
		}
		void render(){
			mCharTexture.render(mCharPosX,mCharPosY,&mCharacterSprite[mCurSprite/CHAR_SPEED_DEC],mFlipType);
		}
};

Character gMyCharacter;

const int gBuildingCount = 3;
const int gTreeCount = 3;

//Variable to check the change in characters position
int charCurPosX ,charCurPosY;

//Texture files

Texture gBackgroundTexture;
Texture gBushTexture;
Texture gBuildingTexture[gBuildingCount];
Texture gTreesTexture[gTreeCount];

std::vector<SDL_Rect>room1Objects;
std::vector<SDL_Rect>gBushTextureSprite;

std::vector<std::pair<int,int>>gBuildingPositions;
std::vector<std::tuple<int,int,int>>gTreePositions;

bool loadMedia(){
	if(!gMyCharacter.mCharTexture.loadFile("images/png/walk1.png"))return false;
	if(!gBackgroundTexture.loadFile("images/png/background1.png"))return false;
	if(!gBuildingTexture[0].loadFile("images/png/house2.png",true,0,64,128))return false;
	if(!gBuildingTexture[1].loadFile("images/png/house1_5.png"))return false;
	if(!gBuildingTexture[2].loadFile("images/png/house1_4.png"))return false;
	if(!gTreesTexture[0].loadFile("images/png/tree1.png"))return false;
	if(!gTreesTexture[1].loadFile("images/png/tree2.png"))return false;
	if(!gTreesTexture[2].loadFile("images/png/tree3.png"))return false;
	if(!gBushTexture.loadFile("images/png/bushAll.png"))return false;

	SDL_Rect tRect;	

	tRect.x = 900,tRect.y = 450;
	gBuildingPositions.push_back(std::make_pair(tRect.x,tRect.y));
	tRect.w = gBuildingTexture[0].getWidth() - 20,tRect.h = gBuildingTexture[0].getHeight() - gMyCharacter.charHeight;
	room1Objects.push_back(tRect);

	tRect.x = 120,tRect.y = 500;
	gBuildingPositions.push_back(std::make_pair(tRect.x,tRect.y));
	tRect.w = gBuildingTexture[1].getWidth() - 20,tRect.h = gBuildingTexture[1].getHeight() - gMyCharacter.charHeight;
	room1Objects.push_back(tRect);

	tRect.x = 100,tRect.y = 180;
	gBuildingPositions.push_back(std::make_pair(tRect.x,tRect.y));
	tRect.w = gBuildingTexture[2].getWidth() - 20,tRect.h = gBuildingTexture[2].getHeight() - 20;
	room1Objects.push_back(tRect);

	for(int i = 0;i < 6;i++){
		tRect.x = i * gBushTexture.getWidth()/3;
		tRect.y = (i/3) * gBushTexture.getHeight()/3;
		tRect.w = gBushTexture.getWidth()/3,tRect.h = gBushTexture.getHeight()/3;
		gBushTextureSprite.push_back(tRect);
	}
	for( int i = 50; i <= 1000; i+= 190 ){
		tRect.x = i,tRect.y = 30;
		gTreePositions.push_back(std::make_tuple(i%gTreeCount,tRect.x,tRect.y));
		tRect.w = gTreesTexture[i%gTreeCount].getWidth(),tRect.h = gTreesTexture[i%gTreeCount].getHeight();
		room1Objects.push_back(tRect);
	}
	for( int i = 311; i <= 1100; i+= 211 )
	{
		tRect.x = i,tRect.y = 350;
		gTreePositions.push_back(std::make_tuple(i%gTreeCount,tRect.x,tRect.y));
		tRect.w = gTreesTexture[i%gTreeCount].getWidth(),tRect.h = gTreesTexture[i%gTreeCount].getHeight();
		room1Objects.push_back(tRect);
	}

	return true;
}
bool checkCollision(SDL_Rect player,std::vector<SDL_Rect>objects){
	bool flag = false;
	for(int i = 0;i < (int)objects.size();i++){
		bool tFlag = true;
		if(player.x + player.w <= objects[i].x)tFlag = false;
		if(player.x >= objects[i].x + objects[i].w)tFlag = false;
		if(player.y + player.h <= objects[i].y)tFlag = false;
		if(player.y >= objects[i].y + objects[i].h)tFlag = false;
		if(tFlag){
			flag = true;
			break;
		}
	}
	return flag;
}
void closeAll(){
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRender);
	Mix_FreeMusic(gMusic);
	gMyCharacter.mCharTexture.free();
	gBackgroundTexture.free();
	for(int i = 0;i < gBuildingCount;i++)gBuildingTexture[i].free();
	for(int i = 0;i < gTreeCount;i++)gTreesTexture[i].free();
	gBushTexture.free();
	
	
	gWindow = NULL;
	gRender = NULL;
	gMusic = NULL;
	gFont = NULL;
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	Mix_Quit();
}

void renderRoom1Objects(){  
    SDL_SetRenderDrawColor(gRender,255,255,255,255);
    SDL_RenderClear(gRender);
    
    gBackgroundTexture.render(0,0);

	//building render
    for(int i = 0;i < gBuildingCount;i++)
		gBuildingTexture[i].render(gBuildingPositions[i].first,gBuildingPositions[i].second);
	for(int i = 0;i < gTreePositions.size();i++){
		gTreesTexture[std::get<0>(gTreePositions[i])].render(std::get<1>(gTreePositions[i]),std::get<2>(gTreePositions[i]));
	}

	//bush rendering
	int bushHeigt1 = 80,bushHeigt2 = 400;
	for( int i = 100; i <=200; i+= 41 ) gBushTexture.render(i,bushHeigt1,&gBushTextureSprite[i%5]);
	for( int i = 302; i <= 400; i+= 41 ) gBushTexture.render(i,bushHeigt1,&gBushTextureSprite[i%5]);
	for( int i = 470; i <= 570; i+= 43 ) gBushTexture.render(i,bushHeigt1,&gBushTextureSprite[i%5]);
	for( int i = 700; i <= 1000; i+= 43 ) gBushTexture.render(i,bushHeigt1,&gBushTextureSprite[i%5]);
	for( int i = 150; i <=300; i+= 41 ) gBushTexture.render(i,bushHeigt2,&gBushTextureSprite[i%5]);
	for( int i = 350; i <=500; i+= 41 ) gBushTexture.render(i,bushHeigt2,&gBushTextureSprite[i%5]);
	for( int i = 800; i <=1030; i+= 41 ) gBushTexture.render(i,bushHeigt2,&gBushTextureSprite[i%5]);

	//Checking if player has moved
	if(charCurPosX != gMyCharacter.mCharPosX || charCurPosY != gMyCharacter.mCharPosY){
		gMyCharacter.spriteChanger();

		if(charCurPosX > gMyCharacter.mCharPosX)gMyCharacter.mFlipType = SDL_FLIP_HORIZONTAL;
		else if(charCurPosX < gMyCharacter.mCharPosX)gMyCharacter.mFlipType = SDL_FLIP_NONE;

		//update character position
		charCurPosX = gMyCharacter.mCharPosX;
		charCurPosY = gMyCharacter.mCharPosY;

	}else gMyCharacter.mCurSprite = 0;

    gMyCharacter.render();
}

bool init(){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		ERROR;
		return false;
	}
	gWindow = SDL_CreateWindow("kms",SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
	if(gWindow == NULL){
		ERROR;
		return false;
	}
	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1")){
		printf("Linear texture filtering not enabled\n");
	}
	gRender = SDL_CreateRenderer(gWindow,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(gRender == NULL){
		ERROR;
		return false;
	}
	SDL_SetRenderDrawColor(gRender,0xFF,0xFF,0xFF,0xFF);
	if(!IMG_Init(IMG_INIT_PNG)){
		ERROR_I;
		return false;
	}
	if(TTF_Init() == -1){
		ERROR_T;
		return false;
	}
	if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048) < 0){
		ERROR_M;
		return false;
	}
	return true;
}

bool checkCollisionRect(SDL_Rect player,SDL_Rect object){
	bool tFlag = true;
	if(player.x + player.w <= object.x)tFlag = false;
	if(player.x >= object.x + object.w)tFlag = false;
	if(player.y + player.h <= object.y)tFlag = false;
	if(player.y >= object.y + object.h)tFlag = false;
	return tFlag;
}

//enumerate each task
bool ifTaskPosition(int posX,int posY){
	SDL_Rect dorRect;
	dorRect.x = gBuildingPositions[1].first + 50;
	dorRect.y = gBuildingPositions[1].second + 71;
	dorRect.w = 15;
	dorRect.h = 0;
	return (checkCollisionRect(dorRect,gMyCharacter.mCharShape));
}

int main(int argc, char* argv[])
{
	srand(time(0));
	if(!init()){
		printf("Failed to initialize\n");
		return 0;
	}
	std::string pathCursor = "images/png/cursor.png";
	SDL_Surface* myCursorSurface = IMG_Load(pathCursor.c_str());
	SDL_Cursor* myCursor =  SDL_CreateColorCursor(myCursorSurface, 0, 0);
	SDL_SetCursor(myCursor);

	if(!loadMedia()){
		printf("Failed to load media\n");
		return 0;
	}

	bool quit = false;
	SDL_Event e;
	int BB_score = 0;
	charCurPosX = gMyCharacter.mCharPosX;
	charCurPosY = gMyCharacter.mCharPosY;
	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE))quit = true;
			gMyCharacter.handleEvent(e);
		}
        gMyCharacter.move(room1Objects);
		bool flag = ifTaskPosition(gMyCharacter.mCharPosX,gMyCharacter.mCharPosY);
		if(flag){
			BB_score = bucketBall();
			gMyCharacter.positionReset();
		}
		renderRoom1Objects();
		SDL_RenderPresent(gRender);
	}
	closeAll();
	return 0;
}
