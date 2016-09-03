#ifndef QUADTREE_H
#define QUADTREE_H

#include <windows.h>
#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include "SimpleMath.h"
#include "Object.h"

using namespace DirectX;
using namespace SimpleMath;

static const int max = 10;

class QuadTree
{
private:

	Vector3 quadStart;
	Vector3 quadEnd;
	QuadTree* topLeft;
	QuadTree* topRight;
	QuadTree* bottomLeft;
	QuadTree* bottomRight;
	int xMax;
	int xMin;
	int zMax;
	int zMin;

	vector<Object*> objects;

	bool giveObject(Object &object);
	void addObject(Object &object);
	float lengthOf(Vector3 point1, Vector3 point2);
	bool isInsideQuad(Vector3 position);
public:
	QuadTree();
	QuadTree(Vector3 start, Vector3 end, int depth, int xMax, int xMin, int zMax, int zMin);
	~QuadTree();

	void setTreeData(vector<Object> &objects);
	//vector<Object*> getObjectsToDraw(Vector3 position);
	void getObjectsToDraw(Vector3 position);



};


#endif
