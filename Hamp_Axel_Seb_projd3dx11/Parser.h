#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;
using namespace std;

class Parser
{
public:
	Parser();
	~Parser();
	void progressFile(const string& dest);
	//getVerticies(); const
	string toString() const;

private:
	vector<Vector3*>verticies;
	vector<Vector2*>UVtext;
	vector<Vector3*>faces;
};