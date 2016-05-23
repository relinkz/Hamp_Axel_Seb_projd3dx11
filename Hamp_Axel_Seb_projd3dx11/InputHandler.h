#pragma once
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class InputHandler
{
private:
	IDirectInput8* directInput;
	IDirectInputDevice8* mouse;

	DIMOUSESTATE mouseState;

	int screenWidth;
	int screenHeight;

	int mouse_X;
	int mouse_Y;

private:
	bool readMouse();
	void processInput();
public:
	InputHandler();
	virtual ~InputHandler();

	void initialize(HINSTANCE hInstance, HWND hwind, int screenWidth, int screenHeight) throw(...);
	void shutdown();
	void update();

	void getMouseLocation(int &xPos, int &yPos);
};

#endif

