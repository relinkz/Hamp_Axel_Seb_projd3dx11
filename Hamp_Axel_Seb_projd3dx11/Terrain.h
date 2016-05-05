#pragma once
#include <vector>
#include "Parser.h"
#include "Object.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;


class Terrain
{
private:
	
	struct HeightMapInfo
	{
		int terrainWidth;
		int terrainLength;
		XMFLOAT3 *heightMap;
	} hminfo;

	int TerrainLength;
	int TerrainWidth;
	std::vector<TriangleVertex> vertecies;
	Object object;
	std::vector<std::vector<std::vector<int>>> connectedPoints;

	void setHeightmapToMap();
	void loadFromFile();
	XMVECTOR getNewNormal(int first, int second, int third);
public:
	Terrain(int width = 20, int length = 20);
	virtual ~Terrain();

	std::vector<TriangleVertex> getVertecies() const;
	float getY(int x, int z)const;
	int getWidth()const;
	int getLength()const;
};