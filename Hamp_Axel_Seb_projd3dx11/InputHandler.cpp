#include "InputHandler.h"

bool InputHandler::readMouse()
{
	return false;
}

void InputHandler::processInput()
{
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

	resultHelp = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)this->directInput, nullptr);

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
	resultHelp = this->mouse->SetCooperativeLevel(hwind, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	if (FAILED(resultHelp))
	{
		throw("INPUTHANDLER : failed to set the mouse cooperative level");
	}

	//accuire the mouse
	resultHelp = this->mouse->Acquire();

	if (FAILED(resultHelp))
	{
		throw("INPUTHANDLER : failed to accuire interface");
	}
}

void InputHandler::shutdown()
{
}

void InputHandler::update()
{
}

void InputHandler::getMouseLocation(int & xPos, int & yPos)
{
}
