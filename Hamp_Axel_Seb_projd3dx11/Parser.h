#ifndef PARSER_H
#define PARSER_H
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"
#include "Material.h"

struct triangleData
{
	int vIndex;
	int txIndex;
	int vNormIndex;
};

struct TriangleVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	int ID;
};

class Parser
{
public:
	Parser();
	virtual ~Parser();
	void progressFile(const string& dest) throw(...);
	void loadMaterial(const string& dest) throw(...);
	
	/*getters*/
	int getNrOfTriangleVertices()			const;
	string getMtllib()						const;
	string getGeometry()					const;
	string getUseMtl()						const;
	string getImageFile()					const;

	Vector3 getVertex(const int &nr)		const;
	void createList();
	TriangleVertex popFirst();

private:
	string imageFile;
	string mtllib;
	string geometry;
	string usemtl;
	bool finished;
	Material material;

	vector<Vector3>verticies;
	vector<Vector3>vertexNormals;
	list<triangleData>triVertex;
	list<TriangleVertex>finalVertexes;
	vector<Vector2>UVtext;

	void loadDataIntoTriangleData(const string& triangleDesc);
};

#endif