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

struct TriangleVertex
{
	float x, y, z;
	float u, v;
};

class Parser
{
public:
	Parser();
	virtual ~Parser();
	void progressFile(const string& dest) throw(...);
	
	/*getters*/
	int getNrOfTriangleVertices()	const;
	string getMtllib()				const;
	string getGeometry()			const;
	string useMtl()					const;


private:
	string mtllib;
	string geometry;
	string usemtl;
	bool finished;

	vector<Vector3>verticies;
	vector<Vector3>vertexNormals;
	list<triangleData>triVertex;
	list<TriangleVertex>finalVertexes;
	vector<Vector2>UVtext;

	void loadDataIntoTriangleData(const string& triangleDesc);
	void processDataIntoList();
};