#ifndef GRAPHIC_CPP
#define GRAPHIC_CPP

#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "Box.cpp"

/**
 * imageList's name set
*/
enum IMAGELIST{
    HEADIMAGE, // snake's head
    BODYIMAGE, // snake's body
    MAPIMAGE,  // map floor
	HOVERIMAGE, // a black with 0.5 alpha
	CHERRYIMAGE, // an apple or a cherry 's image
	GAMEOVERIMAGE, // include a "GAMEOVER"
	PRESSZIMAGE, // include a "PRESS Z TO CONTINUE"
	BACKGROUNDIMAGE, // background
	NUMBER_0, // numbers
	NUMBER_1,
	NUMBER_2,
	NUMBER_3,
	NUMBER_4,
	NUMBER_5,
	NUMBER_6,
	NUMBER_7,
	NUMBER_8,
	NUMBER_9,
    END
};

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *imageList[END];
const char *windowTitle = "Snake";
const char *imageURLList[] = { 
	"./image/head.png",
	"./image/body.png",
	"./image/map-floor.png",
	"./image/hover.png",
	"./image/cherry.png",
	"./image/gameover.png",
	"./image/pressZ.png",
	"./image/bg.png",
	"./image/0.png",
	"./image/1.png",
	"./image/2.png",
	"./image/3.png",
	"./image/4.png",
	"./image/5.png",
	"./image/6.png",
	"./image/7.png",
	"./image/8.png",
	"./image/9.png"};
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

SDL_Texture * getImage(const char * URL);

/**
 * transform a box into a SDL_Rect
*/
SDL_Rect transform(Box);

bool initSDL();
void closeSDL();

/**
 * putImage
 * 1 var for texture
 * 3 vars in Box on renderer:   r_centrePos r_halfHeight r_halfWidth 
 * 3 vars in Box on texture:    t_centrePos t_halfHeight t_halfWidth (default case is full image)
*/
void putImage(SDL_Texture *texture, Box onRenderer = { {0,0},-1,-1 }, double angle = 0, Box onTexture = { {0,0},-1,-1 }); 

/**
 * for getImage(imageURLList)
*/
void loadImages();
void destoryImages();

/**
 * put an integer on the "point", the point is for the most left picture's centre
*/
void putNumber(int,point);

SDL_Rect transform(Box box) {
    SDL_Rect rect = {
		(int)(box.centrePos.x - box.halfWidth),
		(int)(box.centrePos.y - box.halfHeight),
		(int)(2*box.halfWidth),(int)(2*box.halfHeight)};
    return rect;
}
SDL_Texture * getImage(const char * URL) {
    SDL_Surface *image = IMG_Load(URL);
	if (image == NULL) {
		printf("IMG_Load <%s> failed\n" ,URL);
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);
	return texture;
}
bool initSDL() {
	if (IMG_Init(IMG_INIT_PNG) == -1) {
		printf("[ERROR] Init Image_PNG : %s\n",SDL_GetError());
		return 0;
	}
	//init the window
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		printf("[ERROR] Init SDL : %s\n",SDL_GetError());
		return 0;
	}
	//create a window
	window = SDL_CreateWindow( windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		printf("[ERROR] Create Window : %s\n",SDL_GetError());
		return 0;
	}
	//set icon
	
	SDL_Surface *_icon= IMG_Load("./image/icon.png");
	if (_icon == nullptr) {
		printf("[ERROR] Image_jpg : %s\n",SDL_GetError());
		return 0;
	}
	SDL_SetWindowIcon(window, _icon);
	SDL_FreeSurface(_icon);
	_icon = nullptr;
	
	//create a renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		printf("[ERROR] Create Renderer : %s\n",SDL_GetError());
		return 0;
	}

	//Textures Load
    loadImages();
	return 1;
}
void closeSDL() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = nullptr;
    renderer = nullptr;

    destoryImages();
    IMG_Quit();
    SDL_Quit();
}
void putImage(SDL_Texture *texture, Box onRenderer, double angle, Box onTexture) {
    SDL_Rect srcrect = transform(onTexture), dstrect = transform(onRenderer);
    SDL_RenderCopyEx(renderer, texture, (onTexture.halfHeight==-1)?nullptr:&srcrect, (onRenderer.halfHeight==-1)?nullptr:&dstrect,angle,nullptr,(SDL_RendererFlip)0);
}
void loadImages() {
    for (int i = 0; i < END; i ++)
    {
		std::cout << "loading:" << imageURLList[i] << "\n";
        imageList[i] = getImage(imageURLList[i]);
        if (imageList[i] == nullptr) {
            std::cerr << "[getImage ERROR]" << imageURLList[i] << " load failed.\n";
        }
		else {
			std::clog << "getImage success:" << imageURLList[i] << "\n";
		}
    }
}
void destoryImages() {
    for (int i = 0; i < END; i ++)
    {
        SDL_DestroyTexture(imageList[i]);
        imageList[i] = nullptr;
    }
}

// ATTENTION : this function have not checked its correctness, may have some bugs.

const int numberWidth  = 10; // temporary number
const int numberHeight = 10; // temporary number

void putNumber(int n, point pos) {
	int digits[10]={0};
	int highestDigit(0);
	for(int i = 0; i < 10; i ++) {
		digits[i] = (n%10);
		n /= 10;
		if(digits[i]) {
			highestDigit = i;
		}
	}
	for(int i = highestDigit; i >= 0; i --) {
		Box onRenderer = {pos, numberWidth/2, numberHeight/2};
		putImage(imageList[digits[i]+8],onRenderer);
		pos = pos + (point){numberWidth,0};
	}
}

#endif 