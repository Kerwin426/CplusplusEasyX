#include<graphics.h>
#include<iostream>


char board_data[3][3] = {
	{'-','-','-'},{'-','-','-'},{'-','-','-'}
};

char current_piece = 'O';

bool checkWin(char c);
bool checkDraw();
void DrawBoard();
void DrawPiece();
void DrawTipText();

int main() {
	
	initgraph(600, 600);
	bool running = true;
	ExMessage msg;
	BeginBatchDraw();
	while (running) {
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN) {
				int x = msg.x;
				int y = msg.y;
				int index_x = x / 200;
				int index_y = y / 200;
				if (board_data[index_y][index_x] == '-') {
					board_data[index_y][index_x] = current_piece;
					if (current_piece == 'O') {
						current_piece = 'X';
					}
					else {
						current_piece = 'O';
					}
				}
			}
		
		}

		cleardevice();
		DrawBoard();
		DrawPiece();
		DrawTipText();
		FlushBatchDraw();
		if (checkWin('X')) {
			MessageBox(GetHWnd(),_T("X玩家获胜"),_T("游戏结束"),MB_OK);
			running = false;
		}
		else if (checkWin('O')) {
			MessageBox(GetHWnd(), _T("O玩家获胜"), _T("游戏结束"), MB_OK);
			running = false;
		}
		else if (checkDraw()) {
			MessageBox(GetHWnd(), _T("平局！"), _T("游戏结束"), MB_OK);
			running = false;
		
		}
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time-start_time ;
		if (delta_time < 1000 / 60) {
			Sleep(1000 / 60 - delta_time);
		}

	}
	EndBatchDraw();
	return 0;
}

bool checkWin(char c) {
	if (board_data[0][0] == c && board_data[0][1] == c && board_data[0][2] == c)
		return true;
	if (board_data[1][0] == c && board_data[1][1] == c && board_data[1][2] == c)
		return true;
	if (board_data[2][0] == c && board_data[2][1] == c && board_data[2][2] == c)
		return true;
	if (board_data[0][0] == c && board_data[1][0] == c && board_data[2][0] == c)
		return true;
	if (board_data[0][1] == c && board_data[1][1] == c && board_data[2][1] == c)
		return true;
	if (board_data[0][2] == c && board_data[1][2] == c && board_data[2][2] == c)
		return true;
	if (board_data[0][2] == c && board_data[1][1] == c && board_data[2][0] == c)
		return true;
	if (board_data[0][0] == c && board_data[1][1] == c && board_data[2][2] == c)
		return true;
	return false;
}


bool checkDraw() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (board_data[i][j] == '-') {
				return false;
			}
		}
	}
	return true;
}

void DrawBoard() {
	line(0,200,600,200);
	line(0, 400, 600, 400);
	line(200, 0, 200, 600);
	line(400, 0, 400, 600);

}
void DrawPiece() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			switch (board_data[i][j])
			{
			case'O':
				circle(200*j+100,200*i+100,100);
				break;
			case'X':
				line(200 * j, 200 * i, 200 * (j+1), 200 * (i + 1));
				line(200 * (j + 1), 200 * i, 200 * j, 200 * (i + 1));
				break;
			case'-':
				break;
			}
		}
	}

}
void DrawTipText() {
	static TCHAR str[64];
	_stprintf_s(str,_T("当前棋子类型：%c"),current_piece);
	settextcolor(RGB(255,175,45));
	outtextxy(0,0,str);

}