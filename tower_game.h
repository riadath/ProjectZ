#ifndef TOWERGAME_H
#define TOWERGAME_H

#include "texture_timer.h"

const int running_animation_frames = 6;

int gInitialX;

Texture gTowermanTexture;
Texture gTowerBackgroundTexture;
Texture gSidetower1Texture;
Texture gSidetower2Texture;
SDL_Rect gTowermanRunningClips[ running_animation_frames ];

std::deque<SDL_Rect>Platforms;


int y_coordinate;

bool init();

bool checkCollision_tower( SDL_Rect man,SDL_Rect plat );

bool Towerman_Collided_With_Platform( SDL_Rect A );

void loadTowerMedia();

void load_Tower();



struct Towerman
{
	SDL_RendererFlip mFlipType = SDL_FLIP_NONE;
	static const int Towerman_WIDTH = 59;
	static const int Towerman_HEIGHT = 67;

	static const int Towerman_Vel = 6;
	static const int jumping_vel = -17;
	static const int falling_vel = 1;
	static const int characterFrameDelay = 5;

	SDL_Rect* currentClip;


	int mPosX, mPosY;

	int mVelX, mVelY;

	int frame = 0;

    bool jumping = 0;
	bool prev = 0;
	bool special = 0;
	
	SDL_Rect mCollider;

	Towerman()
	{
		gInitialX = 160;
		mPosX = 160;
		mPosY = -30;

		mCollider.w =  Towerman_WIDTH;
		mCollider.h = Towerman_HEIGHT;

		mVelX = 0;
        mVelY = 0;
	}

	void handleEvent( SDL_Event& e )
	{
		if( e.type == SDL_KEYDOWN  )
		{
			switch( e.ksym )
			{
				case SDLK_UP:
				{
					if( jumping == 0 && e.key.repeat == 0 ){
						mVelY = jumping_vel;
						jumping = 1;
					}
					break;
				}
				case SDLK_LEFT: mVelX = -Towerman_Vel; break;
				case SDLK_RIGHT: mVelX = +Towerman_Vel; break;
			}
		}
		else if( e.type == SDL_KEYUP )
		{
			switch( e.ksym )
			{
				case SDLK_UP : break;
				case SDLK_LEFT: mVelX = 0; frame = 0; break;
				case SDLK_RIGHT: mVelX = 0; frame = 0; break;
			}
		}
	}
	void move( )
	{
		//X-axis
		mPosX += mVelX;
		mCollider.x = mPosX;
		if( ( mPosX < 0 ) || ( mPosX + Towerman_WIDTH > SCREEN_WIDTH) || (Towerman_Collided_With_Platform( mCollider ) == true  && mVelY >= 0) )
		{
			mPosX -= mVelX;
			mCollider.x = mPosX;
		}
		//Y-axis
		if( mVelY < 0 ){
			mPosY += mVelY;
			mCollider.y = mPosY;
			mVelY += falling_vel;
			if( mVelY > 0 ) mVelY = 0;
		}
		else if( mVelY > 0 ){
			mPosY += mVelY;
			mCollider.y = mPosY;
			if( Towerman_Collided_With_Platform( mCollider ) == true && special == 0 ){
				mPosY = y_coordinate - 70;
				mCollider.y = mPosY;
				mVelY = 0;
				jumping = 0;
			}
			if( Towerman_Collided_With_Platform( mCollider ) == false ) special = 0;
			mVelY = 6;
		}
		else{
			if( Towerman_Collided_With_Platform( mCollider ) == true ){
				mPosY += mVelY;
				mCollider.y = mPosY;
				special = 1;
			}
			mVelY = 6;
		}
	}
	void render()
	{
		if( mPosX != gInitialX ){
			if( mPosX%characterFrameDelay == 0 ) frame++;
			if( frame > 5 ) frame = 0;
			if( mPosX > gInitialX ) mFlipType = SDL_FLIP_NONE;
			else mFlipType = SDL_FLIP_HORIZONTAL;
			gInitialX = mPosX;
		}
		else{
			frame = 0;
		}
		currentClip = &gTowermanRunningClips[ frame ];
		gTowermanTexture.render( mPosX, mPosY, currentClip, mFlipType);

	}
};

bool Towerman_Collided_With_Platform( SDL_Rect A )
{
    for( int i = 0; i < Platforms.size(); i++ ){
        if( checkCollision_tower( A,Platforms[i] ) == true ){
			y_coordinate = Platforms[i].y;
			return true;
		}
    }
    return false;
}

bool checkCollision_tower( SDL_Rect a, SDL_Rect b )
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

void loadTowerMedia()
{
    gTowermanTexture.loadFile( "images/towergame/tower_man_running2.png" );
	gTowerBackgroundTexture.loadFile( "images/towergame/Tower_wall.png" );
	gSidetower1Texture.loadFile( "images/towergame/side_tower.jpg" );
	gSidetower2Texture.loadFile( "images/towergame/side_tower.jpg" );

	for( int i = 0; i < 6; i++ ){
		gTowermanRunningClips[i].x = 59*i;
		gTowermanRunningClips[i].y = 0;
		gTowermanRunningClips[i].w = 59;
		gTowermanRunningClips[i].h = 67;
	}
}

bool valid_platform( int a,int b,int x,int y )
{
	if( x < 150 || x > 1050 || y < 150 || y > 1050 ) return 0;
	if( y < a ){
		if( a-y < 80 ) return 1;
		else return 0;
	}
	else if( x > b ){
		if( x-b < 80 ) return 1;
		else return 0;
	}
	else return 1;
}
int a, b, x, y;
void load_Tower()
{
    SDL_Rect platform;
	int no_of_platforms = Platforms.size();
	for( int i = 0; i < no_of_platforms; i++ ) Platforms[i].y += 2;

	if( no_of_platforms == 0 ){
		platform = { 150,0,300,10 };
		a = 150;
		b = x + 300;
		Platforms.push_back(platform);
		no_of_platforms++;
		for( int i = 1; i <= 6; i++ ){
			while( 1 ){
				x = rand()%750+150;
				y = x + rand()%300 + 100;
				if( valid_platform(a,b,x,y) == 1 ){
					a = x;
					b = y;
					break;
				}
			}
			platform = { x,100*i,y-x,10 };
			Platforms.push_back(platform);
			no_of_platforms++;
		}
	}
	if( no_of_platforms < 7 ){
		while( 1 ){
				x = rand()%750+150;
				y = x+ rand()%300 + 100;
				if( valid_platform(a,b,x,y) == 1 ){
					a = x;
					b = y;
					break;
				}
			}
		platform = { x,-9,y-x,10 };
		Platforms.push_back( platform );
	}

	for( int i = 0; i < Platforms.size(); i++ ){
		SDL_SetRenderDrawColor( gRender, 255,244,79, 0xFF );		
		SDL_RenderFillRect( gRender, &Platforms[i] );
	}
	for( int i = 0; i < Platforms.size(); i++ ){
		if( Platforms[i].y > 700 ) Platforms.erase( Platforms.begin()+i );
	}

}

int towerGame()
{
    loadTowerMedia();	
    bool quit = false;

    SDL_Event e;

    Towerman Man;
    int scrollingOffset = 0;
    int scrollingSpeed = 2;

    int scrollingOffset2 = 0;
    int scrollingSpeed2 = 1;
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }

            Man.handleEvent( e );
        }
        if( Man.mPosY > 700 ) quit = true;

        SDL_SetRenderDrawColor( gRender, 0, 0, 255, 0xFF );
        SDL_RenderClear( gRender );

        scrollingOffset += scrollingSpeed;
        scrollingOffset2 += scrollingSpeed2;
        if( scrollingOffset > gTowerBackgroundTexture.getHeight() )
        {
            scrollingOffset = 0;
        }

        if( scrollingOffset2 > gTowerBackgroundTexture.getHeight() )
        {
            scrollingOffset2 = 0;
        }

        gTowerBackgroundTexture.render( 150, scrollingOffset );
        gTowerBackgroundTexture.render( 150, scrollingOffset - gTowerBackgroundTexture.getHeight() );

        gSidetower1Texture.render(0,scrollingOffset2);
        gSidetower1Texture.render(0,scrollingOffset2- gSidetower1Texture.getHeight() );

        gSidetower2Texture.render(1050,scrollingOffset2);
        gSidetower2Texture.render(1050,scrollingOffset2- gSidetower2Texture.getHeight() );

        Man.move( );
        load_Tower();

        Man.render( );
        SDL_RenderPresent( gRender );
    }

    return 0;
}

#endif