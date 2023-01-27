#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

int running = FALSE;
SDL_Window* window = NULL;
SDL_Surface* windowSurface = NULL;

int last_frame_time = 0;
float delta_time;

int Room;
SDL_Surface* P = NULL;
SDL_Surface* F = NULL;
SDL_Surface* PTT = NULL;
SDL_Surface* PTB = NULL;
SDL_Surface* PTL = NULL;
SDL_Surface* TLB = NULL;
SDL_Surface* LBC = NULL;
SDL_Surface* EBC = NULL;
SDL_Surface* ETL = NULL;
SDL_Surface* ELB = NULL;
SDL_Surface* QDB = NULL;
SDL_Surface* EDB = NULL;
SDL_Surface* BEC = NULL;

int colR,colL,colU,colD;

struct game_object {
	float x;
	float y;
	float width;
	float height;
	SDL_Surface* surface;
} player;

SDL_Rect player_rect;

int Init(void) {
	if ((SDL_Init(SDL_INIT_VIDEO)) != 0)
	{
		fprintf(stderr, "Error in SDL_Init. \n");
		return FALSE;
	}
	window = SDL_CreateWindow(
		"NULL",                 //Title
		SDL_WINDOWPOS_CENTERED, // X
		SDL_WINDOWPOS_CENTERED, // Y
		WINDOW_WIDTH,           // Width
		WINDOW_HEIGHT,          // Height
		SDL_WINDOW_BORDERLESS   //flags
	);
	if(!window){
		fprintf(stderr,"Error creating SDL Window.\n");
		return FALSE;
	}

	windowSurface = SDL_GetWindowSurface( window );
	
	return TRUE;
}

void Setup(){
	player.x = 64;
	player.y = 64;
	player.width = 64;
	player.height = 64;
	player.surface = SDL_LoadBMP( "Assets/player.bmp" );

	Room = 1;
}

void Input(){
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type){
		case SDL_QUIT:
			running = FALSE;
			break;
		case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE)
				running = FALSE;
			break;
	}

	const Uint8* keystate = SDL_GetKeyboardState(NULL); 

	if(keystate[SDL_SCANCODE_RIGHT] && colR != TRUE)
		player.x += 320 * delta_time;
	if(keystate[SDL_SCANCODE_LEFT]  && colL != TRUE)
		player.x -= 320 * delta_time;
	if(keystate[SDL_SCANCODE_DOWN]  && colD != TRUE)
		player.y += 320 * delta_time;
	if(keystate[SDL_SCANCODE_UP]    && colU != TRUE)
		player.y -= 320 * delta_time;
}

void Update(){
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(),last_frame_time + FRAME_TARGET_TIME));
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
	{
		SDL_Delay(time_to_wait);
	}
	delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
	last_frame_time = SDL_GetTicks();

	player.width = 64;
	player.height = 64;

	if(Room == 1){
		SDL_Rect col1 = {0,0,24,WINDOW_HEIGHT};
		SDL_Rect col2 = {0,0,WINDOW_WIDTH,80};
		SDL_Rect col3 = {0,WINDOW_HEIGHT-24,WINDOW_WIDTH,24};
		SDL_Rect col4 = {WINDOW_WIDTH-24,0,24,WINDOW_HEIGHT};
		if(SDL_HasIntersection(&player_rect, &col1)){colL = TRUE;}else{colL = FALSE;}
		if(SDL_HasIntersection(&player_rect, &col2)){colU = TRUE;}else{colU = FALSE;}
		if(SDL_HasIntersection(&player_rect, &col3)){colD = TRUE;}else{colD = FALSE;}
		if(SDL_HasIntersection(&player_rect, &col4)){colR = TRUE;}else{colR = FALSE;}
	}
}

void Draw(){

	if(P == NULL){
		P   = SDL_LoadBMP( "Assets/P.bmp" );
		F   = SDL_LoadBMP( "Assets/F.bmp" );
		PTT = SDL_LoadBMP( "Assets/PTT.bmp" );
		PTB = SDL_LoadBMP( "Assets/PTB.bmp" );
		PTL = SDL_LoadBMP( "Assets/PTL.bmp" );
		TLB = SDL_LoadBMP( "Assets/TLB.bmp" );
		LBC = SDL_LoadBMP( "Assets/LBC.bmp" );
		EBC = SDL_LoadBMP( "Assets/EBC.bmp" );
		ETL = SDL_LoadBMP( "Assets/ETL.bmp" );
		ELB = SDL_LoadBMP( "Assets/ELB.bmp" );
		QDB = SDL_LoadBMP( "Assets/QDB.bmp" );
		EDB = SDL_LoadBMP( "Assets/EDB.bmp" );
		BEC = SDL_LoadBMP( "Assets/BEC.bmp" );
	}
	SDL_Surface* Sala01[7][10] = {
        {PTL,PTT,PTT,PTT,PTT,PTT,PTT,PTT,PTT,ETL},
        {TLB,PTB,PTB,PTB,PTB,PTB,PTB,PTB,PTB,ELB},
        {LBC, F , F , F , F , F , F , F , F ,EBC},
        {LBC, F , F , F , F , F , F , F , F ,EBC},
        {LBC, F , F , F , F , F , F , F , F ,EBC},
        {LBC, F , F , F , F , F , F , F , F ,EBC},
        {QDB,BEC,BEC,BEC,BEC,BEC,BEC,BEC,BEC,EDB}
    };

	SDL_Rect clearRect = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};
    SDL_FillRect(windowSurface,&clearRect,SDL_MapRGB(windowSurface->format,0,0,0));

	for (int y = 0; y < 7; ++y)
    {
        for (int x = 0; x < 10; ++x)
        {
            SDL_Rect asdf = {x*64,y*64,64,64};
            if(Room == 1)
            SDL_BlitScaled(Sala01[y][x], NULL, windowSurface, &asdf);
        }
    }

    player_rect.x = player.x;
    player_rect.y = player.y;
    player_rect.w = player.width;
    player_rect.h = player.height;

    SDL_BlitScaled(player.surface, NULL, windowSurface, &player_rect);

	SDL_UpdateWindowSurface( window );
}

void Close(){
	SDL_FreeSurface( windowSurface );
    windowSurface = NULL;
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(){
	running = Init();

	Setup();
	while (running) {
		Input();
		Update();
		Draw();
	}
	Close();

	return 0;
}
