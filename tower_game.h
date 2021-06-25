#ifndef TOWERGAME_H
#define TOWERGAME_H

#include "texture_timer.h"

const int RUNNING_ANIMATION_FRAMES = 6;

int gInitialX;
//left and right coordinate of the new platform and the old platyform
int gPrevX, gPrevY, gPresX, gPresY;

Texture gTowermanTexture;
Texture gTowerBackgroundTexture;
Texture gSidetower1Texture;
Texture gSidetower2Texture;
SDL_Rect gTowermanRunningClips[ RUNNING_ANIMATION_FRAMES ];

std::deque<SDL_Rect>Platforms;

//variable to regulate the downward speed of the platforms
int gPlatformSpeed = 0;
int gYcoordinate;
int gTowerDelay = 2;
int gVelocity = 1;
int gTowerScoreDelay = 0;
int gScore;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const int gTowergameButtonCount = 4;
bool gIfResumeTowergame = false;

Texture gTowerButtonTexture[gTowergameButtonCount];
Texture gTowerMenuTexture;
Texture gTowerBackTexture;
Texture gTowerHighscoreTexture;

SDL_Rect gTowergameButtonPosition[gTowergameButtonCount];
SDL_Rect gTowerBackButtonPosition;
SDL_Rect gTowerHighscorePosition;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//rectangular collision detection function
bool checkCollision_tower( SDL_Rect man,SDL_Rect plat );

bool Towerman_Collided_With_Platform( SDL_Rect A );

//To check is a randomly spawned platform is valid and not impossible to reach for the character
bool valid_platform( int a,int b,int x,int y );

bool loadTowerMedia();

void render_score();

//loads and renders the platforms
void load_Tower();

Timer gTowerTimer;

struct Towerman
{
	SDL_RendererFlip mFlipType = SDL_FLIP_NONE;
	static const int Towerman_WIDTH = 59;
	static const int Towerman_HEIGHT = 67;

	static const int Towerman_Vel = 6;
	static const int jumping_vel = -14;
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
				case SDLK_w:
				{
					if( jumping == 0 && e.key.repeat == 0 ){
						mVelY = jumping_vel;
						jumping = 1;
					}
					break;
				}
				case SDLK_a: mVelX = -Towerman_Vel; break;
				case SDLK_d: mVelX = +Towerman_Vel; break;
			}
		}
		else if( e.type == SDL_KEYUP )
		{
			switch( e.ksym )
			{
				case SDLK_w : break;
				case SDLK_a: mVelX = 0; frame = 0; break;
				case SDLK_d: mVelX = 0; frame = 0; break;
			}
		}
	}
	void move( )
	{
		//X-axis
		mPosX += mVelX;
		mCollider.x = mPosX;
		if( ( mPosX < 150 ) || ( mPosX + Towerman_WIDTH > SCREEN_WIDTH - 150) || (Towerman_Collided_With_Platform( mCollider ) == true  && mVelY >= 0) )
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
				mPosY = gYcoordinate - 70;
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
Towerman Man;


bool Towerman_Collided_With_Platform( SDL_Rect A )
{
    for( int i = 0; i < Platforms.size(); i++ ){
        if( checkCollision_tower( A,Platforms[i] ) == true ){
			gYcoordinate = Platforms[i].y;
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

bool loadTowerMedia()
{
    gTowermanTexture.loadFile( "images/towergame/tower_man_running2.png" );
	gTowerBackgroundTexture.loadFile( "images/towergame/Tower_wall.png" );
	gSidetower1Texture.loadFile( "images/towergame/side_tower.jpg" );
	gSidetower2Texture.loadFile( "images/towergame/side_tower.jpg" );

	if (!gTowerButtonTexture[PLAY].loadFile("images/main/play_button.png"))
		return false;
	if (!gTowerButtonTexture[CONTINUE].loadFile("images/main/continue_button.png"))
		return false;
	if (!gTowerButtonTexture[HELP].loadFile("images/main/help_button.png"))
		return false;
	if (!gTowerButtonTexture[EXIT].loadFile("images/main/exit_button.png"))
		return false;
	if (!gTowerMenuTexture.loadFile("images/towergame/menu_back.png"))
		return false;
	if (!gTowerBackTexture.loadFile("images/main/back_button.png"))
		return false;
	if (!gTowerHighscoreTexture.loadFile("images/main/highscore_button.png"))
		return false;

	gFont = TTF_OpenFont("images/fonts/Oswald-BoldItalic.ttf", 24);
	if (gFont == NULL)
	{
		ERROR_T;
	}

	for( int i = 0; i < 6; i++ ){
		gTowermanRunningClips[i].x = 59*i;
		gTowermanRunningClips[i].y = 0;
		gTowermanRunningClips[i].w = 59;
		gTowermanRunningClips[i].h = 67;
	}

	for (int i = 0; i < gTowergameButtonCount; i++)
	{
		gTowergameButtonPosition[i].x = SCREEN_WIDTH / 2 - 200;
		gTowergameButtonPosition[i].y = (i - 1) * (i != 0) * 100 + 100;
		gTowergameButtonPosition[i].w = 309;
		gTowergameButtonPosition[i].h = 55;
	}

	gTowerHighscorePosition.x = SCREEN_WIDTH / 2 - 200;
	gTowerHighscorePosition.y = 3 * 100 + 100;
	gTowerHighscorePosition.w = 309;
	gTowerHighscorePosition.h = 55;

	gTowerBackButtonPosition.x = 0;
	gTowerBackButtonPosition.y = 0;
	gTowerBackButtonPosition.w = gTowerBackTexture.getWidth();
	gTowerBackButtonPosition.h = gTowerBackTexture.getHeight();

	return true;
}

bool valid_platform( int a,int b,int x,int y )
{
	if( x < 150 || x > 1050 || y < 150 || y > 1050 ) return 0;
	if( y < a ){
		if( abs(a-y) < 50 ) return 1;
		else return 0;
	}
	else if( x > b ){
		if( abs(x-b) < 50 ) return 1;
		else return 0;
	}
	else return 1;
}

void load_Tower()
{
	gPlatformSpeed++;

	int current_time = gTowerTimer.getTicks();
	gVelocity = current_time/15000 + 1;
	if( gPlatformSpeed > 100 ) gPlatformSpeed = 0;
    SDL_Rect platform;
	int no_of_platforms = Platforms.size();

	if( gPlatformSpeed%2 == 0 ) for( int i = 0; i < no_of_platforms; i++ ) Platforms[i].y += gVelocity;

	if( no_of_platforms == 0 ){
		platform = { 150,0,300,10 };
		gPrevX = 150;
		gPrevY = gPresX + 300;
		Platforms.push_back(platform);
		no_of_platforms++;
		for( int i = 1; i <= 6; i++ ){
			while( 1 ){
				gPresX = rand()%750+150;
				gPresY = gPresX + rand()%300 + 100;
				if( valid_platform(gPrevX,gPrevY,gPresX,gPresY) == 1 ){
					gPrevX = gPresX;
					gPrevY = gPresY;
					break;
				}
			}
			platform = { gPresX,100*i,gPresY-gPresX,10 };
			Platforms.push_back(platform);
			no_of_platforms++;
		}
	}
	if( no_of_platforms < 7 ){
		while( 1 ){
				gPresX = rand()%750+150;
				gPresY = gPresX+ rand()%300 + 100;
				if( valid_platform(gPrevX,gPrevY,gPresX,gPresY) == 1 ){
					gPrevX = gPresX;
					gPrevY = gPresY;
					break;
				}
			}
		platform = { gPresX,-9,gPresY-gPresX,10 };
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

void render_score()
{
	Texture towermanScore;
	std :: stringstream towermantext;

	towermantext.str("");
	towermantext << "Score : " << gScore;
	if (!towermanScore.loadFromText(towermantext.str().c_str(), {255,255,255,0}))
	{
		printf("Unable to render score texture\n");
	}
	towermanScore.render(0, 0);
}
void tower_Game_Init()
{
	gTowerTimer.start();
	Platforms.clear();
	//initial coordinates of the character
    Man.mPosY = -30;
    Man.mPosX = 160;
	
	gPlatformSpeed = 0;
	gYcoordinate;
	gTowerDelay = 2;
	gVelocity = 1;
	gTowerScoreDelay = 0;
	gIfResumeTowergame = false;
	Man.mVelX = 0;
	Man.mVelY = 0;
	Man.frame = 0;
    Man.jumping = 0;
	Man.prev = 0;
	Man.special = 0;
	
}



MENU_OPTIONS handleTowergameUI(SDL_Event &e)
{
	gTowerTimer.stop();

	bool quit = false;
	while (!quit)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		// Mouse hover animation on button
		for (int i = 0; i < gTowergameButtonCount; i++)
		{
			gTowerButtonTexture[i].setAlpha(255);
			if (mouseX >= gTowergameButtonPosition[i].x && mouseX <= gTowergameButtonPosition[i].x + gTowergameButtonPosition[i].w && mouseY >= gTowergameButtonPosition[i].y && mouseY <= gTowergameButtonPosition[i].y + gTowergameButtonPosition[i].h)
			{
				gTowerButtonTexture[i].setAlpha(200);
			}
		}

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return FULL_EXIT;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				//check mouse_button position
				if (mouseX >= gTowergameButtonPosition[PLAY].x && mouseX <= gTowergameButtonPosition[PLAY].x + gTowergameButtonPosition[PLAY].w && mouseY >= gTowergameButtonPosition[PLAY].y && mouseY <= gTowergameButtonPosition[PLAY].y + gTowergameButtonPosition[PLAY].h)
					return START_GAME;

				if (mouseX >= gTowergameButtonPosition[EXIT].x && mouseX <= gTowergameButtonPosition[EXIT].x + gTowergameButtonPosition[EXIT].w && mouseY >= gTowergameButtonPosition[EXIT].y && mouseY <= gTowergameButtonPosition[EXIT].y + gTowergameButtonPosition[EXIT].h)
					return FULL_EXIT;
				// if (mouseX >= gTowergameButtonPosition[HELP].x && mouseX <= gTowergameButtonPosition[HELP].x + gTowergameButtonPosition[HELP].w && mouseY >= gTowergameButtonPosition[HELP].y && mouseY <= gTowergameButtonPosition[HELP].y + gTowergameButtonPosition[HELP].h)
				// 	return HELP_MENU;

				if (mouseX >= gTowerHighscorePosition.x && mouseX <= gTowerHighscorePosition.x + gTowerHighscorePosition.w && mouseY >= gTowerHighscorePosition.y && mouseY <= gTowerHighscorePosition.y + gTowerHighscorePosition.h)
					return SHOW_HIGHSCORE;
			}
		}
		//render UI
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);

		//render UI background and buttons
		gTowerMenuTexture.render(0, 0);
		int st = 0, skip = -1;
		if (gIfResumeTowergame)
			st = 1;
		else
			skip = 1;

		for (int i = st; i < gTowergameButtonCount; i++)
		{
			if (skip != -1 && i == skip)
				continue;
			int pos = i;
			if (i == 1)
				pos = i - st;
			gTowerButtonTexture[i].render(gTowergameButtonPosition[pos].x, gTowergameButtonPosition[pos].y);
		}
		gTowerHighscoreTexture.render(gTowerHighscorePosition.x, gTowerHighscorePosition.y);
		SDL_RenderPresent(gRender);
	}
	return LOADING_SCREEN;
}

MENU_OPTIONS showTowergameScore(SDL_Event &e, std::string username)
{
	bool quit = false;
	gIfResumeTowergame = false;
	Texture tScoreTexture;
	SDL_Color textColor = {255, 180, 0, 255};
	std::stringstream tempText;
	tempText << "Your Score : " << gScore;
	tScoreTexture.loadFromText(tempText.str().c_str(), textColor);

	//insert score to high score list
	std::vector<std::pair<std::string, int>> scoreList;
	std::ifstream highscoreFile;

	highscoreFile.open("saved_files/towergame.score");
	if (!highscoreFile)
	{
		printf("Could not open file\n");
	}
	std::string tStr;
	int tScore;
	while (highscoreFile.eof() == false)
	{
		highscoreFile >> tStr >> tScore;
		if (highscoreFile.eof() == false)
			scoreList.push_back(std::make_pair(tStr, tScore));
	}
	highscoreFile.close();

	std::ofstream highscoreFileOut;
	highscoreFileOut.open("saved_files/towergame.score");
	if (!highscoreFileOut)
	{
		printf("Could not open file\n");
	}

	scoreList.push_back(std::make_pair(username, gScore));
	std::sort(scoreList.begin(), scoreList.end(), comp1);
	std::map<std::string, bool> tCheck;

	for (int i = 0; i < std::min(10, (int)scoreList.size()); i++)
	{
		highscoreFileOut << scoreList[i].first << " " << scoreList[i].second << "\n";
	}

	highscoreFileOut.close();
	gScore = 0;

	while (!quit)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return FULL_EXIT;
			}
			else if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
			{
				return LOADING_SCREEN;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (mouseX >= gTowerBackButtonPosition.x && mouseX <= gTowerBackButtonPosition.x + gTowerBackButtonPosition.w && mouseY >= gTowerBackButtonPosition.y && mouseY <= gTowerBackButtonPosition.y + gTowerBackButtonPosition.h)
				{
					return LOADING_SCREEN;
				}
			}
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);
		gTowerMenuTexture.render(0, 0);
		gTowerBackTexture.render(gTowerBackButtonPosition.x, gTowerBackButtonPosition.y);
		tScoreTexture.render(SCREEN_WIDTH / 2 - tScoreTexture.getWidth(), 100);
		SDL_RenderPresent(gRender);
	}
	return FULL_EXIT;
}

MENU_OPTIONS showTowergameHighScore(SDL_Event &e)
{
	bool quit = false;
	while (!quit)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return FULL_EXIT;
			}
			else if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
			{
				return LOADING_SCREEN;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (mouseX >= gTowerBackButtonPosition.x && mouseX <= gTowerBackButtonPosition.x + gTowerBackButtonPosition.w && mouseY >= gTowerBackButtonPosition.y && mouseY <= gTowerBackButtonPosition.y + gTowerBackButtonPosition.h)
				{
					return LOADING_SCREEN;
				}
			}
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);
		gTowerMenuTexture.render(0, 0);
		gTowerBackTexture.render(gTowerBackButtonPosition.x, gTowerBackButtonPosition.y);

		std::ifstream highscore;
		highscore.open("saved_files/towergame.score");
		int position = 1;
		Texture tHighscore;
		tHighscore.loadFromText("User           Score",{255,188,0,255});
		tHighscore.render(200, 0);
		while (highscore.eof() == false)
		{
			std::string str;
			int score;

			highscore >> str >> score;

			// std::cout<<str<<"  "<<score<<"\n";

			if(highscore.eof() == false){
				std::stringstream tempText;
				tempText << position++ << ". " << str << "           " << score;
				tHighscore.loadFromText(tempText.str().c_str(), {255, 255, 255, 255});
				tHighscore.render(200, (position - 1) * 50);
			}
		}
		highscore.close();
		SDL_RenderPresent(gRender);
	}
	return FULL_EXIT;
}



int towerGame(std::string username)
{
    loadTowerMedia();	
    
	tower_Game_Init();

	bool game_quit = false;
	
	SDL_Event e;
	MENU_OPTIONS menuState = LOADING_SCREEN;

	//scrolling speed of the background
	while(!game_quit){
		int scrollingOffset = 0;
		int scrollingSpeed = gVelocity;
		int scrollingOffset2 = 0;
		int scrollingSpeed2 = gVelocity;
  
		bool quit = true;
		if (menuState == LOADING_SCREEN)
		{
			menuState = handleTowergameUI(e);
		}
		if (menuState == FULL_EXIT)
		{
			game_quit = true;
		}
		if (menuState == START_GAME)
		{
			quit = false;
		}
		if (menuState == SHOW_SCORE)
		{
			menuState = showTowergameScore(e, username);
		}
		if (menuState == SHOW_HIGHSCORE)
		{
			menuState = showTowergameHighScore(e);
		}
		while( !quit )
		{
			gIfResumeTowergame = true;
			gTowerScoreDelay++;
			if( gTowerScoreDelay%5 == 0 ) gScore++;
			while( SDL_PollEvent( &e ) != 0 )
			{
				if( e.type == SDL_QUIT )
				{
					quit = true;
					gIfResumeTowergame = false;
					menuState = LOADING_SCREEN;
				}

				if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
				{
					quit = true;
					menuState = LOADING_SCREEN;
				}

				Man.handleEvent( e );
			}

			SDL_SetRenderDrawColor( gRender, 0, 0, 255, 0xFF );
			SDL_RenderClear( gRender );

			scrollingOffset += scrollingSpeed;
			gPlatformSpeed++;
			if( (gPlatformSpeed/gTowerDelay)%2 == 0 ) scrollingOffset2 += scrollingSpeed2;
			
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

			render_score();

			Man.move( );
			load_Tower();

			Man.render( );
			
			SDL_RenderPresent( gRender );
			if( Man.mPosY > 700 ){
				menuState = SHOW_SCORE;
				quit = true;
				tower_Game_Init();
			}
		}
	}

    return 0;
}

#endif