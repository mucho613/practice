#include <Windows.h>
#include <math.h>
#include <string.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

#define SCREEN_X 128
#define SCREEN_Y 128

#define CHAR_ON 'W'
#define CHAR_OFF ' '

HANDLE frontBuffer, backBuffer;

void swapBuffer() {
	HANDLE temp;

	temp = frontBuffer;
	frontBuffer = backBuffer;
	backBuffer = temp;

	SetConsoleActiveScreenBuffer(frontBuffer);
}

void rotate(CHAR_INFO *matrix, CHAR_INFO *rotated, Eigen::Vector3d vRadian) {
	for(int i=0; i<SCREEN_X; i++) {
		for(int j=0; j<SCREEN_Y; j++) {
			rotated[i*SCREEN_X + j].Char.UnicodeChar = CHAR_OFF;
			rotated[i*SCREEN_X + j].Attributes = 0b0111;
		}
	}

	Eigen::Vector2d v1;
	Eigen::Vector2d vCenter(SCREEN_X / 2, SCREEN_Y / 2);

	Eigen::Matrix2d mRotateX;
	Eigen::Matrix2d mRotateY;
	Eigen::Matrix2d mRotateZ;
	Eigen::Matrix2d mRotate;

	mRotateX <<	1, 0,
			 				0, 1;

	mRotateY <<	cos(vRadian.y()), 0,
	 						0, 1;

	mRotateZ <<	cos(vRadian.z()), -1 * sin(vRadian.z()),
	 						sin(vRadian.z()), cos(vRadian.z());

	mRotate = mRotateZ * mRotateY * mRotateX;

	Eigen::Vector2d vRotated;

	for(int i=0; i<SCREEN_Y; i++) {
		for(int j=0; j<SCREEN_X; j++) {
			int x, y;

			v1.x() = j;
			v1.y() = i;

			v1 -= vCenter;

			vRotated = mRotate * v1;

			vRotated += vCenter;

			x = vRotated.x();
			y = vRotated.y();

			if(x>0 && x<SCREEN_X && y>0 && y<SCREEN_Y) {
				memcpy(&rotated[y*SCREEN_X+x], &matrix[i*SCREEN_X+j], sizeof(CHAR_INFO));
			}
		}
	}
}
 
int main(){
	COORD pos = {0, 0}, size = {SCREEN_X, SCREEN_Y};
	SMALL_RECT region = {0, 0, SCREEN_X, SCREEN_Y};
	CHAR_INFO *matrix, *rotated;
	Eigen::Vector3d vRotation(0, 0, 0);

	frontBuffer = CreateConsoleScreenBuffer(
		GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	backBuffer = CreateConsoleScreenBuffer(
		GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(backBuffer);

	matrix  = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * (SCREEN_X * SCREEN_Y));
	rotated = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * (SCREEN_X * SCREEN_Y));

	// Generate image
	for(int i=0; i<SCREEN_X; i++) {
		for(int j=0; j<SCREEN_Y; j++) {
			WCHAR temp;
			if((i>SCREEN_Y/4 && i<SCREEN_Y/4*3) && (j>SCREEN_X/4 && j<SCREEN_X/4*3)) {
				matrix[i*SCREEN_X + j].Char.UnicodeChar = CHAR_ON;
				if(i<SCREEN_Y/2) {
					if(j<SCREEN_X/2) {
						matrix[i*SCREEN_X + j].Attributes = 0b0100;
					}
					if(j>=SCREEN_X/2) {
						matrix[i*SCREEN_X + j].Attributes = 0b0010;
					}
				}
				if(i>=SCREEN_Y/2) {
					if(j<SCREEN_X/2) {
						matrix[i*SCREEN_X + j].Attributes = 0b0001;
					}
					if(j>=SCREEN_X/2) {
						matrix[i*SCREEN_X + j].Attributes = 0b0110;
					}
				}	
			}
			else {
				matrix[i*SCREEN_X + j].Char.UnicodeChar = CHAR_OFF;
				matrix[i*SCREEN_X + j].Attributes = 0b0111;
			}
		}
	}

	// Main loop
	while(true) {
		rotate(matrix, rotated, vRotation);

		// “K“–‚É‰ñ‚·
		vRotation.x() += (3.1415926535 * 2) / 720;
		vRotation.y() += (2.1415926535 * 2) / 320;
		vRotation.z() += (3.1415926535 * 2) / 520;

		WriteConsoleOutput(backBuffer, rotated, size, pos, &region);

		// Sleep(1000/60);

		swapBuffer();
	}

	return 0;
}