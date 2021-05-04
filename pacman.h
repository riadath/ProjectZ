#ifndef PACMAN_H
#define PACMAN_H

#include "texture_timer.h"

// using namespace std;

std::vector<SDL_Rect>WALL;

int point = 0;
int missed = 5;

bool if_collided( SDL_Rect a );

bool loadPacmanMedia();

void closePacman();

int pacman();

bool checkWallCollision( SDL_Rect a, SDL_Rect b );

Texture gPacmanTexture;
Texture gPacmanFoodTexture;
Texture gPacmanScoreTexture;
Texture gPacmanRemainingLivesTexture;

std::stringstream PacmanScoreText;
std::stringstream PacmanLivesText;

struct PacmanFood
{
	static const int Food_WIDTH = 20;
	static const int Food_HEIGHT = 20;


	int mPosX, mPosY;
	
	SDL_Rect mCollider;

	PacmanFood()
	{

		mPosX = rand()%1100;
		mPosY = rand()%650;

		mCollider.w = 20;
		mCollider.h = 20;

	}

	void render()
	{
		gPacmanFoodTexture.render( mPosX, mPosY,NULL);
	}
};

std::deque<std::pair<PacmanFood,int> >Food_queue;

struct Pacman
{
	SDL_RendererFlip mFlipType = SDL_FLIP_NONE;
	static const int DOT_WIDTH = 65;
	static const int DOT_HEIGHT = 65;

	static const int DOT_VEL = 8;

	int mPosX, mPosY;

	int mVelX, mVelY;
	
	SDL_Rect mCollider;

	Pacman()
	{
		mPosX = 0;
		mPosY = 300;

		mCollider.w = 65;
		mCollider.h = 65;

		mVelX = 0;
		mVelY = 0;
	}

	void handleEvent( SDL_Event& e )
	{
		if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
		{
			switch( e.key.keysym.sym )
			{
				case SDLK_UP: mVelY -= DOT_VEL;break;
				case SDLK_DOWN: mVelY += DOT_VEL;break;
				case SDLK_LEFT: mVelX -= DOT_VEL; mFlipType = SDL_FLIP_HORIZONTAL; break;
				case SDLK_RIGHT: mVelX += DOT_VEL; mFlipType = SDL_FLIP_NONE; break;
			}
		}
		else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
		{
			switch( e.key.keysym.sym )
			{
				case SDLK_UP: mVelY += DOT_VEL; break;
				case SDLK_DOWN: mVelY -= DOT_VEL; break;
				case SDLK_LEFT: mVelX += DOT_VEL; break;
				case SDLK_RIGHT: mVelX -= DOT_VEL; break;
			}
		}
	}
	void move( )
	{
		mPosX += mVelX;
		mCollider.x = mPosX;

		if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || if_collided( mCollider ) )
		{
			mPosX -= mVelX;
			mCollider.x = mPosX;
		}

		mPosY += mVelY;
		mCollider.y = mPosY;

		if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) || if_collided( mCollider ) )
		{
			mPosY -= mVelY;
			mCollider.y = mPosY;
		}
	}
	void render()
	{
		gPacmanTexture.render( mPosX, mPosY,NULL,mFlipType);
	}
};

void load_map()
{
	int x = SCREEN_WIDTH/10;
	int y = SCREEN_HEIGHT/8;
	SDL_Rect fillRect;
	fillRect = { 0, 0, 2*x , 2*y};
	SDL_SetRenderDrawColor( gRender, 0xFF, 0x00, 0x00, 0xFF );		
	SDL_RenderFillRect( gRender, &fillRect );
    WALL.push_back( fillRect );

	fillRect = { 3*x, 2*y, 4*x, y };
	SDL_SetRenderDrawColor( gRender, 0xFF, 0x00, 0x00, 0xFF );		
	SDL_RenderFillRect( gRender, &fillRect );
    WALL.push_back( fillRect );
			
	fillRect = { 8*x, y, x, 3*y };
	SDL_SetRenderDrawColor( gRender, 0xFF, 0x00, 0x00, 0xFF );		
	SDL_RenderFillRect( gRender, &fillRect );
    WALL.push_back( fillRect );
			
	fillRect = { 2*x, 4*y, x, 2*y };
	SDL_SetRenderDrawColor( gRender, 0xFF, 0x00, 0x00, 0xFF );		
	SDL_RenderFillRect( gRender, &fillRect );
    WALL.push_back( fillRect );

	fillRect = { 3*x, 5*y, x, y };
	SDL_SetRenderDrawColor( gRender, 0xFF, 0x00, 0x00, 0xFF );		
	SDL_RenderFillRect( gRender, &fillRect );
    WALL.push_back( fillRect );

	fillRect = { 6*x, 5*y, x, 3*y };
	SDL_SetRenderDrawColor( gRender, 0xFF, 0x00, 0x00, 0xFF );		
	SDL_RenderFillRect( gRender, &fillRect );
    WALL.push_back( fillRect );

	fillRect = { 7*x, 7*y, 3*x, y };
	SDL_SetRenderDrawColor( gRender, 0xFF, 0x00, 0x00, 0xFF );		
	SDL_RenderFillRect( gRender, &fillRect );
    WALL.push_back( fillRect );
	
	fillRect = { 0, 0, 2*x , 2*y};
	SDL_SetRenderDrawColor( gRender, 0, 0, 0, 0xFF );		
	SDL_RenderDrawRect( gRender, &fillRect );

	fillRect = { 3*x, 2*y, 4*x, y };
	SDL_SetRenderDrawColor( gRender, 0, 0, 0, 0xFF );			
	SDL_RenderDrawRect( gRender, &fillRect );
			
	fillRect = { 8*x, y, x, 3*y };
	SDL_SetRenderDrawColor( gRender, 0, 0, 0, 0xFF );			
	SDL_RenderDrawRect( gRender, &fillRect );
			
	fillRect = { 2*x, 4*y, x, 2*y };
	SDL_SetRenderDrawColor( gRender, 0, 0, 0, 0xFF );			
	SDL_RenderDrawRect( gRender, &fillRect );

	fillRect = { 3*x, 5*y, x, y };
	SDL_SetRenderDrawColor( gRender, 0, 0, 0, 0xFF );			
	SDL_RenderDrawRect( gRender, &fillRect );

	fillRect = { 6*x, 5*y, x, 3*y };
	SDL_SetRenderDrawColor( gRender, 0, 0, 0, 0xFF );		
	SDL_RenderDrawRect( gRender, &fillRect );

	fillRect = { 7*x, 7*y, 3*x, y };
	SDL_SetRenderDrawColor( gRender, 0, 0, 0, 0xFF );			
	SDL_RenderDrawRect( gRender, &fillRect );
}

bool loadPacmanMedia()
{
	int k = gPacmanTexture.loadFile( "images/png/pacman.png" );
	k = gPacmanFoodTexture.loadFile( "images/png/dot.png" );
	gFont = TTF_OpenFont("images/fonts/Oswald-BoldItalic.ttf",24);
    if(gFont == NULL){
        ERROR_T;
        return false;
    }
	return true;
}

void closePacman()
{
	gPacmanTexture.free();
	SDL_DestroyRenderer( gRender );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRender = NULL;
	IMG_Quit();
	SDL_Quit();
}

bool if_collided( SDL_Rect a )
{
    for( int i = 0; i < WALL.size(); i++ ){
        if(checkWallCollision( a,WALL[i] ) == true ) return true;
    }
    return false;
}

bool checkWallCollision_with_food( SDL_Rect a,PacmanFood F )
{
	SDL_Rect b = { F.mPosX,F.mPosY,20,20 };
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

    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    return true;

}

void if_collided_with_food( Pacman p )
{
	SDL_Rect a = { p.mPosX,p.mPosY,65,65 };
	for( int i = 0; i < Food_queue.size(); i++ ){
        if(checkWallCollision_with_food( a,Food_queue[i].first ) == true ){
			Food_queue.erase( Food_queue.begin()+i );
			point++;
		}
    }
}

bool checkWallCollision( SDL_Rect a, SDL_Rect b )
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

    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    return true;
}

void render_food( std::deque<std::pair<PacmanFood,int>>Food_queue )
{
	SDL_Color textColor = {0,0,0,255};
	for( int i = 0; i < Food_queue.size(); i++ ){
		Food_queue[i].first.render();
	}
	PacmanScoreText.str("");
    PacmanScoreText<<"Score : "<<point;
    if(!gPacmanScoreTexture.loadFromText(PacmanScoreText.str().c_str(),textColor)){
        printf("Unable to render score texture\n");
    }
	PacmanLivesText.str("");
    PacmanLivesText<<"Lives : "<<missed;
    if(!gPacmanRemainingLivesTexture.loadFromText(PacmanLivesText.str().c_str(),textColor)){
        printf("Unable to render score texture\n");
    }
	gPacmanScoreTexture.render( 0,0 );
	gPacmanRemainingLivesTexture.render(0,20);

}

bool is_FoodValid( PacmanFood temp )
{
	SDL_Rect spawned_food;
	spawned_food = { temp.mPosX, temp.mPosY, 20 , 20};
	return if_collided( spawned_food );
}

void initPacman(){
    point = 0;
    missed = 5;
    Food_queue.clear();
}

int pacman()
{
    initPacman();
	loadPacmanMedia();	

    bool quitPacman = false;

    SDL_Event e;

    Pacman pacman;

    Uint32 startTime = 0;

    int prev = 0;	
    while( !quitPacman )
    {   
        if(missed <= 0)quitPacman = true;

        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE) )
            {
                quitPacman = true;
            }
            pacman.handleEvent( e );
        }

        pacman.move( );

        if_collided_with_food( pacman );


        SDL_SetRenderDrawColor( gRender, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRender );

        load_map();

        int food_spawning_gap = 10000;
        int food_vanishing_gap = 5000;
        int curr_time = SDL_GetTicks();
        int food_number = curr_time/food_spawning_gap + 1;
        for( int i = 0; i < Food_queue.size(); i++ ){
            if( curr_time-Food_queue[i].second > food_vanishing_gap ){
                Food_queue.erase( Food_queue.begin()+i );
                missed--;
            }
        }
        
        PacmanFood temp;
        while( Food_queue.size() < food_number && is_FoodValid( temp ) == 0 ){
            Food_queue.push_back(std::make_pair(temp,curr_time) );
        }

        pacman.render( );
        render_food( Food_queue );
        
        SDL_RenderPresent( gRender );
    }

	closePacman();

	return point;
}

#endif