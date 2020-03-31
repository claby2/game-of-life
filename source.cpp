#include <SDL2/SDL.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

const int SCREEN_HEIGHT = 1000;
const int SCREEN_WIDTH = 1000;

const int FRAME_RATE = 500;

const int height = 100;
const int width = 100;

const uint32_t WHITE = 0xffffffff;
const uint32_t BLACK = 0xff000000;

int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

bool init() {
	bool success = true;
	if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		gWindow = SDL_CreateWindow( "Game Of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
		if(gWindow == NULL) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if(gRenderer == NULL) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
			}
		}
	}
	return success;
}

void setPixels(std::vector<uint32_t>& pixels) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(rand()%2) {
                pixels[i*width + j] = BLACK;
            }
        }
    }
}

int countNeighbors(std::vector<uint32_t>& pixels, int x, int y) {
    int s = 0;
    for(int i = 0; i < 8; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if(nx < 0 || ny < 0) continue;
        if(nx >= width || ny >= height) continue;
        if(pixels[ny*width + nx] == WHITE) s++;
    }
    return s;
}

void applyRules(std::vector<uint32_t>& pixels, std::vector<uint32_t>& displayPixels) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int x = countNeighbors(pixels, j, i);
            uint32_t state = pixels[i*width + j];
            if(state == BLACK && x == 3) {
                displayPixels[i*width + j] = WHITE;
            } else if(pixels[i*width + j] == WHITE && (x < 2 || x > 3)) {
                displayPixels[i*width + j] = BLACK;
            } else {
                displayPixels[i*width + j] = state;
            }
        }
    }
}

int main(int argc, char* args[]){
    srand((unsigned)time(NULL));
    if(!init()){
        printf("Failed to initialize!\n");
    } else {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        SDL_Texture* gTexture = SDL_CreateTexture(
            gRenderer,
            SDL_PIXELFORMAT_ABGR8888,
            SDL_TEXTUREACCESS_STATIC,
            width,
            height
        );
        bool quit = false;

        std::vector<uint32_t> pixels(height*width, WHITE);
        std::vector<uint32_t> displayPixels(height*width, WHITE);
        setPixels(pixels); 
        SDL_Event e;

        int frame = 0;

        while(!quit){
            while(SDL_PollEvent( &e ) != 0) {
                if( e.type == SDL_QUIT ) {
                    quit = true;
                } else if(e.type == SDL_KEYDOWN){
                    switch(e.key.keysym.sym){
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                    }
                }
            }
            SDL_UpdateTexture(gTexture, NULL, &pixels[0], width * sizeof(uint32_t));
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
            SDL_RenderPresent(gRenderer);
            displayPixels.clear();
            frame++;
            if(frame > FRAME_RATE) {
                applyRules(pixels, displayPixels);
                frame = 0;
                for(int i = 0; i < pixels.size(); i++) {
                    pixels[i] = displayPixels[i];
                }
            }
        }
        SDL_DestroyTexture(gTexture);
    }
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;
    SDL_Quit();
    return 0;
}