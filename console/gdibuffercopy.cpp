#include <Windows.h>
#include <math.h>

#define NUM 1000
#define TWOPI (2 * 3.14159)

#define SCREEN_X 640
#define SCREEN_Y 480

#define CONVERT_0BGR_TO_0RGB(crColor) (0b00000000 |	(crColor & 0x0800) >> 9 | (crColor & 0x0080) >> 6 | (crColor & 0x0008) >> 3)

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

HANDLE buffer;
CHAR_INFO *bitmap;
COORD pos = {0, 0}, size = {SCREEN_X, SCREEN_Y};
SMALL_RECT region = {0, 0, SCREEN_X, SCREEN_Y};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
	static TCHAR szAppName[] = TEXT("SineWave");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra	= 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	// Prepare console
	AllocConsole();
	buffer = CreateConsoleScreenBuffer(GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(buffer);
	bitmap = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * SCREEN_X * SCREEN_Y);

	if(!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("Program requires Windows NT."), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("Sine Wave Using Polyline"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_X, SCREEN_Y,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;
	POINT apt[NUM];

	static HBITMAP hbmpBMP, hbmpOld;
	static HDC hdcBMP;
	static BITMAPINFO biBMP;
	static LPDWORD lpdwPixel;
	int i, j;

	switch(message) {
	case WM_CREATE:
		ZeroMemory(&biBMP, sizeof(biBMP));
		biBMP.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		biBMP.bmiHeader.biBitCount = 32;
		biBMP.bmiHeader.biPlanes = 1;
		biBMP.bmiHeader.biWidth = SCREEN_X;
		biBMP.bmiHeader.biHeight = -SCREEN_Y;
		hbmpBMP = CreateDIBSection(NULL, &biBMP, DIB_RGB_COLORS, (void **)(&lpdwPixel), NULL, 0);
		hdc = GetDC(hwnd);
		hdcBMP = CreateCompatibleDC(hdc);
		ReleaseDC(hwnd, hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcBMP, hbmpBMP);

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		for(i=0; i<SCREEN_X * SCREEN_Y; i++) {
			lpdwPixel[i] = 0x00FFFFFF;
		}

		MoveToEx(hdcBMP, 0, cyClient / 2, NULL);
		LineTo(hdcBMP, cxClient, cyClient / 2);

		for(i=0; i<NUM; i++) {
			apt[i].x = i * cxClient / NUM;
			apt[i].y = (int)(cyClient / 2 * (1 - sin(TWOPI * i / NUM)));
		}
		Polyline(hdcBMP, apt, NUM);

		BitBlt(hdc, 0, 0, SCREEN_X, SCREEN_Y, hdcBMP, 0, 0, SRCCOPY);

		// Draw to console buffer
		COLORREF crColor;
		for(int i=0; i<SCREEN_X * SCREEN_Y; i++) {
			crColor = lpdwPixel[i];
			bitmap[i].Char.AsciiChar = (char)' ';
		 	bitmap[i].Attributes = CONVERT_0BGR_TO_0RGB(crColor) << 4;
		}
		WriteConsoleOutput(buffer, bitmap, size, pos, &region);

		EndPaint(hwnd, &ps);

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		SelectObject(hdcBMP, hbmpOld);
		DeleteObject(hbmpBMP);
		DeleteDC(hdcBMP);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}