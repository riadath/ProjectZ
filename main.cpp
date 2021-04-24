
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <math.h>
#include <cstdio>
#include <vector>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// #include <SDL2/SDL_ttf.h>

#define ERROR printf("------> %s\n", SDL_GetError());
#define ERROR_I printf("-----> %s\n", IMG_GetError());
#define ERROR_T printf("-----> %s\n", TTF_GetError());

//hello2

#define ksym key.keysym.sym

const int WIDTH = 1200;
const int HEIGHT = 697;

bool init();
bool loadMedia();
bool checkCollision(SDL_Rect player,std::vector<SDL_Rect>objects);
void closeAll();

//Global window and renderer
SDL_Window* gWindow = NULL;
SDL_Renderer* gRender = NULL;

class Texture{
	private:
		SDL_Texture* mTexture;
		int mWidth,mHeight;
	public:
		Texture(){
			mTexture = NULL;
			mWidth = 0,mHeight = 0;
		}
		~Texture(){
			free();
		}
		int getWidth(){
			return mWidth;
		}
		int getHeight(){
			return mHeight;
		}
		void free(){
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0,mHeight = 0;
		}
		void setColor(Uint8 r,Uint8 g,Uint8 b){
			SDL_SetTextureColorMod(mTexture,r,g,b);
		}
		void render(int x,int y,SDL_Rect* clip = NULL,SDL_RendererFlip flip = SDL_FLIP_NONE){
			SDL_Rect renderSize = {x,y,mWidth,mHeight};
			if(clip != NULL)renderSize.h = clip->h,renderSize.w = clip->w;
			
			SDL_RenderCopyEx(gRender,mTexture,clip,&renderSize,0,NULL,flip);
		}
		
		bool loadFile(std::string path,bool ifColKey = 0,Uint8 r = 255,Uint8 g = 255,Uint8 b = 255){
			free();
			SDL_Surface* tSurface = IMG_Load(path.c_str());
			SDL_Texture* tTexture = NULL;
			if(tSurface == NULL){
				ERROR_I;
				return false;
			}
			
			SDL_SetColorKey(tSurface,ifColKey,SDL_MapRGB(tSurface->format,r,g,b));
			tTexture = SDL_CreateTextureFromSurface(gRender,tSurface);
			if(tTexture == NULL){
				ERROR;
				return false;
			}
			mWidth = tSurface->w;
			mHeight = tSurface->h;
			SDL_FreeSurface(tSurface);
			mTexture = tTexture;
			return (mTexture != NULL);
		}
		void setBlendMode(SDL_BlendMode blending){
			SDL_SetTextureBlendMode(mTexture,blending);
		}
		void setAlpha(Uint8 alpha){
			SDL_SetTextureAlphaMod(mTexture,alpha);
		}
		
		#if defined(SDL_TTF_MAJOR_VERSION)
		bool loadFromText(std::string path,SDL_Color textColor){
			free();
			SDL_Surface* textSurface = TTF_RenderText_Solid(gFont,path.c_str(),textColor);
			if(textSurface == NULL){
				ERROR_T;
				return false;
			}
			mTexture = SDL_CreateTextureFromSurface(gRender,textSurface);
			if(mTexture == NULL){
				ERROR;
				return false;
			}
			mWidth = textSurface->w;
			mHeight = textSurface->h;
			SDL_FreeSurface(textSurface);
			return (mTexture != NULL);
		}
		#endif
};

class Character{
	private:
		int mCharPosX,mCharPosY;
		int mCharVelX,mCharVelY;
		const int CHAR_SPRITE_COUNT = 10;
		SDL_Rect mCharShape,mCharacterSprite[10];
	public:
		const int charWidth = 32,charHeight = 65; 
		const int CHAR_VEL = 3;
		int mCurSprite,mSpeedDec;
		SDL_RendererFlip mFlipType;
		Texture mCharTexture;
		Character(){
			mCharPosX = WIDTH/2,mCharPosY = HEIGHT/2;
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
			if(mCharPosX < 0 || (mCharPosX + charWidth > WIDTH) || checkCollision(mCharShape,objects)){
				mCharPosX -= mCharVelX;
				mCharShape.x = mCharPosX;
			}
			mCharPosY += mCharVelY;
			mCharShape.y = mCharPosY;
			if(mCharPosY < 0 || (mCharPosY + charHeight > HEIGHT) || checkCollision(mCharShape,objects)){
				mCharPosY -= mCharVelY;
				mCharShape.y = mCharPosY;
			}
		}
		void render(){
			mCharTexture.render(mCharPosX,mCharPosY,&mCharacterSprite[mCurSprite/mSpeedDec],mFlipType);
		}
};

Character gMyCharacter;
Texture gBackgroundTexture,gBuilding[2],gTrees[3],gBush;
std::vector<SDL_Rect>room1Objects,room2Objects,gBushSprite;

bool init(){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		ERROR;
		return false;
	}
	gWindow = SDL_CreateWindow("kms",SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
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
	// if(TTF_Init() == -1){
	// 	ERROR_T;
	// 	return false;
	// }
	return true;
}

bool loadMedia(){
	if(!gMyCharacter.mCharTexture.loadFile("images/png/walk1.png",true,255,255,255))return false;
	if(!gBackgroundTexture.loadFile("images/png/room1f.png"))return false;
	if(!gBuilding[0].loadFile("images/png/building1.png"))return false;
	if(!gBuilding[1].loadFile("images/png/house2.png",true,0,64,128))return false;
	if(!gTrees[0].loadFile("images/png/tree1.png"))return false;
	if(!gTrees[1].loadFile("images/png/tree2.png"))return false;
	if(!gTrees[2].loadFile("images/png/tree3.png"))return false;
	if(!gBush.loadFile("images/png/bushAll.png",true,255,255,255))return false;

	SDL_Rect tRect;	

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

	tRect.x = (3 * WIDTH) /4,tRect.y = (HEIGHT/5);
	tRect.w = 45,tRect.h = 46;
	room1Objects.push_back(tRect);

	tRect.x = WIDTH/18,tRect.y = (4 * HEIGHT)/5 - gMyCharacter.charHeight;
	tRect.w = gBuilding[1].getWidth() - 20,tRect.h = gBuilding[1].getHeight() - 10;
	room1Objects.push_back(tRect);

	tRect.x = room1Objects[1].x - 20,tRect.y = room1Objects[1].y - 30;
	tRect.w = gTrees[0].getWidth(),tRect.h = gTrees[0].getHeight();
	room1Objects.push_back(tRect);

	tRect.x = WIDTH/3,tRect.y = HEIGHT/5;
	tRect.w = gTrees[0].getWidth(),tRect.h = gTrees[0].getHeight();
	room1Objects.push_back(tRect);

	tRect.x = 4*WIDTH/5,tRect.y = 5 * HEIGHT/6;
	tRect.w = gTrees[0].getWidth(),tRect.h = gTrees[0].getHeight();
	room1Objects.push_back(tRect);	

	tRect.x = 500,tRect.y = 500;
	tRect.w = gTrees[0].getWidth(),tRect.h = gTrees[0].getHeight();
	room1Objects.push_back(tRect);
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
	gWindow = NULL;
	gRender = NULL;
	IMG_Quit();
	// TTF_Quit();
	SDL_Quit();
}

void renderRoom1(){  
    SDL_SetRenderDrawColor(gRender,255,255,255,255);
    SDL_RenderClear(gRender);
    
    gBackgroundTexture.render(0,0);

    gBuilding[0].render(room1Objects[0].x,room1Objects[0].y);
    gBuilding[1].render(room1Objects[1].x,room1Objects[1].y);

    gTrees[0].render(room1Objects[2].x,room1Objects[2].y);
    gTrees[1].render(room1Objects[3].x,room1Objects[3].y);
    gTrees[2].render(room1Objects[4].x,room1Objects[4].y);
	gTrees[0].render(500,500);
	gBush.render(200,200,&gBushSprite[3]);

    // SDL_RenderDrawPoint(gRender,WIDTH/12 - 100,HEIGHT - 100);
    gMyCharacter.render();
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
	while(!quit){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE))quit = true;
			gMyCharacter.handleEvent(e);
		}
        gMyCharacter.move(room1Objects);
		renderRoom1();
		SDL_RenderPresent(gRender);
	}

	closeAll();
	return 0;
}
