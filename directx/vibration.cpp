#include <Windows.h>
#include <XInput.h>
#pragma comment(lib, "xinput.lib")

int main(){
	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = 0xFFFF;
	vibration.wRightMotorSpeed = 0;
	XInputSetState(0, &vibration);

	Sleep(10000);

	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
	XInputSetState(0, &vibration);

	return 0;
}