#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;
using namespace std;

struct triangleData 
{
	int vIndex;
	int txIndex;
	int vNormIndex;
};

class Parser
{
public:
	Parser();
	virtual ~Parser();
	void progressFile(const string& dest) throw(...);
private:
	string mtllib;
	string geometry;
	string usemtl;

	vector<Vector3>verticies;
	vector<Vector3>vertexNormals;
	list<triangleData>triVertex;
	vector<Vector2>UVtext;
	void loadDataIntoList(const string& triangleDesc);
};