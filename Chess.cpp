#define OLC_PGE_APPLICATION
#include "include/olcPixelGameEngine.h"
#include <unistd.h>

#define WHITE_PIECE 0
#define BLACK_PIECE 1

#define PAWN 1
#define KNIGHT 3
#define BISHOP 4
#define ROOK 5
#define KING 8
#define QUEEN 9

#define BLACK_LONG 0
#define BLACK_SHORT 1
#define WHITE_LONG 2
#define WHITE_SHORT 3

class Chess : public olc::PixelGameEngine
{
public:
	Chess()
	{
		sAppName = "Chesseo";
	}

	olc::Sprite* sprPieces = nullptr;
	olc::Decal* decPieces = nullptr;

	int chessBoard[8][8] = // Initial board setup
	{{5,3,4,9,8,4,3,5},
	{1,1,1,1,1,1,1,1},	
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{11,11,11,11,11,11,11,11},
	{15,13,14,19,18,14,13,15}};

	olc::vf2d mouse_click = {0, 0};
	int lastPawnMovedTwice;
	bool isCastlePossible[4] = {true, true, true, true};

public:
	bool OnUserCreate() override
	{
		sprPieces = new olc::Sprite("./img/pieces.png");
		decPieces = new olc::Decal(sprPieces);
		//Decals are lighter to render and allow resizing
		DrawMap();

		lastPawnMovedTwice = -1; // This is a global variable used to check if en passant is possible.
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{


		DrawMap();	
		DrawPieces(chessBoard);

		if(GetMouse(0).bPressed)
			mouse_click = {float(GetMouseX()), float(GetMouseY())};

		if(GetMouse(0).bHeld || GetMouse(0).bReleased)
			MovePiece(mouse_click);

		return true;
	}

	void DrawMap(){
		for(int i = 0; i < 8; i++){
			for(int j = 0; j< 8; j++){
				if((i+j)%2)
					FillRect(i*32, j*32, 32, 32, olc::VERY_DARK_CYAN);
				else
					FillRect(i*32, j*32, 32, 32, olc::WHITE);
			}

		}
		/* Debug stuff */
		// DrawString(10, ScreenHeight()/2, "Last pawn moved twice: " + std::to_string(lastPawnMovedTwice), olc::RED);
		// DrawString(10, ScreenHeight()/2, std::to_string(isCastlePossible[BLACK_SHORT]) + std::to_string(isCastlePossible[BLACK_LONG]) + std::to_string(isCastlePossible[WHITE_SHORT]) + std::to_string(isCastlePossible[WHITE_LONG]) , olc::RED);
	}

	void DrawPiece(int piece, int row, int column, bool is_black){
		switch(piece){
			case KING:
				DrawPartialDecal({float(row)*32 - 2, float(column)*32 - 2}, decPieces, {200*0, 200*float(is_black)}, {200, 200}, {0.18f, 0.18f});
			break;

			case QUEEN:
				DrawPartialDecal({float(row)*32 - 2, float(column)*32 - 2}, decPieces, {200*1, 200*float(is_black)}, {200, 200}, {0.18f, 0.18f});
			break;

			case BISHOP:
				DrawPartialDecal({float(row)*32 - 2, float(column)*32 - 2}, decPieces, {200*2, 200*float(is_black)}, {200, 200}, {0.18f, 0.18f});
			break;

			case KNIGHT:
				DrawPartialDecal({float(row)*32 - 2, float(column)*32 - 2}, decPieces, {200*3, 200*float(is_black)}, {200, 200}, {0.18f, 0.18f});
			break;

			case ROOK:
				DrawPartialDecal({float(row)*32 - 2, float(column)*32 - 2}, decPieces, {200*4, 200*float(is_black)}, {200, 200}, {0.18f, 0.18f});
			break;

			case PAWN:
				DrawPartialDecal({float(row)*32 - 2, float(column)*32 - 2}, decPieces, {200*5, 200*float(is_black)}, {200, 200}, {0.18f, 0.18f});
			break;
		}
	}
		
	void DrawPieces(int chessBoard[8][8]){
		for(int i = 0; i < 8; i++){
			for(int j = 0; j<8; j++){
				if(chessBoard[i][j] < 10)
					DrawPiece(chessBoard[i][j], j, i, BLACK_PIECE);
				else
					DrawPiece(chessBoard[i][j] - 10, j, i, WHITE_PIECE);
			}
		}
	}
	bool isBlack(int piece){
		if(piece < 10 && piece != 0)
			return true;
		else
			return false;
	}

	bool isValidMove(int initialSquare[2], int finalSquare[2], int piece){
		bool is_black = isBlack(chessBoard[initialSquare[0]][initialSquare[1]]);
		int slope = -1;
		if(!is_black)
			piece = piece - 10;

		if(is_black == isBlack(chessBoard[finalSquare[0]][finalSquare[1]]) && chessBoard[finalSquare[0]][finalSquare[1]])	// Checks if final position has a piece of the same color
			return false;		


		switch(piece){
			case PAWN:
				if (is_black){	// Different cases for black and white
					if(!chessBoard[finalSquare[0]][finalSquare[1]]){	// Checks for no piece
						if((finalSquare[0] == initialSquare[0] + 1) && (finalSquare[1] == initialSquare[1])){ // Any move
							return true;
						}	
						if((finalSquare[0] == 3) && (initialSquare[0] == 1) && finalSquare[1] == initialSquare[1]){	// First move can jump 2
							return true;
						}
						if((finalSquare[0] == 5) && (initialSquare[0] == 4) && (finalSquare[1] == (lastPawnMovedTwice - 10)) && (abs(initialSquare[1] - finalSquare[1]) == 1)){ 	// En passant
							return true;
						}
					}
					else{	// Capture pieces (normal)
						if((finalSquare[0] - initialSquare[0] == 1) && (abs(finalSquare[1] - initialSquare[1]) == 1) && !isBlack(chessBoard[finalSquare[0]][finalSquare[1]])){
							return true;
						}
					}
				}
				else{
					if(!chessBoard[finalSquare[0]][finalSquare[1]]){	// Checks for no piece
						if((finalSquare[0] == initialSquare[0] - 1) && (finalSquare[1] == initialSquare[1])){	// Any move 
							return true;	
						}
						if((finalSquare[0] == 4) && (initialSquare[0] == 6) && finalSquare[1] == initialSquare[1]){	// First move can jump 2
							return true;
						}
						if((finalSquare[0] == 2) && (initialSquare[0] == 3) && (finalSquare[1] == lastPawnMovedTwice) && (abs(initialSquare[1] - finalSquare[1]) == 1)){	// En passant
							return true;
						}
					}
					else{	// Capture pieces (normal)
						if((finalSquare[0] - initialSquare[0] == -1) && (abs(finalSquare[1] - initialSquare[1]) == 1) && isBlack(chessBoard[finalSquare[0]][finalSquare[1]])){
							return true;
						}
					}		
				}
				return false;
			break;

			case KNIGHT:
				if((abs(finalSquare[0] - initialSquare[0])) == 2 && (abs(finalSquare[1] - initialSquare[1]) == 1)) // Checks L movement
					return true;
				if((abs(finalSquare[1] - initialSquare[1])) == 2 && (abs(finalSquare[0] - initialSquare[0]) == 1))
					return true;
			break;

			case BISHOP:
				if(abs(initialSquare[0] - finalSquare[0]) != abs(initialSquare[1] - finalSquare[1]))	// Checks diagonal movement
					return false;
				
				/* Check for colision in the diagonal */
				if(finalSquare[1] > initialSquare[1])	// To add/substract the x coordinate in each iteration
					slope = 1;
				else
					slope = -1;
				if(finalSquare[0] > initialSquare[0]){	// Two cases: move upwards or downwards
					int j = initialSquare[1] + slope;
					for(int i = initialSquare[0] + 1; i < finalSquare[0]; i++){	// Checks for colision from initial square + 1 to the last one - 1
						if(chessBoard[i][j] != 0)
							return false;
						j += slope;
					}
				}
				else{
					int j = initialSquare[1] + slope;
					for(int i = initialSquare[0] - 1; i > finalSquare[0]; i--){
						if(chessBoard[i][j] != 0)
							return false;
						j += slope;
					}
				}
				return true;	// If colision does not break, return true
			break;

			case ROOK:
				if(initialSquare[0] != finalSquare[0] && initialSquare[1] != finalSquare[1])	// Checks straight movement
					return false;

				/*Check for colision in the row */
				if(finalSquare[1] > initialSquare[1]){
					for(int i = initialSquare[1] + 1; i<finalSquare[1]; i++){
						if(chessBoard[initialSquare[0]][i] != 0)
							return false;
					}
				}
				else{ 
					for(int i = initialSquare[1] - 1; i>finalSquare[1]; i--){
						if(chessBoard[initialSquare[0]][i] != 0)
							return false;
					}
				}
				/*Check for colision in the column */
				if(finalSquare[0] > initialSquare[0]){
					for(int i = initialSquare[0] + 1; i<finalSquare[0]; i++){
						if(chessBoard[i][initialSquare[1]] != 0)
							return false;
					}
				}
				else{
					for(int i = initialSquare[0] - 1; i>finalSquare[0]; i--){
						if(chessBoard[i][initialSquare[1]] != 0)
							return false;
					}
				}
				return true;	// If no colision, returns true
			break;

			case KING:
				if((abs(finalSquare[1] - initialSquare[1]) <= 1) && (abs(finalSquare[0]-initialSquare[0]) <= 1))	// Normal moves
					return true;

				/* Castle */
				if(is_black){
					if(finalSquare[0] == 0 && finalSquare[1] == 2 && isCastlePossible[BLACK_LONG] && (chessBoard[0][1] + chessBoard[0][2] + chessBoard[0][3] == 0))
						return true;
					if(finalSquare[0] == 0 && finalSquare[1] ==  6 && isCastlePossible[BLACK_SHORT] && (chessBoard[0][6] + chessBoard[0][5] == 0))
						return true;
				}
				else{
					if(finalSquare[0] == 7 && finalSquare[1] == 2 && isCastlePossible[WHITE_LONG] && (chessBoard[7][1] + chessBoard[7][2] + chessBoard[7][3] == 0))
						return true;
					if(finalSquare[0] == 7 && finalSquare[1] == 6 && isCastlePossible[WHITE_SHORT] && (chessBoard[7][6] + chessBoard[7][5] == 0))
						return true;
				}
				return false;
			break;

			case QUEEN:
				if(isValidMove(initialSquare, finalSquare, BISHOP + (!is_black)*10))
					return true;
				else{ 
					if(isValidMove(initialSquare, finalSquare, ROOK + (!is_black)*10))
						return true;
				}
				return false;
			break;
		}
		return false;
	}
	void drawValidMoves(int piece, int init_square[2]){
		for(int i = 0; i < 8; i++){
			for(int j = 0; j < 8; j++){
				int final_square[2] = {j, i};
				if(isValidMove(init_square, final_square, piece))
					FillRect(i*32, j*32, 32, 32, olc::GREEN);
			}
		}
	}



	void MovePiece(olc::vf2d initPos){
		int init_square[2] = {int(floor(initPos.y*8/ScreenHeight())),int(floor(initPos.x*8/ScreenWidth()))};
		int end_square[2] = {-1,-1};
		int piece = chessBoard[init_square[0]][init_square[1]];
		drawValidMoves(piece, init_square);
		if(GetMouse(0).bReleased){
			end_square[0] = int(floor(GetMouseY()*8/ScreenHeight()));
			end_square[1] = int(floor(GetMouseX()*8/ScreenWidth()));
			if(isValidMove(init_square, end_square, piece)){
				/* Check if a pawn was taken en passant */
				if(piece == PAWN && (end_square[0] - init_square[0] == 1) && (abs(end_square[1] - init_square[1]) == 1) && (chessBoard[end_square[0]][end_square[1]] == 0))
					chessBoard[end_square[0] - 1][end_square[1]] = 0;
				else if(piece == PAWN + 10 && (init_square[0] - end_square[0] == 1) && (abs(end_square[1] - init_square[1]) == 1) && (chessBoard[end_square[0]][end_square[1]] == 0))
					chessBoard[end_square[0] + 1][end_square[1]] = 0;
				/* Update double pawn jump variable */
				if(piece == PAWN && abs(end_square[0] - init_square[0]) == 2)
					lastPawnMovedTwice = end_square[1];
				else if(piece == PAWN + 10 && abs(end_square[0] - init_square[0]) == 2)
					lastPawnMovedTwice = end_square[1] + 10;
				else
					lastPawnMovedTwice = -1;

				/* Update castle possibilities */ 
				if(piece == ROOK){
					if(init_square[0] == 0 && init_square[1] == 0)
						isCastlePossible[BLACK_LONG] = false;
					else if(init_square[0] == 0 && init_square[1] == 7)
						isCastlePossible[BLACK_SHORT] = false;
				}
				else if(piece == ROOK + 10){
					if(init_square[0] == 7 && init_square[1] == 0)
						isCastlePossible[WHITE_LONG] = false;
					else if(init_square[0] == 7 && init_square[1] == 7)
						isCastlePossible[WHITE_SHORT] = false;		
				}
				if(piece == KING){
					isCastlePossible[BLACK_SHORT] = 0;
					isCastlePossible[BLACK_LONG] = 0;
					if(end_square[1] == 2 && abs(end_square[1] - init_square[1]) == 2){
						chessBoard[0][2] = KING;
						chessBoard[0][3] = ROOK;	
						chessBoard[0][0] = 0;
						chessBoard[0][4] = 0;	
						return;				
					}
					else if(end_square[1] == 6 && abs(end_square[1] - init_square[1]) == 2){
						chessBoard[0][6] = KING;
						chessBoard[0][5] = ROOK;	
						chessBoard[0][7] = 0;
						chessBoard[0][4] = 0;	
						return;				
					}
				}
				else if(piece == KING + 10){
					isCastlePossible[WHITE_SHORT] = 0;
					isCastlePossible[WHITE_LONG] = 0;
					if(end_square[1] == 2 && abs(end_square[1] - init_square[1]) == 2){
						chessBoard[7][2] = KING + 10;
						chessBoard[7][3] = ROOK + 10;	
						chessBoard[7][0] = 0;
						chessBoard[7][4] = 0;	
						return;				
					}
					else if(end_square[1] == 6 && abs(end_square[1] - init_square[1]) == 2){
						chessBoard[7][6] = KING + 10;
						chessBoard[7][5] = ROOK + 10;	
						chessBoard[7][7] = 0;
						chessBoard[7][4] = 0;	
						return;				
					}		
				}

				/* Update squares */
				chessBoard[end_square[0]][end_square[1]] = chessBoard[init_square[0]][init_square[1]];
				chessBoard[init_square[0]][init_square[1]] = 0;

				/* Check for pawn promotions */
				if((chessBoard[end_square[0]][end_square[1]] == PAWN || chessBoard[end_square[0]][end_square[1]] == PAWN + 10) && ((end_square[0] == 0 )||(end_square[0] == 7)))
					chessBoard[end_square[0]][end_square[1]] += 8;
			}
		}
		/* More debug stuff */
		//std::string to_print = std::to_string(initPos.x) + ',' + std::to_string(initPos.y);
		//DrawString(ScreenWidth()/2, ScreenHeight()/2, std::to_string(piece), olc::RED);
	}

};


int main()
{
	Chess game;
	if (game.Construct(256, 256, 4, 4))
		game.Start();

	return 0;
}
