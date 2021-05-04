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
#include <sstream>
#include <time.h>
#include <tuple>
#include <utility>
// #include <bits/stdc++.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define ERROR printf("------> %s\n", SDL_GetError());
#define ERROR_I printf("-----> %s\n", IMG_GetError());
#define ERROR_T printf("-----> %s\n", TTF_GetError());
#define ERROR_M printf("-----> %s\n",Mix_GetError());
#define ksym key.keysym.sym

SDL_Window* gWindow = NULL;
SDL_Renderer* gRender = NULL;
TTF_Font* gFont = NULL;
Mix_Music* gMusic = NULL;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;

struct Texture{
	SDL_Texture* mTexture;
	int mWidth,mHeight;
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

struct Timer{
	Uint32 mStartTime,mPausedTime;
	bool mIfPaused,mIfStarted;
	Timer(){
		mStartTime = 0;
		mPausedTime = 0;
		mIfPaused = false;
		mIfStarted = false;
	}
	void start(){
		mIfStarted = true;
		mIfPaused = false;
		mStartTime = SDL_GetTicks();
		mPausedTime = 0;
	}
	void stop(){
		mIfStarted = false;
		mIfPaused = false;
		mStartTime = 0;
		mPausedTime = 0;
	}
	void pause(){
		if(mIfStarted && !mIfPaused){
			mIfPaused = true;
			mPausedTime = SDL_GetTicks() - mStartTime;
			mStartTime = 0;
		}
	}
	void unpause(){
		if(mIfStarted && mIfPaused){
			mIfPaused = false;
			mStartTime = SDL_GetTicks() - mPausedTime;
			mPausedTime = 0;
		}
	}
	Uint32 getTicks(){
		Uint32 time = 0;
		if(mIfStarted){
			if(mIfPaused)time = mPausedTime;
			else time = SDL_GetTicks() - mStartTime;
		}
		return time;
	}
	bool isStarted(){return mIfStarted;}
	bool isPaused(){return mIfPaused && mIfStarted;}
};
