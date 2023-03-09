#define _CRT_SECURE_NO_WARNINGS
//#define _USE_MATH_DEFINES
#include <stdio.h>
//#include <stdlib.h>
//#include <conio.h>
#include <windows.h>
#include <time.h>
//#include <math.h>
#include <string.h>



// Global parametrs 
#define Height 29   //40   24  29 28
#define Width 119  // 164   80  119 120

#define MapLength 160 * 1



// Objects
typedef struct Player {
	int x;
	int y;

	int w;
	int h;
} GenPlayer;



// Initialization
GenPlayer Player;
GenPlayer PrevPlayerPos;

unsigned char map[Height][Width + MapLength + 1];
unsigned char lastMap[Height][Width + MapLength + 1];

BOOL PlayerJump = FALSE;
BOOL BlockDown = FALSE;
BOOL BlockMove = FALSE;

int JumpCount = 0;
int Count = 0;
int KeyCount = 0;
int lvl = 0;
int MoveCount = 0;
int SnowTimer = 0;
int SleepCount = 28;  // For 60 Hz monitor    14/28 

BOOL GameEnd = FALSE;
BOOL HouseIsGenerated = FALSE;

int FloorSymbol = 219;
int UnderFloorSymbol_1 = 177;
int UnderFloorSymbol_2 = 176;
int UnderFloorSymbol_3 = 177;
int PlatformWidth = 5;

int PlayerSymbol = 178;

int AmoutOfSnow = 5;
BOOL IsFallingSnow = FALSE;



// Functions
int GenMap(int Pos) {

	for (int i = 0; i < Height; i++) {     // Making alll of FloorSymbols
		for (int k = 0 + Pos; k < Width + MapLength; k++)
			if (map[i][k] != '*') map[i][k] = FloorSymbol;

	}

	for (int i = 0; i < Height - 5; i++)   // Making Clear area
		for (int k = 0 + Pos; k < Width + MapLength; k++)
			if (map[i][k] != '*') map[i][k] = ' ';

	for (int i = Height - 4; i < Height; i++)  // Making some "texture" under floor
		for (int k = 0 + Pos; k < Width + MapLength; k++) {
			if (rand() % 33) map[i][k] = UnderFloorSymbol_3;
			if (rand() % 2) map[i][k] = UnderFloorSymbol_2;
			else map[i][k] = UnderFloorSymbol_1;
		}

	for (int i = Height - 5; i < Height; i++)  // Making HOLE
		for (int k = 25 + Pos; k < 30; k++)
			map[i][k] = ' ';

	GenRandomPlatfroms(Pos);

	for (int i = 0; i < Height; i++)
		if (Pos == 0) map[i][Width] = '\0';


	for (int i = 0; i < Height; i++)            // Coping map into the buffer
		for (int k = 0 + Pos; k < Width + MapLength; k++) {
			lastMap[i][k] = map[i][k];
			if (lastMap[i][k] == '*' || lastMap[i][k] == '_') lastMap[i][k] = ' ';
		}

}

int ShowMap() {

	for (int i = 0; i < Height; i++)    // Showing a map
		printf("%s\n", map[i]);
}

int ClearSC(int x, int y) {
	COORD coord;

	coord.X = x;
	coord.Y = y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

}

int GenRandomPlatfroms(int Pos) {
	for (int i = 0 + Pos; i < Width + MapLength; i++) {             // Generates random pixel platform
		int Rand = Height - 6 - (rand() % 5);
		if (rand() % 2) {
			map[Rand][i] = FloorSymbol;
			i += 20;
		}

	}

	for (int i = Height - 10; i < Height - 5; i++)     // Makes NORMAL platform on found pixel Platform
		for (int k = 0 + Pos; k < Width + MapLength; k++)
			if (map[i][k] == FloorSymbol) {
				MakePlatform(i, k);
				k += PlatformWidth;
			}
}

int MakePlatform(int y, int x) {
	RandomWidth();

	for (int i = x; i < x + PlatformWidth; i++)  // Makes TOP platform
		if (i < Width + MapLength) map[y][i] = FloorSymbol;



	for (int i = y + 1; i < Height; i++)   // Makes UNDER floor block
		for (int j = x; j < x + PlatformWidth; j++) {
			if (j < Width + MapLength) {
				if (rand() % 33) map[i][j] = UnderFloorSymbol_3;
				if (rand() % 2) map[i][j] = UnderFloorSymbol_2;
				else map[i][j] = UnderFloorSymbol_1;

			}
		}


}

int RandomWidth() {
	PlatformWidth = (rand() % 10);
	if (PlatformWidth < 3) RandomWidth();
}

int SlowMapMove() {

	for (int i = Player.y; (i > Player.y - Player.h) && !BlockMove; i--) {
		if (lastMap[i][Player.x + Player.w - 1] == ' ') {
			BlockMove = FALSE;
		}
		else BlockMove = TRUE;

	}

	if (!BlockMove) {
		if (Player.x > (Width - 70)) {

			Player.x--;

			for (int i = 0; i < Height; i++) {
				for (int k = 0; k < Width + MapLength; k++) {

					map[i][k] = map[i][k + 1];

					lastMap[i][k] = lastMap[i][k + 1];

				}

				map[i][Width - 1] = map[i][Width];
				map[i][Width] = '\0';

				lastMap[i][Width - 1] = lastMap[i][Width];
				lastMap[i][Width] = '\0';

			}
			MoveCount++;
			BlockMove = FALSE;
		}
	}

	if (MoveCount == 60) {
		GenMap(Width + 1);
		MoveCount = 0;
	}

}


int GenSnow() {
	for (int i = 0; i < Width - 1 + MapLength; i++) {
		if (rand() % 1000 - AmoutOfSnow < 1)
			if (map[0][i] == ' ') map[0][i] = '*';
	}
}

int MoveSnow() {
	int dx;

	for (int i = Height - 1; i >= 0; i--)
		for (int j = 0; j < Width - 1 + MapLength; j++) {
			if (map[i][j] == '*') {
				if (map[i + 1][j] != FloorSymbol) {
					if (rand() % 10 < 2) {
						map[i][j] = ' ';


						dx = 0;
						if (map[i][j - 1] != '*' && map[i][j + 1] != '*') {
							if (rand() % 10 < 1) dx++;
							if (rand() % 10 < 1) dx--;
						}

						dx += j;

						if ((dx >= 0) && (dx < Width) && (i + 1 < Height))
							if (map[i + 1][dx] == ' ') map[i + 1][dx] = '*';
					}
				}
				if (map[i + 1][j] == FloorSymbol && map[i][j] == '*') map[i][j] = '_';
			}
		}
}



int SetPlayerParametrs(int Y, int X, int height, int width) {

	Player.x = X;
	Player.y = Y;

	Player.h = height;
	Player.w = width;
}

int PutPlayer() {

	for (int i = Player.y; i > Player.y - Player.h; i--)          // Changes the player position
		for (int k = Player.x; k < Player.x + Player.w; k++)
			map[i][k] = PlayerSymbol;

	map[Player.y - Player.h + 1][Player.x] = lastMap[Player.y - Player.h + 1][Player.x];
	map[Player.y - Player.h + 1][Player.x + Player.w - 1] = lastMap[Player.y - Player.h + 1][Player.x + Player.w - 1];


	if (GetKeyState('D') < 0) {
		map[Player.y - Player.h + 1][Player.x] = lastMap[Player.y - Player.h + 1][Player.x];
		map[Player.y - Player.h + 1][Player.x + Player.w - 1] = PlayerSymbol;
		map[Player.y - Player.h + 3][Player.x + Player.w - 1] = lastMap[Player.y - Player.h + 3][Player.x + Player.w - 1];
	}
	if (GetKeyState('A') < 0) {
		map[Player.y - Player.h + 1][Player.x + Player.w - 1] = lastMap[Player.y - Player.h + 1][Player.x + Player.w - 1];
		map[Player.y - Player.h + 1][Player.x] = PlayerSymbol;
		map[Player.y - Player.h + 3][Player.x] = lastMap[Player.y - Player.h + 3][Player.x];
	}
}



int Control() {
	PrevPlayerPos.x = Player.x;
	PrevPlayerPos.y = Player.y;

	if (GetKeyState('A') < 0) {                    // Moving to the left
		Player.x--;

		for (int i = Player.y; i > Player.y - Player.h; i--)
			map[i][Player.x + Player.w] = lastMap[i][Player.x + Player.w];
	}
	if (GetKeyState('D') < 0) {                    // Moving to the right
		if (Player.x < (Width - 69)) {
			Player.x++;
			for (int i = Player.y; i > Player.y - Player.h; i--)
				map[i][Player.x - 1] = lastMap[i][Player.x - 1];
		}
		SlowMapMove();
	}
	if (GetKeyState('S') < 0) {                    // Moving to the down
		Player.y++;

		for (int i = Player.x; i < Player.x + Player.w; i++)
			map[Player.y - Player.h][i] = lastMap[Player.y - Player.h][i];

	}


	if (GetKeyState(' ') < 0 && !BlockDown) PlayerJump = TRUE;
	Jump();


	if ((GetKeyState('K') < 0) && IsFallingSnow && KeyCount > 20) {
		KeyCount = 0;
		if (IsFallingSnow) {
			IsFallingSnow = FALSE;

			for (int i = 0; i < Height; i++)
				for (int k = 0; k < Width - 1; k++)
					if (map[i][k] == '*') map[i][k] = lastMap[i][k];
			KeyCount = 0;
		}

	}
	if ((GetKeyState('K') < 0) && !IsFallingSnow && KeyCount > 20) {
		KeyCount = 0;
		if (!IsFallingSnow) {
			IsFallingSnow = TRUE;
			KeyCount = 0;
		}
	}

	if ((GetKeyState(VK_OEM_PLUS) < 0)) SleepCount++;
	if ((GetKeyState(VK_OEM_MINUS) < 0) && SleepCount > 0) SleepCount--;

}

int BoundingControl() {

	for (int l = Player.y; l > Player.y - Player.h; l--)  // Left side control 
		if (lastMap[l][Player.x] == FloorSymbol || lastMap[l][Player.x] == UnderFloorSymbol_1 || lastMap[l][Player.x] == UnderFloorSymbol_2 || lastMap[l][Player.x] == UnderFloorSymbol_3) {
			Player.x = PrevPlayerPos.x;

			SlowMapMove();
		}

	for (int l = Player.y; l > Player.y - Player.h; l--)  // Rigth side control
		if (lastMap[l][Player.x + Player.w - 1] == FloorSymbol || lastMap[l][Player.x + Player.w - 1] == UnderFloorSymbol_1 || lastMap[l][Player.x + Player.w - 1] == UnderFloorSymbol_2 || lastMap[l][Player.x + Player.w - 1] == UnderFloorSymbol_3) {
			Player.x = PrevPlayerPos.x;

			SlowMapMove();
		}

	for (int l = Player.x; l < Player.x + Player.w; l++) // Down side control
		if (lastMap[Player.y][l] == FloorSymbol || lastMap[Player.y][l] == UnderFloorSymbol_1 || lastMap[Player.y][l] == UnderFloorSymbol_2 || lastMap[Player.y][l] == UnderFloorSymbol_3)
			Player.y = PrevPlayerPos.y;

	for (int l = Player.x; l < Player.x + Player.w; l++) // Up side control
		if (lastMap[Player.y - Player.h][l] == FloorSymbol || lastMap[Player.y - Player.h][l] == UnderFloorSymbol_1 || lastMap[Player.y - Player.h][l] == UnderFloorSymbol_2 || lastMap[Player.y - Player.h][l] == UnderFloorSymbol_3)
			Player.y = PrevPlayerPos.y;

	if (Player.x < 0 || Player.x + Player.w > Width - 1) Player.x = PrevPlayerPos.x; // Out of X border control
	if (Player.y < 1) Player.y = PrevPlayerPos.y; // Top border limit

}

int Jump() {
	if (PlayerJump) {
		if (JumpCount < 6) {
			Player.y--;
			JumpCount++;
		}
		if (JumpCount > 5) {
			Player.y++;
			JumpCount++;
		}
		if (JumpCount > 11) {
			JumpCount = 0;
			PlayerJump = FALSE;
		}


		BlockDown = TRUE;

	}
	else BlockDown = FALSE;

	if (!PlayerJump) GravityControl();

}

int ClearAround() {

	for (int i = Player.y + 3; i > Player.y - Player.h - 3; i--)
		for (int k = Player.x - 3; k < Player.x + Player.w + 3; k++)
			if (map[i][k] != '*' && map[i][k] != '_') map[i][k] = lastMap[i][k];
	PutPlayer();

}

int GravityControl() {
	BOOL IsPlatform = FALSE;

	for (int i = Player.x; i < Player.x + Player.w; i++)
		if (map[Player.y + 1][i] == FloorSymbol) IsPlatform = TRUE;

	if (!IsPlatform && !PlayerJump) 	Player.y++;

	BoundingControl();
}



// MapMove is not using anymore. If Player.x > Width - 10 | Generates new map. Not smooth
int MapMove() {

	if (Player.x > Width - 10) {

		GenMap(0);
		for (int i = PrevPlayerPos.y; i > 0; i--)
			if (lastMap[i + 1][1] == FloorSymbol) PrevPlayerPos.y = i;
		SetPlayerParametrs(PrevPlayerPos.y, 1, 4, 4);

		IsFallingSnow = TRUE;
		AmoutOfSnow += 1;

		for (int i = 0; i < Height - 7; i++) {
			if (IsFallingSnow) {
				GenSnow();
				MoveSnow();
			}
		}
		lvl++;
	}
}

int DrawObject() {
	if (GetKeyState('L') < 0 && Count > 10) {
		for (int i = Player.x; i < Player.x + Player.w; i++) {
			if (map[Player.y + 1][i] != ' ') {
				map[Player.y + 1][i] = ' ';
				lastMap[Player.y + 1][i] = ' ';

				if (Player.y < Height - 2) {
					map[Player.y + 2][i] = FloorSymbol;
					lastMap[Player.y + 2][i] = FloorSymbol;
				}
			}
		}
		Player.y++;
		BlockDown = TRUE;
		Count = 0;
	}

}

int GameOver() {
	if ((Player.y - Player.h) > Height) {
		GameEnd = TRUE;
		Sleep(100);
		system("cls");
		for (int i = 0; i < (Height / 2) - 1; i++)
			printf("\n");
		for (int i = 0; i < ((Width - 4) / 3); i++)
			printf("\t");
		printf("  GAME OVER");
		Sleep(5000);
	}
}
//GenHous temporary is not in use.  Generates a House after 3 MapMove generates
int GenHous() {
	if (lvl == 3) {
		GenMap(0);
		int Param = (Width / 2) + Player.w + 50;

		for (int i = Height - 6; i > Height - 11; i--)
			for (int k = (Width / 2) + Player.w; k < Width; k++) {
				map[i][k] = ' ';
				lastMap[i][k] = ' ';
				map[Height - 5][k] = FloorSymbol;
				lastMap[Height - 5][k] = FloorSymbol;
			}

		for (int i = (Height / 2) + 2; i < (Height / 2) + 8; i++)
			for (int k = (Width / 2) + Player.w + 10; k < Width - 1; k++) {
				map[i][k] = FloorSymbol;
				lastMap[i][k] = FloorSymbol;
			}
		for (int i = (Height / 2) + 2; i < (Height / 2) + 8; i++) {
			for (int k = (Width / 2) + Player.w + 10; k < Param - 25; k++) {
				map[i][k] = ' ';
				lastMap[i][k] = ' ';
				if (k == (Width / 2) + Player.w + 15)
					for (int j = i + 6; map[j][(Width / 2) + Player.w + 15] != FloorSymbol; j++) {
						for (int b = (Width / 2) + Player.w + 15; b < Width - 1; b += 15) {
							map[j][b] = 221;
							lastMap[j][b] = 221;

							map[j][b + 1] = 222;
							lastMap[j][b + 1] = 222;
						}
					}
			}
			Param -= 3;
		}
		lvl++;
		HouseIsGenerated = TRUE;
	}
}



int	main() {
	srand(time(NULL));

	GenMap(0);    // Creating the Map
	SetPlayerParametrs(Height - 10, (Width / 2) - 10, 4, 4);  // Player parametrs


	while (!GameEnd) {

		Count++;       // Timer for normal work of 'L' - DrawObject, that Draws/Destroys blocks
		KeyCount++;    // Timer for normal work of 'K' - snow activator
		BlockMove = FALSE;

		Control();             // Moving player by key, etc
		GravityControl();	   // If Jump is not activated moves player to the ground smoothly
		PutPlayer();           // Moves the player

		if (IsFallingSnow) {
			GenSnow();         // Generates snow in the first line
			MoveSnow();		   // Moves snow down with some percent of moving to the sides
		}
		//GenHous();
		//if (HouseIsGenerated)
		//	if (Player.x > (Width / 2) + Player.w + 40)
		//		Player.y = Height + Player.h + 1;
		//MapMove();
		SnowTimer++;
		if (SnowTimer == 256) IsFallingSnow = TRUE;

		ClearAround();         // Cleares the old Player sprite in radius of 2 symbols
		DrawObject();          // Destroys\Draws smth

		ClearSC(0, 0);				// Fixes bug when cursor  too fast change the position      
		printf(" Sleep %dms", SleepCount);

		ShowMap();             // Draws a picture	

		if (SnowTimer < 256) {
			ClearSC(0, 2);
			printf(" A - Left   D - Right");
			ClearSC(0, 4);
			printf(" +\\- change \"FPS\"  (14 / 28 optimal)");
			ClearSC(0, 6);
			printf(" K - Snow switch");
			ClearSC(0, 7);
			printf(" L - Dig under you");

		}

		ClearSC(0, 0);         // Moving Cursor to the (0, 0);

		GameOver();            // If player.y + Player.w > Height --- GAME OVER	
		Sleep(SleepCount);

	}
}