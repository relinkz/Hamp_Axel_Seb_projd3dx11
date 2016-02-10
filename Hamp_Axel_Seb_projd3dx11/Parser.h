#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;
using namespace std;

struct intArr 
{
	int a;
	int b;
	int c;
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

	vector<Vector3*>verticies;
	vector<Vector3*>vertexNormals;
	vector<intArr*> faces;
	vector<Vector2*>UVtext;
};