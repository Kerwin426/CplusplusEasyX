#include<graphics.h>
#include<iostream>

int main() {
	initgraph(1280,720);
	int x=300, y=300;
	BeginBatchDraw();
	while (true) {
		ExMessage mag;
		while (peekmessage(&mag)) {
			if (mag.message == WM_MOUSEMOVE) {
				x = mag.x;
				y = mag.y;
			}
		}
		cleardevice();
		solidcircle(x,y,100);
		FlushBatchDraw();
	}
	EndBatchDraw();
	return 0;


}