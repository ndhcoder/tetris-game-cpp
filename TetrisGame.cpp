// TetrisGame.cpp : Defines the entry point for the console application.
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include<string.h>
#define backgr ' '
#define LEFT 30
#define heightBoard 21
#define widthBoard 10
#define f1 15
#define f2 'x'


void clrscr()
{
	CONSOLE_SCREEN_BUFFER_INFO	csbiInfo;                  
	HANDLE	hConsoleOut;
	COORD	Home = {0,0};
	DWORD	dummy;

	hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsoleOut,&csbiInfo);

	FillConsoleOutputCharacter(hConsoleOut,' ',csbiInfo.dwSize.X * csbiInfo.dwSize.Y,Home,&dummy);
	csbiInfo.dwCursorPosition.X = 0;
	csbiInfo.dwCursorPosition.Y = 0;
	SetConsoleCursorPosition(hConsoleOut,csbiInfo.dwCursorPosition);
}

//screen: goto [x,y]
void gotoXY (int column, int line)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}


void TextColor (int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE) , color);
}

int _eat[20], num = 0 , id , flag = 0,id_2,pause = 0;
int t1, t2, check = 0;
int Board[heightBoard][LEFT + widthBoard + 1];
int time1, time2, check2 = 0,time1_2 , time2_2,check2_2,numBrick,color,changeColor = 0;
	
// dinh nghia 1 diem 
class MARK{
	public:
	int x;
	int y;
};
//class Play quan ly cap do cua tro choi
class Play{
	public:
	long deley;
	long score;
	long overScore;
	int at;
	Play(){
		deley = 500;
		score = 0;
		overScore = 1000;
		at = 1;
	}
	void restart(){
		deley = 500;
		score = 0;
		overScore = 1000;
		at = 1;
		numBrick = 1;
		flag = 0;
	}

};

Play level;


//class BRICK : quan li khoi gach
class BRICK{
	public:
	int cl;
	int arr[4][4];
	int row, col;
	MARK board;
	
	void setBrick(){
	for (int i = 0; i < row; i++)
	for (int j = 0; j < col; j++)
		arr[i][j] = 1;
	}

	void CREATE(int id,int vt){
		//id = 0 la cot thang dung 4x1
		//id = 1 la hinh vuong 2x2
		//id = 2 la hinh xe tang 2x3
		//id = 3 la hinh chu L nguoc
		//id = 4 la hinh chu L xuoi
		//id = 5 la hinh chu Z xuoi
		//id = 6 la hinh chu Z nguoc
		switch (id){
		case 0:
			row = 4;
			col = 1;
			setBrick();
			break;
		case 1:
			row = col = 2;
			setBrick();
			break;
		case 2:
			row = 2;
			col = 3;
			setBrick();
			arr[0][0] = arr[0][2] = 0;
			break;
		case 3:
			row = 3;
			col = 2;
			setBrick();
			arr[0][0] = arr[1][0] = 0;
			break;
		case 4:
			row = 3;
			col = 2;
			setBrick();
			arr[0][1] = arr[1][1] = 0;
			break;
		case 5:
			row = 3;
			col = 2;
			setBrick();
			arr[2][1] = arr[0][0] = 0;
			break;
		case 6:
			row = 3;
			col = 2;
			setBrick();
			arr[0][1] = arr[2][0] = 0;
			break;
		}
	
		this->board.x = vt;
		this->board.y = -1;
		cl = 1 + rand() % 15;
		
	}
	void rotate(){
		BRICK b;
		b.row = this->col;
		b.col = this->row;
		for (int i = this->row - 1; i >= 0; i--){
			for (int j = this->col - 1; j >= 0; j--)
				b.arr[j][this->row - i - 1] = this->arr[i][j];
		}
		for (int i = 0; i < b.row; i++){
			int x = this->board.y + i;
			for (int j = 0; j < b.col; j++){
				int y = this->board.x + j;
				if (Board[x][y] != 0 && x >= 0 && b.arr[i][j] == 1) return;
			}
		}
		this->row = b.row;
		this->col = b.col;
		for (int i = 0; i < this->row; i++)
		for (int j = 0; j < this->col; j++)
			this->arr[i][j] = b.arr[i][j];
	}
	int hasLEFT(){
		int d;
		for (int i = 0; i < this->row; i++){
			int x = this->board.y + i;
			for (int j = 0; j < this->col; j++)
			if (this->arr[i][j] == 1) {
				d = j;
				break;
			}
			if (Board[x][this->board.x + d - 1] != 0  && this->board.y >= 0) return 0;
			if(Board[x+2][this->board.x + d - 1] == -1  ) return 0;
		}
		return 1;
	}
	
	int hasRIGHT(){
		for (int i = 0; i < this->row; i++){
			int x = this->board.y + i;
			for (int j = 0; j < this->col; j++){
				int y = this->board.x + j;
				if (this->arr[i][j] == 1){
					if ( (Board[x][y + 1] < 0 || Board[x][y+1] > 16) && this->board.y >= 0) return 0;
					if ( Board[x][y+1] != 0 && Board[x+1][y + 1] == -1  ) return 0;
					
					
				}
			}
		}
		return 1;
	}
	int hasDOWN(){
		for (int i = 0; i < this->row; i++){
			int x = this->board.y + i;
			for (int j = 0; j < this->col; j++){
				int y = this->board.x + j;
				if (this->arr[i][j] == 1){
					if (Board[x + 1][y] > 16  && x <= 0)  return 2;
					if ((x+1 == heightBoard  || Board[x + 1][y] > 16) && x >= 0) return 0;
				}
			}
		}
		return 1;
	}
};

BRICK brick;
BRICK brick2;

class  BoardGame{
		public:
		//khoi tao 
		void CREATE(int x = 0){
			for (int i = 1; i < heightBoard ; i++){
			for (int j = LEFT + 1; j <= LEFT + widthBoard; j++)
				Board[i][j] = 0;
				if(x ==0) {
					Board[i][LEFT] = -1;
					Board[i][LEFT + widthBoard + 1] = -1;
				}
			}
			if(x==0){
				for (int j = LEFT ; j <= LEFT + widthBoard; j++){
					Board[0][j]  =  -1;
					Board[heightBoard][j] = -1;
				}
			}
		}
		//xu li an diem
		void EAT(){
			TextColor(162);
			//to mau nhung hang an
			for (int i = 0; i < num; i++){
				int k = _eat[i];
				level.score += 100;
				for (int j = LEFT + 1; j <= LEFT + widthBoard; j++){
					gotoXY(j, k);
					printf("%c", f1);
				}
			}
			num = 0;
			//xu li khi an
			for (int i = _eat[0]; i > 1; i--){
				for (int j = LEFT + 1; j <= LEFT + widthBoard; j++){
					Board[i][j] = Board[i - 1][j];
				}
			}
			//xu li sau khi an
			for (int i = _eat[0]; i > 1; i--){
				for (int j = LEFT + 1 ; j <= LEFT + widthBoard ; j++){
					if(Board[i][j] > 16) {
						if(Board[i][j-1] == 0 && Board[i][j+1] == 0 && Board[i+1][j] == 0  ){
							int x = i;
							while(Board[++x][j] == 0){
								Board[x][j] = Board[x-1][j] ;
								Board[x-1][j]= 0;
							};
						}
					}
				}
			}
			TextColor(color);
		}
		//cap nhat lai Board
		void Update(){
			if(numBrick == 2 ) {
				for (int i = 0; i < brick2.row; i++){
					for (int j = 0; j < brick2.col; j++){
						int x = brick2.board.y + i;
						int y = brick2.board.x + j;
						if (x > 0 && x < heightBoard){
							if (brick2.arr[i][j] == 1){ 
								if (time2_2 - time1_2 <= level.deley) Board[x][y] = brick2.cl;
								else Board[x][y] = brick2.cl + 16;
							}
						}
					}
				}
			}
			
			//clrscr();
			for (int i = 0; i < brick.row; i++){
				for (int j = 0; j < brick.col; j++){
					int x = brick.board.y + i;
					int y = brick.board.x + j;
					if (x > 0 && x < heightBoard){
						if (brick.arr[i][j] == 1){
							if (time2 - time1 <= level.deley) Board[x][y] = brick.cl;
							else Board[x][y] = brick.cl + 16;
						}
					}
				}
			}

		}
		//hien thi va xu li
		void show(){
			for (int i = 1; i < heightBoard ; i++){
				for (int j = LEFT + 1; j <= LEFT + widthBoard; j++){
						gotoXY(j, i);
						int cl = Board[i][j]%16;
						TextColor(cl);
						printf("%c",f1);
						if (Board[i][j] < 16 ){
							Board[i][j] = 0;
						}
				}
					
			}
		}

};

BoardGame bg;

//class Inter Face quan li giao dien game
class InterFace{
	public:
		void clear(int x , int y, int k,int p){
			//xoa bat dau tu toa do (x,y) sang phai k , xuong duoi p don vi
 			for(int  i = 0 ; i < k ; i++){
				for(int j = 0 ; j < p ; j++){
					gotoXY(x+i,y+ j);
					putchar(' ');
				}
			}
		} 
		
		void show(){
			TextColor(color);
			for (int i = 1; i <= 20; i++){
				
				gotoXY(LEFT - 2, i);
				printf("%d ", i);
				gotoXY(LEFT, i);
				printf("%c", 178);
				gotoXY(LEFT + widthBoard + 1, i);
				printf("%c", 178);
			}
			for (int i = LEFT + 1; i <= widthBoard + LEFT; i++){
				gotoXY(i, 21);
				putchar(206);
				gotoXY(i, 0);
				putchar(206);
			}
			gotoXY(LEFT, 0);
			putchar(3);
			gotoXY(LEFT, 21);
			putchar(6);
			gotoXY(LEFT + widthBoard + 1, 0);
			putchar(3);
			gotoXY(LEFT + widthBoard + 1, 21);
			putchar(6);
			
			//TIP
			gotoXY(LEFT -25 ,10);
			printf("%c<P>: [ON]/[OFF] ",16);
			gotoXY(LEFT - 19,11);
			printf("2 BRICK model");
			
		

			gotoXY(LEFT -25 ,13);
			printf("%c<Space> : Pause/Play",16);
			gotoXY(LEFT -25 ,14);
			printf("%c<ESC> : menu ");
			gotoXY(LEFT - 25,15);
			printf("%c<O> : Change Theme");
			if(changeColor == 1 && numBrick ==2){
				gotoXY(LEFT -25 ,17);
				printf("%cUsing A,S,D,W to ",16);
				gotoXY(LEFT -25 ,18);
				printf("move the left-brick");
			}
			gotoXY(LEFT + widthBoard + 8 ,17);
			printf("%cUsing %c,%c,%c,%c  to ",16,24,25,26,27);
			gotoXY(LEFT + widthBoard + 8 ,18);
			printf("move the right-brick");

		}
		void GameOver(){
			clear(LEFT - 2,3,6 + widthBoard,6);
			for(int i = 0 ; i < widthBoard + 10 ;i++){
				gotoXY(LEFT - 4 + i ,3);
				putchar(196);
				gotoXY(LEFT - 4 + i ,9);
				putchar(196);
			}
			
			
			
			
			TextColor(14);
			gotoXY(LEFT +1,4);
			printf("GAME OVER");
			gotoXY(LEFT -1 , 5);
			printf("Your Score : %d",level.score);
			gotoXY(LEFT + 1 , 6);
			printf("<1> Play Again");
			gotoXY(LEFT + 1 , 7);
			printf("<2> Goto Menu ");
			gotoXY(LEFT + 1 , 8);
			printf("<3> Exit");
		}
		
	/*	void pause(){
			this->GameOver();
			this->clear(LEFT -1  , 4 , 20 ,2);
			gotoXY(LEFT + 2, 4);
			printf("MENU");
			gotoXY(LEFT +1 , 4);
			printf("<ESC> Continue ");
			
		}*/
		void drawNext(int id,int x,int m){
			//khung
			TextColor(color);
			gotoXY(x-2,2);
			printf("_Next_");
			for(int i = 0 ; i < 4 ; i++){
				gotoXY(x-3,3 + i);
				putchar(16);
				gotoXY(x+4,3 + i);
				putchar(17);
			}
			for(int i = 0 ; i < 8 ; i++){
				gotoXY(x-3+i,7);
				putchar(196);
			}
			gotoXY(x-3,7);
			putchar(192);
			gotoXY(x+4,7);
			putchar(217);
			
			//item
			BRICK b;
			b.CREATE(id,LEFT + 7);
			if(m == 3){
				for(int i = 0 ; i < 4; i++){
					for(int j = 0 ; j < 4 ; j ++){
						gotoXY(x + j,3+ i);
						putchar(' ');
					}
				}
			}
			for(int i = 0 ; i < b.row ; i++){
				for(int j = 0 ; j < b.col ; j ++){
					gotoXY(x+ j,3 + i);
					if(b.arr[i][j]==1){
						putchar(f1);
					}
				}
			}
		}
		
		void showLevel(){
			TextColor(color);
			if(level.score >= level.overScore){
				level.at++;
				if(level.deley > 50 ) level.deley -= 50;
				level.overScore +=1000;
			}
			gotoXY(LEFT + widthBoard + 8 ,10);
			printf("%cLevel : %d",16,level.at);
			gotoXY(LEFT + widthBoard + 8 ,11);
			printf("%cScore : %ld / %ld ",16,level.score,level.overScore);
			gotoXY(LEFT + widthBoard + 8 ,12);
			printf("%cSpeed: %ld (ms) ",16,level.deley);
		}
		
		char showMenu(){
		int lf = 0;
		int f = 0;
		while(1){
			int mau = rand()%16;
			TextColor(mau);
			Sleep(150);
			
			
			gotoXY(lf,3);	                                                              
			printf("	_|_|_|_|_|  _|_|_|_|  _|_|_|_|_|  _|_|_|    _|_|_|    _|_|_| ");
			gotoXY(lf,4);
			printf("	    _|      _|            _|      _|    _|    _|    _|       "); 
			gotoXY(lf,5);
			printf("	    _|      _|_|_|        _|      _|_|_|      _|      _|_|    ");
			gotoXY(lf,6);
			printf("	    _|      _|            _|      _|    _|    _|          _|  ");
			gotoXY(lf,7);
			printf("	    _|      _|_|_|_|      _|      _|    _|  _|_|_|  _|_|_|");
			
			
			char s[] = "Code by Hieu Nguyen - FB : http://facebook.com/billhieu0612 ";
			if(f == 0 ){
				int sp;
				if(pause == 0) sp = 50;
				else sp = 0; 
				TextColor(6);
				for(int i = 0 ; i < strlen(s) ; i++ ){
					Sleep(sp);
					gotoXY(lf +10 + i,10);
					putchar(s[i]);
				}
				f = 1;
			}
				TextColor(10);
				if(pause == 1) {
					gotoXY(30,13);
					printf("0.Continue");
				}
				gotoXY(30,15);
				printf("1.New Game");
				gotoXY(30,17);
				printf("2.Exit");
				
				if(kbhit()){
					char key = _getch();
					if(key == '1' || key == '2' ) {
						clrscr();
						return key;
					}
			
				}
				if(pause == 1) return 'c';

			}
		}
};

InterFace ITF;
		
// class Game : xu li tinh huong
class Game{
	public:
		//ham xu li tuong tac nguoi dung
		int kbhit(){
			TextColor(color);
			if (_kbhit()){
				char key = _getch();
				if (key == 75){
					if (brick.hasLEFT()){
						brick.board.x--;
					}
				}
				else if (key == 77){
					if (brick.hasRIGHT()){
						brick.board.x++;
					}
				}
				else if (key == 72)
					brick.rotate();
				else if (key == 80){
					if (brick.hasDOWN()){
						while (brick.hasDOWN()) {
							level.score++;
							brick.board.y++;
						}

					}
				}
				else if(key == 'p' || key == 'P'){
					if(flag %2 ==0){
						numBrick = 2;
						id_2 = rand()%7;
						brick2.CREATE(id_2,LEFT + 2);

						gotoXY(LEFT -25 ,17);
						printf("%cUsing A,S,D,W to ",16);
						gotoXY(LEFT -25 ,18);
						printf("move the left-brick");

					}
					else {
						numBrick = 1;
						ITF.clear(LEFT - 14 ,2,8,6);
						ITF.clear(LEFT-25,17,19,2);
					}
					flag++;
				}
				else if(key == 'o' || key == 'O'){
						color = 1 +  rand()%15;
						changeColor = 1;
				}
				else if(key == 27){
					clrscr();
					pause = 1;
					ITF.showMenu();
					return 1;
				}
				else if(key == 32) {
					gotoXY(LEFT+3,5);
					printf("PAUSE");
					while(1){
						char k = _getch();
						if(k == 32) {
							ITF.clear(LEFT+3,5,5,0);
							break;
						}
					};
				}
		
				if(numBrick ==2 ) {
					if (key == 'a' || key=='A'){
						if (brick2.hasLEFT()){
							brick2.board.x--;
						}
					}
					else if (key == 'd' || key == 'D' ){
						if (brick2.hasRIGHT()){
							brick2.board.x++;
						}
					}
					else if (key == 'w' || key == 'W')
						brick2.rotate();
					else if (key == 's' || key == 'S'){
						if (brick2.hasDOWN()){
							while (brick2.hasDOWN()) {
								level.score++;
								brick2.board.y++;
							}
							
	
						}
					}
					
				}
			}
			return 0;

		}
		//ham xu li khi  toc do
		int Control(){
			int x = this->kbhit();
			if(x == 1) return 1;
			if (check == 0){
				t1 = clock();
				t2 = clock();
				check = 1;
			}
			else{
				t2 = clock();
				if (t2 - t1 >level.deley){
					check = 0;
					if (brick.hasDOWN()) {
						brick.board.y++;
					}
					if(numBrick == 2 && brick2.hasDOWN()){
						brick2.board.y++;	
					}
				}
			}
			return 0;
		}
		
		//ham xu li gameover , an diem ...
		int Process(){
			//kiem tra gameover
			if (brick.hasDOWN() ==  2 || (brick2.hasDOWN() == 2 && numBrick == 2) ) return 2;
			//kiem tra an hay ko
			for (int i = 20; i > 0; i--){
				int eat = 0;
				for (int j = LEFT + 1; j <= LEFT + widthBoard; j++){
					if (Board[i][j] < 16 ) {
						eat = 1;
						break;
					}
				}
				if (eat == 0) _eat[num++] = i;
			}
			if (num > 0) bg.EAT();
			if(numBrick == 2){
				if (!brick2.hasDOWN()){
					if (check2_2 == 0){
						time1_2 = clock();
						time2_2 = clock();
						check2_2 = 1;
					}
					else{
						time2_2 = clock();
						bg.Update();
						if (time2_2 - time1_2 > level.deley){
							time1_2 = time2_2;
							brick2.CREATE(id_2,LEFT + 2);
							id_2 = rand()%7;
							check2_2 = 0;
							return 3;
	
						}
					}
				}
			}

			if (!brick.hasDOWN()){
				if (check2 == 0){
					time1 = clock();
					time2 = clock();
					check2 = 1;
				}
				else{
					time2 = clock();
					bg.Update();
					if (time2 - time1 > level.deley){
						time1 = time2;
						brick.CREATE(id,LEFT + 7);
						id = rand()%7;
						check2 = 0;
					}
				}
				return 3;
			}
			return 0;
		}
			
};

int main()
{
	bg.CREATE();
	color = 7;
	Game game;
	srand(time(NULL));
	id = rand() % 7;
	brick.CREATE(id,LEFT + 7);
	id = rand() % 7;
	menu:
	
	char k = ITF.showMenu();
	if(k == '2') return 0;
	start:
	clrscr();
	ITF.show();
	int m = 0,n;
	while (1){
		if(changeColor == 1){
			TextColor(color);
			ITF.show();
			changeColor = 0;
		}
		bg.Update();
		bg.show();
		game.Control();
		if(pause == 1){
			while(1){
				char key = _getch();
				if(key == '0')  {
					clrscr();
					ITF.show();
					pause = 0;
					break;
				}
				else if(key == '1'){
					pause =0;
					bg.CREATE(1);
					level.restart();
					id = rand()%7;
					brick.CREATE(id,LEFT + 7);
					goto start;
				}
				else if(key == '2') return 0;
			};
		}
		m = game.Process();
		if (m == 2 ){
			ITF.GameOver();
			while(1){
				char key=getch();
				if(key == '1') {
					bg.CREATE(1);
					level.restart();
					goto start;
				}
				if(key == '2') {
					clrscr();
					bg.CREATE(1);
					level.restart();
					goto menu;
				}
				if(key == '3') return 0;
			
			}
		}
		ITF.drawNext(id,LEFT + widthBoard + 11,m);
		if(numBrick == 2) ITF.drawNext(id_2,LEFT - 11,m);
		ITF.showLevel();
	};
}


