#include "Camera.h"


Camera::Camera()
{
	this->screenHeight = 480;
	this->screenWidth = 640;
	setDefaultValue();
}
Camera::Camera(int width, int height)
{
	this->screenHeight = height;
	this->screenWidth = width;
	setDefaultValue();
}
Camera::Camera(int width, int height, float x, float y, float z)
{
	this->screenHeight = height;
	this->screenWidth = width;

	this->Pos = Vector3(x, y, z);
	this->lookAtPoint = Vector3(0, 0, 1);
	this->lookUpPoint = Vector3(0, 1, 0);
	this->lookRightPoint = Vector3(1, 0, 0);

	this->mousePoint.x = 0;
	this->mousePoint.y = 0;
	this->mousePointOld.x = 100;
	this->mousePointOld.y = 100;
	this->yRotation = 0;
}
Camera::~Camera()
{

}
void Camera::Update(HWND hWnd)
{
	XMMATRIX rotation;
	Vector3 lookAtVector;
	Vector3 leftRight;
	Vector3 upDown;


	//ClientToScreen(hWnd, &point);
	//POINT point;
	//point = GetMousePos(hWnd);

	this->mousePoint = GetMousePos(hWnd);
	Vector3 posDiff(0, 0, 0);
	posDiff.x = mousePoint.x - mousePointOld.x;
	posDiff.y = mousePoint.y - mousePointOld.y;
	posDiff.Normalize();


	upDown = this->lookUpPoint - Vector3(0, 0, 0);
	upDown.Normalize();

	if (GetAsyncKeyState(VK_LEFT) || posDiff.x < 0)
	{
		rotation = XMMatrixRotationY(-0.001f);
		this->lookAtPoint = XMVector3Transform(this->lookAtPoint, rotation);
		this->lookRightPoint = XMVector3Transform(this->lookRightPoint, rotation);
	}
	else if (GetAsyncKeyState(VK_RIGHT)|| posDiff.x > 0 )
	{
		rotation = XMMatrixRotationY(0.001f);
		this->lookAtPoint = XMVector3Transform(this->lookAtPoint, rotation);
		this->lookRightPoint = XMVector3Transform(this->lookRightPoint, rotation);
	}

	lookAtVector = this->lookAtPoint - Vector3(0, 0, 0);
	lookAtVector.Normalize();

	leftRight = this->lookRightPoint - Vector3(0, 0, 0);
	leftRight.Normalize();

	if (this->yRotation > -3.14f / 3 &&  (GetAsyncKeyState(VK_UP) ||posDiff.y < 0))
	{
		rotation = XMMatrixRotationAxis(leftRight, -0.001f);
		this->yRotation += -0.001f;
		this->lookAtPoint = XMVector3Transform(this->lookAtPoint, rotation);
		this->lookUpPoint = XMVector3Transform(this->lookUpPoint, rotation);
	}
	else if (this->yRotation < 3.14f / 3 &&  (GetAsyncKeyState(VK_DOWN) ||posDiff.y > 0))
	{
		rotation = XMMatrixRotationAxis(leftRight, 0.001f);
		this->yRotation += 0.001f;
		this->lookAtPoint = XMVector3Transform(this->lookAtPoint, rotation);
		this->lookUpPoint = XMVector3Transform(this->lookUpPoint, rotation);
	}


	this->mousePointOld = this->mousePoint;
	if (this->mousePoint.x >= this->screenWidth || this->mousePoint.x <= 0)
	{
		//SetCursorPos(this->screenWidth / 2, this->mousePoint.y);
	}
	if (this->mousePoint.y >= this->screenHeight || this->mousePoint.y <= 0)
	{
		//SetCursorPos(this->mousePoint.x, this->screenHeight / 2);
	}

	//spacebar
	if (GetAsyncKeyState(VK_SPACE))
	{
		this->Pos += Vector3(0, 1, 0) * 0.0011f;
	}
	//ctrl
	if (GetAsyncKeyState(VK_CONTROL))
	{
		this->Pos += Vector3(0, 1, 0) * -0.0011f;
	}
	//0x57 = W
	if (GetAsyncKeyState(0x57))
	{
		this->Pos += lookAtVector * 0.0011f;
	}
	//0x53 = S
	if (GetAsyncKeyState(0x53))
	{
		this->Pos += lookAtVector * -0.0011f;
	}
	//0x44 = D
	if (GetAsyncKeyState(0x44))
	{
		this->Pos += leftRight * 0.0011f;
	}
	//0x41 = A
	if (GetAsyncKeyState(0x41))
	{
		this->Pos += leftRight * -0.0011f;
	}
	//0x52 = R
	if (GetAsyncKeyState(0x52))
	{
		setDefaultValue();
	}


}

Matrix Camera::getViewMatrix()
{
	Matrix viewMatrix;
	Matrix moveToCameraPos;
	Vector3 upDown;

	//Create translation matrix to move points to positions realtive to the camera
	moveToCameraPos = XMMatrixTranslation(this->Pos.x,this->Pos.y,this->Pos.z);

	//move lookAtPoint to its position relative to the camera
	Vector3 newLookPoint = Vector3(0, 0, 1);
	newLookPoint = XMVector3Transform(this->lookAtPoint, moveToCameraPos);

	//move lookUpPoint to its position relative to the camera
	Vector3 newLookUpPoint = Vector3(0, 1, 0);
	newLookUpPoint = XMVector3Transform(this->lookUpPoint, moveToCameraPos);
	upDown = newLookUpPoint - this->Pos;

	viewMatrix = Matrix(XMMatrixLookAtLH(this->Pos, newLookPoint, Vector3(0,1,0)));

	return viewMatrix;
}
Vector3 Camera::getCameraPos()
{
	return this->Pos;
}

void Camera::setDefaultValue()
{
	this->Pos = Vector3(0, 0, 0);
	this->lookAtPoint = Vector3(0, 0, 1);
	this->lookUpPoint = Vector3(0, 1, 0);
	this->lookRightPoint = Vector3(1, 0, 0);

	this->mousePoint.x = 0;
	this->mousePoint.y = 0;
	this->mousePointOld.x = 0;
	this->mousePointOld.y = 0;
	this->yRotation = 0;
}
POINT Camera::GetMousePos(HWND hWnd)
{
	//returns the position of the mouse relative to the screen
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	//ScreenToClient(hWnd, &cursorPos);
	return cursorPos;
}