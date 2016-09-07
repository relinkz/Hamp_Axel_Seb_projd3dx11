#include "Camera.h"



Camera::Camera()
{
	this->useMouse = false;
	this->screenHeight = 480;
	this->screenWidth = 640;
	setDefaultValue();
	setUpViewFustrumPlanes();
}
Camera::Camera(int width, int height)
{
	this->useMouse = false;
	this->screenHeight = height;
	this->screenWidth = width;
	setDefaultValue();
	setUpViewFustrumPlanes();
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
	setUpViewFustrumPlanes();
}
Camera::~Camera()
{

}
void Camera::Update(HWND hWnd, Terrain* terrain, float dt)
{
	XMMATRIX rotation;
	XMMATRIX transToOrig;
	XMMATRIX transBack;
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

	float rotationPerFrame = 1.0f;
	//Vector3 offset = this->Pos - Vector3(0, 0, 0);
	//transToOrig = DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z);
	//
	//offset = Vector3(0, 0, 0) - this->Pos;
	//transBack = DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z);

	if (GetAsyncKeyState(VK_LEFT) || (posDiff.x < 0 && this->useMouse == true))
	{
		rotation = DirectX::XMMatrixRotationY(-rotationPerFrame* dt);
		//this->lookAtPoint = DirectX::XMVector3Transform(this->lookAtPoint, rotation);
		//this->lookRightPoint = DirectX::XMVector3Transform(this->lookRightPoint, rotation);
		rotatePoint(this->lookAtPoint, rotation);
		rotatePoint(this->lookRightPoint, rotation);

		for (int i = 0; i < this->viewFustrumPlanes.size(); i++)
		{
			rotatePoint(this->viewFustrumPlanes.at(i).pos, rotation);
			rotatePoint(this->viewFustrumPlanes.at(i).normal, rotation);
		}
	}
	else if (GetAsyncKeyState(VK_RIGHT) || (posDiff.x > 0 && this->useMouse == true))
	{
		rotation = DirectX::XMMatrixRotationY(rotationPerFrame* dt);
		//this->lookAtPoint = DirectX::XMVector3Transform(this->lookAtPoint, rotation);
		//this->lookRightPoint = DirectX::XMVector3Transform(this->lookRightPoint, rotation);
		rotatePoint(this->lookAtPoint, rotation);
		rotatePoint(this->lookRightPoint, rotation);
		for (int i = 0; i < this->viewFustrumPlanes.size(); i++)
		{
			rotatePoint(this->viewFustrumPlanes.at(i).pos, rotation);
			rotatePoint(this->viewFustrumPlanes.at(i).normal, rotation);
		}
	}

	lookAtVector = this->lookAtPoint - Vector3(0, 0, 0);
	lookAtVector.Normalize();

	leftRight = this->lookRightPoint - Vector3(0, 0, 0);
	leftRight.Normalize();

	if (this->yRotation > -3.14f / 3 && (GetAsyncKeyState(VK_UP) || (posDiff.y < 0 && this->useMouse == true)))
	{
		rotation = DirectX::XMMatrixRotationAxis(leftRight, -rotationPerFrame* dt);
		this->yRotation += -rotationPerFrame* dt;
		//this->lookAtPoint = DirectX::XMVector3Transform(this->lookAtPoint, rotation);
		//this->lookUpPoint = DirectX::XMVector3Transform(this->lookUpPoint, rotation);
		rotatePoint(this->lookAtPoint, rotation);
		rotatePoint(this->lookUpPoint, rotation);
		for (int i = 0; i < this->viewFustrumPlanes.size(); i++)
		{
			rotatePoint(this->viewFustrumPlanes.at(i).pos, rotation);
			rotatePoint(this->viewFustrumPlanes.at(i).normal, rotation);
		}
	}
	else if (this->yRotation < 3.14f / 3 && (GetAsyncKeyState(VK_DOWN) || (posDiff.y > 0 && this->useMouse == true)))
	{
		rotation = DirectX::XMMatrixRotationAxis(leftRight, rotationPerFrame* dt);
		this->yRotation += rotationPerFrame* dt;
		//this->lookAtPoint = DirectX::XMVector3Transform(this->lookAtPoint, rotation);
		//this->lookUpPoint = DirectX::XMVector3Transform(this->lookUpPoint, rotation);
		rotatePoint(this->lookAtPoint, rotation);
		rotatePoint(this->lookUpPoint, rotation);
		for (int i = 0; i < this->viewFustrumPlanes.size(); i++)
		{
			rotatePoint(this->viewFustrumPlanes.at(i).pos, rotation);
			rotatePoint(this->viewFustrumPlanes.at(i).normal, rotation);
		}
	}


	this->mousePointOld = this->mousePoint;
	if (this->useMouse == true && (this->mousePoint.x >= this->screenWidth || this->mousePoint.x <= 0))
	{
		SetCursorPos(this->screenWidth / 2, this->mousePoint.y);
	}
	if (this->useMouse == true && (this->mousePoint.y >= this->screenHeight || this->mousePoint.y <= 5))
	{
		SetCursorPos(this->mousePoint.x, this->screenHeight / 2);
	}

	//spacebar
	if (GetAsyncKeyState(VK_SPACE))
	{
		this->Pos += Vector3(0, 1, 0) * dt;
		//this->moveViewFustrum(Vector3(0, 1, 0) * dt);
	}
	/*
	else//walking on terrain
	{
		this->Pos += Vector3(0, -1, 0) * 0.0011f;
		
		float newY = terrain->getY(this->Pos.x, this->Pos.z);

		if (newY > this->Pos.y - 1)
		//if (0 > newY)
		{
			this->Pos.y = newY + 1;
		}

	}
	*/
	//ctrl
	if (GetAsyncKeyState(VK_CONTROL))
	{
		this->Pos += Vector3(0, 1, 0) * -dt;
		//this->moveViewFustrum(Vector3(0, 1, 0) * -dt);
	}
	//0x57 = W
	if (GetAsyncKeyState(0x57))
	{
		this->Pos += lookAtVector * dt;
		//this->moveViewFustrum(lookAtVector * dt);
	}
	//0x53 = S
	if (GetAsyncKeyState(0x53))
	{
		this->Pos += lookAtVector * -dt;
		//this->moveViewFustrum(lookAtVector * -dt);
	}
	//0x44 = D
	if (GetAsyncKeyState(0x44))
	{
		this->Pos += leftRight * dt;
		//this->moveViewFustrum(leftRight * dt);
	}
	//0x41 = A
	if (GetAsyncKeyState(0x41))
	{
		this->Pos += leftRight * -dt;
		//this->moveViewFustrum(leftRight * -dt);
	}
	//0x52 = R
	if (GetAsyncKeyState(0x52))
	{
		setDefaultValue();

		this->viewFustrumPlanes.clear();
		this->setUpViewFustrumPlanes();
	}
	if (GetAsyncKeyState(0x4D))
	{
		this->useMouse = false;
	}
	if (GetAsyncKeyState(0x4E))
	{
		this->useMouse = true;
	}


}

Matrix Camera::getViewMatrix()
{
	Matrix viewMatrix;
	Matrix moveToCameraPos;
	Vector3 upDown;

	//Create translation matrix to move points to positions realtive to the camera
	moveToCameraPos = DirectX::XMMatrixTranslation(this->Pos.x, this->Pos.y, this->Pos.z);

	//move lookAtPoint to its position relative to the camera
	Vector3 newLookPoint = Vector3(0, 0, 1);
	newLookPoint = DirectX::XMVector3Transform(this->lookAtPoint, moveToCameraPos);

	//move lookUpPoint to its position relative to the camera
	Vector3 newLookUpPoint = Vector3(0, 1, 0);
	newLookUpPoint = DirectX::XMVector3Transform(this->lookUpPoint, moveToCameraPos);
	upDown = newLookUpPoint - this->Pos;

	viewMatrix = Matrix(DirectX::XMMatrixLookAtLH(this->Pos, newLookPoint, Vector3(0, 1, 0)));
	
	//Vector3 tempPos = Vector3(5, 7, 0);
	//Vector3 tempLook = Vector3(tempPos.x, tempPos.y - 1, tempPos.z);
	//viewMatrix = Matrix(DirectX::XMMatrixLookAtLH(tempPos, tempLook, Vector3(0, 0, 1)));

	return viewMatrix;
}
Vector3 Camera::getCameraPos()
{
	return this->Pos;
}

Vector3 Camera::getLookAtPoint()
{
	return this->lookAtPoint;
}

Vector3 Camera::getLookRight() const
{
	return this->lookRightPoint;
}

Vector3 Camera::getLookUp() const
{
	return this->lookUpPoint;
}

void Camera::moveViewFustrum(Vector3 move)
{
	for (int i = 0; i < this->viewFustrumPlanes.size(); i++)
	{
		this->viewFustrumPlanes.at(i).pos += move;
	}

}

void Camera::setDefaultValue()
{
	this->Pos = Vector3(5, 1, -1);

	this->lookAtPoint = Vector3(0, 0, 1);
	this->lookUpPoint = Vector3(0, 1, 0);
	this->lookRightPoint = Vector3(1, 0, 0);

	this->mousePoint.x = 0;
	this->mousePoint.y = 0;
	this->mousePointOld.x = 0;
	this->mousePointOld.y = 0;
	this->yRotation = 0;
}
void Camera::rotatePoint(Vector3 &point, XMMATRIX rotation)
{
	point = DirectX::XMVector3Transform(point, rotation);
}
POINT Camera::GetMousePos(HWND hWnd)
{
	//returns the position of the mouse relative to the screen
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	//ScreenToClient(hWnd, &cursorPos);
	return cursorPos;
}

void Camera::setPosition(Vector3 pos)
{
	this->Pos = pos;
}

void Camera::setUpViewFustrumPlanes()
{
	//top plane pointing up
	plane top
	{
		Vector3(0, 3, 2),Vector3(0, 1, 0)
	};
	viewFustrumPlanes.push_back(top);
	//bottom plnae pointing down
	plane bottom
	{
		Vector3(0,-3,2),Vector3(0, -1, 0)
	};
	viewFustrumPlanes.push_back(bottom);
	//right plane pointing to the right	
	plane right
	{
		Vector3(2, 0, 2), Vector3(1, 0, 0)
	};
	viewFustrumPlanes.push_back(right);
	//left plane pointing to the left	
	plane left
	{
		Vector3(-2, 0, 2), Vector3(-1, 0, 0)
	};
	viewFustrumPlanes.push_back(left);
	//forward plane pointing to the left	
	plane forward
	{
		Vector3(0, 0, 6), Vector3(0, 0, 1)
	};
	viewFustrumPlanes.push_back(forward);
	//backward plane pointing to the back	
	plane backward
	{
		Vector3(0, 0, 0), Vector3(0, 0, -1)
	};
	viewFustrumPlanes.push_back(backward);


	//Matrix moveToCameraPos;
	//moveToCameraPos = Matrix(XMMatrixTranslation(this->Pos.x, this->Pos.y, this->Pos.z));
	//for (int i = 0; i < this->viewFustrumPlanes.size(); i++)
	//{
	//
	//	Vector3 temp = XMVector3Transform(viewFustrumPlanes.at(i).pos, moveToCameraPos);
	//	viewFustrumPlanes.at(i).pos = temp;
	//}

	//Vector3 objectToPlane = temp - objects.at(a)->getPosition();
}


void Camera::doFustrumCulling(std::vector<Object> &objects)
{


	for (int a = 0; a < objects.size(); a++)
	{
		if (objects.at(a).getShouldRender())
		{
			bool result = true;
			for (int i = 0; i < this->viewFustrumPlanes.size(); i++)
			{
				Vector3 objectToPlane = viewFustrumPlanes.at(i).pos - objects.at(a).getPosition();
				objectToPlane.Normalize();

				float dot = objectToPlane.Dot(viewFustrumPlanes.at(i).normal);

				if (dot < 0)
				{
					result = false;
				}

			}
			if (result == true)
			{
				//objectsToDraw.push_back(objects.at(a));
			}
		}
	}

	//vector<Object*> objectsToDraw;
	//Matrix moveToCameraPos;
	//moveToCameraPos = Matrix(XMMatrixTranslation(this->Pos.x, this->Pos.y, this->Pos.z));

	/*for (int a = 0; a < objects.size(); a++)
	{
		bool result = true;
		for (int i = 0; i < viewFustrumPlanes.size() && result == true; i++)
		{
			//Vector3 temp = XMVector3Transform(viewFustrumPlanes.at(i).pos, moveToCameraPos);

			//Vector3 objectToPlane = temp - objects.at(a)->getPosition();
			Vector3 objectToPlane = viewFustrumPlanes.at(i).pos - objects.at(a)->getPosition();
			objectToPlane.Normalize();

			float dot = objectToPlane.Dot(viewFustrumPlanes.at(i).normal);

			if (dot < 0)
			{
				result = false;
			}
		}
		if (result == true)
		{
			objectsToDraw.push_back(objects.at(a));
		}

	}

	return objectsToDraw;*/

}

void Camera::doFustrumCulling(Object * object)
{
	DirectX::XMMATRIX translationToCamera;

	Vector3 offSet = this->Pos - Vector3(0, 0, 0);

	translationToCamera = DirectX::XMMatrixTranslation(offSet.x, offSet.y, offSet.z);

	for (int i = 0; i < this->viewFustrumPlanes.size() && object->getShouldRender() == true; i++)
	{
		Vector3 planePos = this->viewFustrumPlanes.at(i).pos;
		Vector3 planeNorm = this->viewFustrumPlanes.at(i).normal;

		planePos = DirectX::XMVector3Transform(planePos, translationToCamera);

		Vector3 objectToPlane = planePos - object->getPosition();

		objectToPlane.Normalize();

		float dot = objectToPlane.Dot(planeNorm);

		if (dot < 0)
		{
			object->swapRender();
		}
	}
}
