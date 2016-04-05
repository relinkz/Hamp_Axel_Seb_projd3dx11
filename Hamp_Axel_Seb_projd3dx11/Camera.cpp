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
void Camera::Update(HWND hWnd, Terrain* terrain)
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

	if (GetAsyncKeyState(VK_LEFT))
	{
		rotation = DirectX::XMMatrixRotationY(-0.001f);
		this->lookAtPoint = DirectX::XMVector3Transform(this->lookAtPoint, rotation);
		this->lookRightPoint = DirectX::XMVector3Transform(this->lookRightPoint, rotation);
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		rotation = DirectX::XMMatrixRotationY(0.001f);
		this->lookAtPoint = DirectX::XMVector3Transform(this->lookAtPoint, rotation);
		this->lookRightPoint = DirectX::XMVector3Transform(this->lookRightPoint, rotation);
	}

	lookAtVector = this->lookAtPoint - Vector3(0, 0, 0);
	lookAtVector.Normalize();

	leftRight = this->lookRightPoint - Vector3(0, 0, 0);
	leftRight.Normalize();

	if (this->yRotation > -3.14f / 3 &&  (GetAsyncKeyState(VK_UP)))
	{
		rotation = DirectX::XMMatrixRotationAxis(leftRight, -0.001f);
		this->yRotation += -0.001f;
		this->lookAtPoint = DirectX::XMVector3Transform(this->lookAtPoint, rotation);
		this->lookUpPoint = DirectX::XMVector3Transform(this->lookUpPoint, rotation);
	}
	else if (this->yRotation < 3.14f / 3 &&  (GetAsyncKeyState(VK_DOWN)))
	{
		rotation = DirectX::XMMatrixRotationAxis(leftRight, 0.001f);
		this->yRotation += 0.001f;
		this->lookAtPoint = DirectX::XMVector3Transform(this->lookAtPoint, rotation);
		this->lookUpPoint = DirectX::XMVector3Transform(this->lookUpPoint, rotation);
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
	else//walking on terrain
	{
		this->Pos += Vector3(0, -1, 0) * 0.0011f;
		int intCamX = (int)floorf(this->Pos.x);
		int intCamZ = (int)floorf(this->Pos.z);

		if (intCamX > terrain->getWidth()-1)
			intCamX = terrain->getWidth()-1;
		if (intCamZ > terrain->getLength()-1)
			intCamZ = terrain->getLength()-1;
		if (intCamX < 0)
			intCamX = 0;
		if (intCamZ < 0)
			intCamZ = 0;


		float y0 = terrain->getY(intCamX, intCamZ);
		float y1 = terrain->getY(intCamX, intCamZ + 1);
		float y2 = terrain->getY(intCamX + 1, intCamZ);
		float y3 = terrain->getY(intCamX + 1, intCamZ + 1);

		XMVECTOR v1 = XMLoadFloat3(&XMFLOAT3(intCamX, y0, intCamZ));
		XMVECTOR v2 = XMLoadFloat3(&XMFLOAT3(intCamX, y1, intCamZ + 1));
		XMVECTOR v3 = XMLoadFloat3(&XMFLOAT3(intCamX + 1, y2, intCamZ));
		XMVECTOR v4 = XMLoadFloat3(&XMFLOAT3(intCamX + 1, y3, intCamZ + 1));

		XMVECTOR norm;
		
		

		if (this->Pos.x - intCamX < this->Pos.z - intCamZ)//vänster triangel
		{
			norm = XMVector3Cross(XMVectorSubtract(v1, v2), XMVectorSubtract(v4, v2));
		}
		else//höger triangel
		{
			norm = XMVector3Cross(XMVectorSubtract(v1, v3), XMVectorSubtract(v4, v3));
		}

		float d = -(XMVectorGetX(norm) * XMVectorGetX(v1) + XMVectorGetY(norm) * XMVectorGetY(v1) + XMVectorGetZ(norm) * XMVectorGetZ(v1));
		float newY = (this->Pos.x * XMVectorGetX(norm) + this->Pos.z * XMVectorGetZ(norm) + d) / -XMVectorGetY(norm);
		//float newY = (XMVectorGetX(norm) * this->Pos.x + XMVectorGetY(norm) * (this->Pos.y - 1) + XMVectorGetZ(norm) * this->Pos.z + d)
		//	/ sqrt(XMVectorGetX(norm)*XMVectorGetX(norm) + XMVectorGetY(norm)*XMVectorGetY(norm) + XMVectorGetZ(norm)*XMVectorGetZ(norm));

		if (newY > this->Pos.y - 1)
		//if (0 > newY)
		{
			this->Pos.y = newY + 1;
		}


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
	moveToCameraPos = DirectX::XMMatrixTranslation(this->Pos.x,this->Pos.y,this->Pos.z);

	//move lookAtPoint to its position relative to the camera
	Vector3 newLookPoint = Vector3(0, 0, 1);
	newLookPoint = DirectX::XMVector3Transform(this->lookAtPoint, moveToCameraPos);

	//move lookUpPoint to its position relative to the camera
	Vector3 newLookUpPoint = Vector3(0, 1, 0);
	newLookUpPoint = DirectX::XMVector3Transform(this->lookUpPoint, moveToCameraPos);
	upDown = newLookUpPoint - this->Pos;

	viewMatrix = Matrix(DirectX::XMMatrixLookAtLH(this->Pos, newLookPoint, Vector3(0,1,0)));

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