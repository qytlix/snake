#include <iostream>
#include <time.h>
#include <string.h>
#include <fstream>
#include <termios.h>
#include <unistd.h>

#define TICKTIME 15
//define page name
#define P_TITLE 0
#define P_SETTING 1
//difficulty
#define RUNTICKTIME 110
//face
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
//colors
#define BACKCLR 8
#define BODYCLR 15
#define CHERRYCLR 12
#define APPLECLR 4

using namespace std;
void gotoxy(int,int);
void run(void);
void difficulty(void);
void choosemap(void);
void color(int,int);
void putlist(string *,int,int,int=7,int=0,int=0,int=7,int=15,int=0);
void init_keyboard(void);
void close_keyboard(void);
void makecherry(void);
int kbhit(void);
int readch(void); 
int getrand(int,int);

int mapbool[16][12],mapto[16][12];
int rate;
const int menu = 30;

bool quit;
string title[10]={"SNAKE  "," Start Game"," Settings  "," Rating    "," Quit      "};
string setting[10]={"SETTING"," Difficulty"," Map       "," Back      "};
int page = P_TITLE;
int number[10] = {4, 3};
int mode = 0;

static struct termios initial_settings, new_settings;
static int peek_character = -1;

void init_keyboard()
{
    tcgetattr(0,&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag |= ICANON;
    new_settings.c_lflag |= ECHO;
    new_settings.c_lflag |= ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}
 
void close_keyboard()
{
    tcsetattr(0, TCSANOW, &initial_settings);
}
 
int kbhit()
{
    unsigned char ch;
    int nread;
 
    if (peek_character != -1) return 1;
    new_settings.c_cc[VMIN]=0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0,&ch,1);
    new_settings.c_cc[VMIN]=1;
    tcsetattr(0, TCSANOW, &new_settings);
    if(nread == 1) 
    {
        peek_character = ch;
        return 1;
    }
    return 0;
}
 
int readch()
{
    char ch;
 
    if(peek_character != -1) 
    {
        ch = peek_character;
        peek_character = -1;
        return ch;
    }
    read(0,&ch,1);
    return ch;
}

void putlist(string *list, int cle, int end, /*û��ѡ��*/int nfore, int nback, /*ѡ��*/int yfore, int yback, /*����*/int tfore, int tback) {
	gotoxy(0,0);
	color(tfore, tback);
	printf(" %s\n", &list[0][0]);
	color(nfore, nback);
	for(int i = 1; i < cle; i ++)
	{
		printf(" %s\n", &list[i][0]);
	}
	color(yfore, yback);
	printf(">%s\n", &list[cle][0]);
	color(nfore, nback);
	for(int i = cle + 1; i < end; i ++)
	{
		printf(" %s\n", &list[i][0]);
	}
}
void color(int fore, int back) {
	back %= 8;
	back += 40;
	fore %= 8;
	fore += 30;
	string console;
	console = "\033["+to_string(back)+";"+to_string(fore)+"m";
	cout<<console.c_str();
}
void gotoxy(int x, int y) {
	string console;
	console = "\033["+to_string(x)+";"+to_string(y)+"H";
	cout<<console.c_str();
}
struct points {
	int x,y;
	void move(int to) {
		switch (to) {
			case UP:
				x = (x + 15) % 16;
				break;
			case DOWN:
				x = (x + 1) % 16;
				break;
			case LEFT:
				y = (y + 11) % 12;
				break;
			case RIGHT:
				y = (y + 1) % 12;
				break;
			default:
				break;
		}
	}
	void body() {
		mapbool[x][y]=2;
		gotoxy(y*2,x);
		color(0,BODYCLR);
		printf("  ");
	}
	void clear() {
		mapbool[x][y]=0;
		gotoxy(y*2,x);
		color(0,BACKCLR);
		printf("  ");
	}
};
int getrand(int min, int max)
{
	return ( rand() % (max - min + 1) ) + min ;
}
void makecherry()
{
	int cherryx = getrand(0,15),cherryy = getrand(0,11);
	while(mapbool[cherryx][cherryy] != 0) {
		cherryx = getrand(0,15);
		cherryy = getrand(0,11);
	}
	mapbool[cherryx][cherryy]=-1;
	gotoxy(cherryy*2,cherryx);
	color(0,CHERRYCLR);
	printf("  ");

	
	color(15,0);
	gotoxy(menu,2);
	printf("Cherry: %d %d  ",cherryx,cherryy);
	gotoxy(menu,3);
	printf("Your Points: %d  ", rate);
}
void run()
{
	srand(time(0));
	memset(mapto,0,sizeof(mapto));
	memset(mapbool,0,sizeof(mapbool));
	mapbool[6][7] = 2;
	mapbool[6][8] = 2;
	mapbool[6][9] = 2;
	mapto[6][7] = RIGHT;
	mapto[6][8] = RIGHT;

	int moveto = RIGHT;
	points head = {6,9}, tail = {6,7};

	int clk = 0, now;
	int kb = 0;

	for (int i = 0; i < 16; i ++) {
		for (int j = 0; j < 12; j ++) {
			switch (mapbool[i][j]) {
				case 2:
					color(0,BODYCLR);
					cout<<"  ";
					break;
				default:
					color(0,BACKCLR);
					cout<<"  ";
					break;
			}
		}
		cout<<endl;
	}
	makecherry();
	bool runflag = 1;
	while (1) {
		if(kbhit()) {
			kb = readch();
			if (kb == 224)kb = readch();
		}
		now = clock();
		if (now - RUNTICKTIME > clk) {
			//show fps
			gotoxy(menu,1);
			color(15,0);
			printf("FPS: %.2lf  ",1000.0/(double)(now-clk));

			clk = now;
			switch (kb) {
				case 72://up
					if(moveto != DOWN and runflag)moveto = UP;
					break;
				case 80://down
					if(moveto != UP and runflag)moveto = DOWN;
					break;
				case 75://le
					if(moveto != RIGHT and runflag)moveto = LEFT;
					break;
				case 77://ri
					if(moveto != LEFT and runflag)moveto = RIGHT;
					break;
				case 'z':
					runflag = !runflag;
					break;
				case 'x':
					return;
				default:
					break;
			}
			kb = 0;
			if (runflag){
				points _head = {head.x, head.y};
				_head.move(moveto);
				//check wall
				if (mapbool[_head.x][_head.y] > 0)
				{
					color(15,0);
					gotoxy(menu,3);
					printf("Your Points: %d", rate);
					//memorize rate
					fstream f;
					f.open("rates",ios::out|ios::app);
					f << rate << '\n';
					f.close();
					//-------------
					gotoxy(menu,5);
					printf("GAMEOVER!", rate);
					gotoxy(menu,6);
					printf("press X to back", rate);
					while(readch()!='x');
					return;
				}
				if (mapbool[_head.x][_head.y] == -1) {
					rate++;
					mapbool[_head.x][_head.y] = 0;
					makecherry();
	
					mapto[head.x][head.y] = moveto;
					head.move(moveto);
					head.body();
				} else {
					mapto[head.x][head.y] = moveto;
					head.move(moveto);
					head.body();
					tail.clear();
					tail.move(mapto[tail.x][tail.y]);
				}
			}
			
		}
	}

}
void difficulty(){
	cout<<"dif";
}
void choosemap(){
	cout<<"map";
}
void rt() {
	cout << "rts\n";
	string getin;
	fstream f;
	f.open("rates",ios::in);
	while(f>>getin)cout<<getin<<endl;
	f.close();
}
int main(){
	// init_keyboard();
	// printf("\033[25i");
	
	int kb = 0;
	int clk = 0, now;
	string *a = title;
	
	while(!quit)
	{
		if(kbhit())
		{
    		kb = readch();
    		if (kb == 224)kb = readch();
		}
		usleep(100);
		now = clock();
		if (now - TICKTIME > clk)
		{
			clk = now;
			switch (kb)
			{
				case 'z':
					switch (page)
					{
						case P_TITLE:
							switch (mode)
							{
								case 0:
									rate = 0;
									system("cls"); 
									color(0,15);
									gotoxy(menu,0);
									printf("SNAKE");
									gotoxy(0,0);
									run();
									system("cls"); 
									break;
								case 1:
									mode = 0;
									page = P_SETTING;
									a = setting;
									system("cls");
									break;
								case 2:
									rt();
									break;
								case 3:
									quit = 1;
									break;
								default:
									break;
							}
							break;
						case P_SETTING:
							switch (mode)
							{
								case 0:
									cout<<"dif "<<RUNTICKTIME;
									break;
								case 1:
									cout<<"map    ";
									break;
								case 2:
									mode = 0;
									page = P_TITLE;
									a = title;
									break;
								default:
									break;
							}
							break;
						default:
							break;
					}
						
					break;
				//�ƶ���� 
				case 72:
					mode = (mode + number[page] - 1) % number[page];
					break;
				case 80:
					mode = (mode + 1) % number[page];
					break;
				default:
					break;
			}
			kb = 0;
			putlist(a,mode+1,number[page]+1); 
		}
	}
	close_keyboard();
	return 0;
}
