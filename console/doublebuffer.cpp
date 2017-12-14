#include <Windows.h>

#define SCREEN_X 120
#define SCREEN_Y 30

HANDLE frontBuffer, backBuffer;

void swapBuffer() {
	HANDLE temp;

	temp = frontBuffer;
	frontBuffer = backBuffer;
	backBuffer = temp;

	SetConsoleActiveScreenBuffer(frontBuffer);
}

int main(){
	COORD pos = {0, 0}, size = {SCREEN_X, SCREEN_Y};
	SMALL_RECT region = {0, 0, SCREEN_X, SCREEN_Y};
	CHAR_INFO *bitmap;

	frontBuffer = CreateConsoleScreenBuffer(
		GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	backBuffer = CreateConsoleScreenBuffer(
		GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(backBuffer);

	bitmap = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * SCREEN_X * SCREEN_Y);

	// Main loop
	while(true) {
		for(int i=0; i<SCREEN_X * SCREEN_Y; i++) {
			bitmap[i].Char.AsciiChar = (char)' ';
			bitmap[i].Attributes = 0b0111;
		}

		WriteConsoleOutput(backBuffer, bitmap, size, pos, &region);

		swapBuffer();
	}

	return 0;
}