#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include "SimpleMath.h"
//#include "SimpleMath.inl"

//#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace SimpleMath;

class Camera
{
private:
	Vector3 Pos;
	Vector3 lookAtPoint;
	Vector3 lookRightPoint;
	Vector3 lookUpPoint;
	int screenWidth;
	int screenHeight;
	float yRotation;

	POINT mousePoint;
	POINT mousePointOld;

	void setDefaultValue();

public:
	Camera();
	Camera(int width, int height);
	Camera(int width, int height, float x, float y, float z);
	~Camera();

	void Update(HWND hWnd);

	Matrix getViewMatrix();
	Vector3 getCameraPos();
	POINT GetMousePos(HWND hWnd);

};


#endif