#include "InputHandler.h"

bool InputHandler::readMouse()
{
	bool result = true;
	HRESULT resultHelp;

	//read the mouse device
	resultHelp = this->mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&this->mouseState);
	
	if (FAILED(resultHelp))
	{
		//read the mouse device
		if ((resultHelp == DIERR_INPUTLOST) || resultHelp == DIERR_NOTACQUIRED)
		{
			this->mouse->Acquire();
		}
		else
		{
			result = false;
		}
	}
	return result;
}

void InputHandler::processInput()
{
	//update the location of the mouse curson based on the change of the mouse location frame
	this->mouse_X += this->mouseState.lX;
	this->mouse_Y += this->mouseState.lY;

	//ensure the mouse location doesn't exceed the screen with or height
	if (this->mouse_X < 0){ this->mouse_X = 0; }
	if (this->mouse_Y < 0){	this->mouse_Y = 0; }

	if (this->mouse_X > this->screenWidth){	this->mouse_X = this->screenWidth; }
	if (this->mouse_Y > this->screenHeight){ this->mouse_Y = this->screenHeight; }
}

InputHandler::InputHandler()
{
	this->directInput = nullptr;
	this->mouse = nullptr;
}

InputHandler::~InputHandler()
{
}

void InputHandler::initialize(HINSTANCE hInstance, HWND hwind, int screenWidth, int screenHeight) throw(...)
{
	HRESULT resultHelp;

	//Store the screen size which will be used for positioning of the mouse coursur
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;

	//initialize the location of the mouse to screen
	this->mouse_X = 0;
	this->mouse_Y = 0;

	resultHelp = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&this->directInput, nullptr);

	if (FAILED(resultHelp))
	{
		throw("INPUTHANDLER : failed to create DirectInput8Create");
	}

	//initialize the interface for mouse
	resultHelp = this->directInput->CreateDevice(GUID_SysMouse, &this->mouse, nullptr);

	if (FAILED(resultHelp))
	{
		throw("INPUTHANDLER : failed to create mouse interface");
	}

	//set the format using predef mouse data format
	resultHelp = this->mouse->SetDataFormat(&c_dfDIMouse);
	
	if (FAILED(resultHelp))
	{
		throw("INPUTHANDLER : failed to set mouseformat");
	}

	//the tutorial says:
	//set the cooperatice level of the mouse to share with other programs
	resultHelp = this->mouse->SetCooperativeLevel(hwind, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	if (FAILED(resultHelp))
	{
		throw("INPUTHANDLER : failed to set the mouse cooperative level");
	}

	//accuire the mouse
	resultHelp = this->mouse->Acquire();

	if (FAILED(resultHelp))
	{
		//throw("INPUTHANDLER : failed to accuire interface");
	}
}

void InputHandler::shutdown()
{
	if (this->mouse != nullptr)
	{
		this->mouse->Unacquire();
		this->mouse->Release();
		this->mouse = nullptr;
	}

	if (this->directInput != nullptr)
	{
		this->directInput->Release();
		this->directInput = nullptr;
	}
}

void InputHandler::update()
{
	bool result = false;

	//read the state of the mouse
	result = readMouse();

	if (result == true)
	{
		this->processInput();
	}
}

void InputHandler::getMouseLocation(int & xPos, int & yPos)
{
	xPos = this->mouse_X;
	yPos = this->mouse_Y;
}
