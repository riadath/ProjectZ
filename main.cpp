#include "bucket_ball.h"

bool loadMedia();
bool checkCollision(SDL_Rect player,std::vector<SDL_Rect>objects);
void closeAll();
void renderRomm1();

struct Character{
		int mCharPosX,mCharPosY;
		int mCharVelX,mCharVelY;
		int mCurSprite,mSpeedDec;

		const int CHAR_SPRITE_COUNT = 10;
		const int charWidth = 32,charHeight = 65; 
		const int CHAR_VEL = 3;

		SDL_Rect mCharShape,mCharacterSprite[10];
		SDL_RendererFlip mFlipType;
		Texture mCharTexture;
		Character(){
			mCharPosX = SCREEN_WIDTH/2,mCharPosY = SCREEN_HEIGHT/2;
			mCharVelX = mCharVelY = 0;
			mCharShape.x = mCharPosX,mCharShape.y = mCharPosY;
			mCharShape.w = charWidth,mCharShape.h = charHeight;
			mCurSprite = 1;
			mSpeedDec = 3;
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
			if(mCurSprite/mSpeedDec >= CHAR_SPRITE_COUNT)mCurSprite = 0;
		}
		void handleEvent(SDL_Event& e){
			if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)spriteChanger();
			if(e.type == SDL_KEYDOWN & e.key.repeat == 0){	
				switch (e.ksym){
					case SDLK_w: mCharVelY -= CHAR_VEL;break;
					case SDLK_s: mCharVelY += CHAR_VEL;break;
					case SDLK_a: mCharVelX -= CHAR_VEL;
						mFlipType = SDL_FLIP_HORIZONTAL;
						break;
					case SDLK_d: mCharVelX += CHAR_VEL;
						mFlipType = SDL_FLIP_NONE;break;
					default:break;
				}
			}
			else if(e.type == SDL_KEYUP && e.key.repeat == 0){
				switch (e.ksym){
					case SDLK_w: mCharVelY += CHAR_VEL;break;
					case SDLK_s: mCharVelY -= CHAR_VEL;break;
					case SDLK_a: mCharVelX += CHAR_VEL;break;
					case SDLK_d: mCharVelX -= CHAR_VEL;break;
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
			mCharTexture.render(mCharPosX,mCharPosY,&mCharacterSprite[mCurSprite/mSpeedDec],mFlipType);
		}
};

Character gMyCharacter;
const int gBuildingCount = 3;
const int gTreeCount = 3;
Texture gBackgroundTexture,gBuilding[gBuildingCount],gTrees[gTreeCount],gBush;
std::vector<SDL_Rect>room1Objects,gBushSprite;

bool loadMedia(){
	if(!gMyCharacter.mCharTexture.loadFile("images/png/walk1.png",true,255,255,255))return false;
	if(!gBackgroundTexture.loadFile("images/png/background1.png"))return false;
	if(!gBuilding[0].loadFile("images/png/greenBuilding.png",true,255,255,255))return false;
	if(!gBuilding[1].loadFile("images/png/whiteBuilding.png",true,255,255,255))return false;
	if(!gBuilding[2].loadFile("images/png/house2.png",true,0,64,128))return false;
	if(!gTrees[0].loadFile("images/png/tree1.png"))return false;
	if(!gTrees[1].loadFile("images/png/tree2.png"))return false;
	if(!gTrees[2].loadFile("images/png/tree3.png"))return false;
	if(!gBush.loadFile("images/png/bushAll.png",true,255,255,255))return false;

	SDL_Rect tRect;	

	tRect.x = 900,tRect.y = 450;
	tRect.w = gBuilding[0].getWidth() - 20,tRect.h = gBuilding[0].getHeight() - 10;
	room1Objects.push_back(tRect);

	tRect.x = 120,tRect.y = 470;
	tRect.w = gBuilding[1].getWidth() - 20,tRect.h = gBuilding[1].getHeight() - 10;
	room1Objects.push_back(tRect);

	tRect.x = 100,tRect.y = 140;
	tRect.w = gBuilding[2].getWidth() - 20,tRect.h = gBuilding[2].getHeight() - 10;
	room1Objects.push_back(tRect);

	for(int i = 0;i < 3;i++){
		tRect.x = i * gBush.getWidth()/3;
		tRect.y = 0;
		tRect.w = gBush.getWidth()/3,tRect.h = gBush.getHeight()/3;
		gBushSprite.push_back(tRect);
	}
	for(int i = 0;i < 3;i++){
		tRect.x = i * gBush.getWidth()/3;
		tRect.y = gBush.getHeight()/3;
		tRect.w = gBush.getWidth()/3,tRect.h = gBush.getHeight()/3;
		gBushSprite.push_back(tRect);
	}
	for(int i = 0;i < 2;i++){
		tRect.x = i * gBush.getWidth()/3;
		tRect.y = 2 * gBush.getHeight()/3;
		tRect.w = gBush.getWidth()/3,tRect.h = gBush.getHeight()/3;
		gBushSprite.push_back(tRect);
	} 

	for( int i = 50; i <= 1000; i+= 190 ){
		tRect.x = i,tRect.y = 50;
		tRect.w = gTrees[i%3].getWidth(),tRect.h = gTrees[i%3].getHeight();
		room1Objects.push_back(tRect);
	}
	for( int i = 100; i <= 1100; i+= 211 )
	{
		tRect.x = i,tRect.y = 300;
		tRect.w = gTrees[i%3].getWidth(),tRect.h = gTrees[i%3].getHeight();
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
	for(int i = 0;i < gBuildingCount;i++)gBuilding[i].free();
	for(int i = 0;i < gTreeCount;i++)gTrees[i].free();
	gBush.free();
	
	
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

    gBuilding[0].render(room1Objects[0].x,room1Objects[0].y);
	gBuilding[1].render(room1Objects[1].x,room1Objects[1].y);
	gBuilding[2].render(room1Objects[2].x,room1Objects[2].y);
//	gBush.render(40,100,&gBushSprite[2]);

	

	//bush rendering
	for( int i = 100; i <=200; i+= 41 ) gBush.render(i,100,&gBushSprite[i%5]);
	for( int i = 302; i <= 400; i+= 41 ) gBush.render(i,100,&gBushSprite[i%5]);
	for( int i = 470; i <= 570; i+= 43 ) gBush.render(i,100,&gBushSprite[i%5]);
	for( int i = 700; i <= 1000; i+= 43 ) gBush.render(i,100,&gBushSprite[i%5]);
	for( int i = 150; i <=300; i+= 41 ) gBush.render(i,350,&gBushSprite[i%5]);
	for( int i = 350; i <=500; i+= 41 ) gBush.render(i,350,&gBushSprite[i%5]);
	for( int i = 800; i <=1030; i+= 41 ) gBush.render(i,350,&gBushSprite[i%5]);
	//tree rendering
	for( int i = 50; i <= 1000; i+= 190 ) gTrees[i%3].render(i,50);
	for( int i = 100; i <= 1100; i+= 211 ) gTrees[i%3].render(i,300);
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

int main(int argc, char* argv[])
{
	if(!init()){
		printf("Failed to initialize\n");
		return 0;
	}
	if(!loadMedia()){
		printf("Failed to load media\n");
		return 0;
	}

	bool quit = false;
	SDL_Event e;
	bool ifBB = false;
	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE))quit = true;
			gMyCharacter.handleEvent(e);
		}
        gMyCharacter.move(room1Objects);
		
		if(gMyCharacter.mCharPosX == 0 && gMyCharacter.mCharPosY == 0 ){
			ifBB = true;
		}
		renderRoom1Objects();
		SDL_RenderPresent(gRender);
	}
	closeAll();
	return 0;
}
