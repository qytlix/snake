#include <iostream>
#include <random>
#include "include/Graphic.cpp"
#include "include/Timer.cpp"
#include "include/Keyboard.cpp"
using namespace std;

// every body and head face to.
enum DIRECTION { 
    UP=0,
    RIGHT=90,
    DOWN=180,
    LEFT=270
};

// mapbool infomation
enum MAPBOOL {
    FLOOR,
    CHERRY,
    SNAKE,
    HEAD,
    TAIL
};

bool quit(0); // main loop wheather quit
Uint pretick(0); // ticks sinse program starting
int snakeLength(0); // the length of all snake ( exclude original 3 blocks )

int mapbool[16][12]; // exist infomation
int mapto[16][12]; // facing infomation
int mapfrom[16][12]; // follow this way, the head could find the tail

void initSnake(); // init map and original snake
void paintMap(); // paint on renderer based on mapbool and mapto
void makeCherry(); // make a cherry on the map
int getrand(int min,int max); // get a random integer at [min, max]
void eating(); // eating a cherry
void printOutAll(); // this is for all print in the main loop

struct integerPos
{
    int x,y;
    void up(){
        x+=15;
        x%=16;
    }
    void down(){
        x+=1;
        x%=16;
    }
    void left(){
        y+=11;
        y%=12;
    }
    void right(){
        y+=1;
        y%=12;
    }
    void leaveAway(){
        mapbool[x][y]=FLOOR;
    }
    void sitDown(int type){
        mapbool[x][y]=type;
    }
    void pointOutDirectionOfMapto(int direction){
        mapto[x][y]=direction;
    }
    void pointOutDirectionOfMapfrom(int oppositeDirection){ // ATTENTION:use the opposite direction
        mapfrom[x][y]=(oppositeDirection+180)%360;
    }
    void nextStep(){
        if (mapto[x][y] == UP)         this->up();
        else if (mapto[x][y] == DOWN)  this->down();
        else if (mapto[x][y] == LEFT)  this->left();
        else if (mapto[x][y] == RIGHT) this->right();
    }
    void beforeStep(){
        if (mapfrom[x][y] == UP)         this->up();
        else if (mapfrom[x][y] == DOWN)  this->down();
        else if (mapfrom[x][y] == LEFT)  this->left();
        else if (mapfrom[x][y] == RIGHT) this->right();
    }
};

#undef main
int main()
{
    if(!initSDL()) {
        cerr << "[initSDL ERROR]";
        return -1;
    }
    initSnake();
    SDL_Event event;
    Timer screenFresh = Timer(16);
    startListenKB();

    integerPos headPosition = {6,9},tailPosition = {6,7};
    Timer keyCooldown = Timer(80); // the CD for press KB
    
    int nowDirection = RIGHT;
    bool alreadyNextStep = 0;
    bool gameover = 0;

    while(!quit) {
        if (screenFresh.check()) {
            pretick ++;
            SDL_RenderPresent(renderer);
            SDL_RenderClear(renderer);
            printOutAll();
            if (pretick % 5 == 0 and gameover == 0){
                headPosition.sitDown(SNAKE);
                headPosition.pointOutDirectionOfMapto(nowDirection);
                headPosition.nextStep();
                alreadyNextStep = 1;
                headPosition.pointOutDirectionOfMapto(nowDirection);
                
                // this input is opposite direction of the value in "mapfrom"
                headPosition.pointOutDirectionOfMapfrom(nowDirection);
                
                if (mapbool[headPosition.x][headPosition.y]==SNAKE or mapbool[headPosition.x][headPosition.y]==TAIL){
                    // headPosition.beforeStep();
                    
                    // here is a question: if gameover here, head will eat a body.
                    gameover = 1;
                }
                else if (mapbool[headPosition.x][headPosition.y]==CHERRY) {
                    eating();
                    makeCherry();
                }
                else {
                    tailPosition.leaveAway();
                    tailPosition.nextStep();
                    tailPosition.sitDown(TAIL);
                }
                headPosition.sitDown(HEAD);
            }
            else if (gameover) {
                putImage(imageList[HOVERIMAGE]); // when it died, the screen will be grow down
                
                // let "GAMEOVER" flash
                if((pretick/25)%2) {
                    putImage(imageList[GAMEOVERIMAGE],{{400,250},275,400});
                }
                
                
                putImage(imageList[PRESSZIMAGE]  ,{{400,550},50,400});
            }
            /**
             * What should do in main loop?
             * 1.get kb state and reflection.
             * 2.paint on renderer based on map and to.
             * 3.refresh renderer.
            */            
        }
        // check the keyboard
        // this can be another Function
        if (keyCooldown.ocheck())
        {
            if(!gameover)
            {
                if (ifPress(SDL_SCANCODE_UP) and alreadyNextStep == 1) {                
                    if (nowDirection != DOWN) nowDirection = UP;
                    alreadyNextStep = 0;
                    keyCooldown.update();
                }
                if (ifPress(SDL_SCANCODE_DOWN) and alreadyNextStep == 1) {
                    if (nowDirection != UP) nowDirection = DOWN;
                    alreadyNextStep = 0;
                    keyCooldown.update();
                }
                if (ifPress(SDL_SCANCODE_LEFT) and alreadyNextStep == 1) {
                    if (nowDirection != RIGHT) nowDirection = LEFT;
                    alreadyNextStep = 0;
                    keyCooldown.update();
                }
                if (ifPress(SDL_SCANCODE_RIGHT) and alreadyNextStep == 1) {
                    if (nowDirection != LEFT) nowDirection = RIGHT;
                    alreadyNextStep = 0;
                    keyCooldown.update();
                }
            }
            else {
                if (ifPress(SDL_SCANCODE_Z)) quit = 1;
            }
        }
        
        // check wheather the mouse click the CROSS of the window
        while (SDL_PollEvent(&event) > 0) {
			switch (event.type)
			{
				case SDL_QUIT:
					quit = true;
					break;
				default:
					break;
			}
		}
    }
    closeSDL();
    return 0;
}

void initSnake() {
    // preparation for making cherry
    srand(time(0));

    // reset map infomation
	memset(mapto,0,sizeof(mapto));
	memset(mapbool,0,sizeof(mapbool));
	mapbool[6][7] = TAIL;
	mapbool[6][8] = SNAKE;
	mapbool[6][9] = HEAD;
	mapto[6][7] = RIGHT;
	mapto[6][8] = RIGHT;
    mapto[6][9] = RIGHT;
    mapfrom[6][7] = LEFT;
    mapfrom[6][8] = LEFT;
    mapfrom[6][9] = LEFT;
    makeCherry();
}

void paintMap() {
    for (int i = 0; i < 16; i ++) {
        for (int j = 0; j < 12; j ++) {
            // 36 and 18 is for halflength and halfwidth, 21 is for background.
            Box onRenderer = {{18+j*36+10,18+i*36+10},18,18};

            putImage(imageList[MAPIMAGE],onRenderer);
            switch (mapbool[i][j])
            {
                case SNAKE:
                    putImage(imageList[BODYIMAGE],onRenderer,mapfrom[i][j]);
                    putImage(imageList[BODYIMAGE],onRenderer,mapto[i][j]);
                    break;
                case HEAD:
                    putImage(imageList[HEADIMAGE],onRenderer,mapto[i][j]);
                    break;
                case CHERRY:
                    putImage(imageList[CHERRYIMAGE],onRenderer,mapto[i][j]);
                    break;
                case TAIL:
                    putImage(imageList[BODYIMAGE],onRenderer,mapto[i][j]);
                default:
                    break;
            }
        }
    }
}
int getrand(int min, int max) {
	return ( rand() % (max - min + 1) ) + min ;
}
void makeCherry(){
    int cherryx = getrand(0,15),cherryy = getrand(0,11);
	while(mapbool[cherryx][cherryy] != 0) {
		cherryx = getrand(0,15);
		cherryy = getrand(0,11);
	}
    mapto[cherryx][cherryy] = UP;
    mapbool[cherryx][cherryy] = CHERRY;
}
void eating() {
    snakeLength ++;
    /**
     * here will be some additional thing:
     * 1. the cherry will have "direction", eating it from different way will get different score and effect.
     * 2. and so on.
    */
}
void printOutAll() {
    // paint bg
    // what a abstruct background!!
    /**
     * background infomation:
     * drawed by qytlix
     * date:2024-05-27
     * the sourse file is in the same folder ( bg.kra ), you can use Krita open it.
    */
    putImage(imageList[BACKGROUNDIMAGE]);

    // print map
    paintMap();

    // Print length and score.
    
    // putImage(...);
    putNumber(snakeLength, {500,150});
}