#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include "Object.h"
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
struct plane
{
	Vector3 pos;
	Vector3 normal;
};
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

	std::vector<plane> viewFustrumPlanes;

	void setDefaultValue();
	void rotatePoint(Vector3 &point, XMMATRIX rotation);

public:
	Camera();
	Camera(int width, int height);
	Camera(int width, int height, float x, float y, float z);
	~Camera();

	void Update(HWND hWnd, float dt);

	Matrix getViewMatrix();
	Vector3 getCameraPos();
	Vector3 getLookAtPoint();
	Vector3 getLookRight() const;
	Vector3 getLookUp() const;

	POINT GetMousePos(HWND hWnd);
	void setUpViewFustrumPlanes();
	std::vector<Object*> doFustrumCulling(std::vector<Object*> objects);

};


#endif