#include "Terrain.h"

#include <iostream>
using namespace std;

Terrain::Terrain(int width, int length)
{
	this->TerrainLength = length;
	this->TerrainWidth = width;

	TriangleVertex input;
	input.x = 0;
	input.y = 0;
	input.z = 0;

	input.nx = 0;
	input.ny = 1;
	input.nz = 0;

	input.u = 0;
	input.v = 0;

	int index = 0;

	this->connectedPoints.resize(this->TerrainWidth);
	for (int i = 0; i < this->connectedPoints.size(); i++)
	{
		this->connectedPoints[i].resize(this->TerrainLength);
	}

	for (int i = 0; i < this->TerrainWidth - 1; i++)
	{
		for (int j = 0; j < this->TerrainLength - 1; j++)
		{

			input.u = 0;
			input.v = 0;

			input.x = i + 0;
			input.z = j + 0;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.u = 0;
			input.v = 1;

			input.x = i + 0;
			input.z = j + 1;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.u = 1;
			input.v = 1;

			input.x = i + 1;
			input.z = j + 1;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.u = 0;
			input.v = 0;

			input.x = i + 0;
			input.z = j + 0;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.u = 1;
			input.v = 1;

			input.x = i + 1;
			input.z = j + 1;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.u = 1;
			input.v = 0;

			input.x = i + 1;
			input.z = j + 0;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

		}
	}

	XMVECTOR normal;

	this->loadFromFile();
	this->setHeightmapToMap();
	for (int i = 0; i < this->vertecies.size(); i += 3)
	{
		normal = this->getNewNormal(i, i + 1, i + 2);
		this->vertecies[i].nx = XMVectorGetX(normal);
		this->vertecies[i].ny = XMVectorGetY(normal);
		this->vertecies[i].nz = XMVectorGetZ(normal);

		this->vertecies[i + 1].nx = XMVectorGetX(normal);
		this->vertecies[i + 1].ny = XMVectorGetY(normal);
		this->vertecies[i + 1].nz = XMVectorGetZ(normal);

		this->vertecies[i + 2].nx = XMVectorGetX(normal);
		this->vertecies[i + 2].ny = XMVectorGetY(normal);
		this->vertecies[i + 2].nz = XMVectorGetZ(normal);

	}
}


Terrain::~Terrain()
{

}

void Terrain::setHeightmapToMap()
{
	for (int i = 0; i < this->TerrainWidth; i++)
	{
		for (int j = 0; j < this->TerrainLength; j++)
		{
			for (int k = 0; k < this->connectedPoints[i][j].size(); k++)
			{
				this->vertecies[this->connectedPoints[i][j][k]].y = this->hminfo.heightMap[i + j].y;
			}
		}
	}

}

void Terrain::loadFromFile()
{
	FILE *filePtr;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize = 0;
	int index = 0;
	unsigned char height;


	filePtr = fopen("heightMap3.bmp", "rb");
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);


	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainLength = bitmapInfoHeader.biHeight;

	imageSize = hminfo.terrainWidth * hminfo.terrainLength * 3;

	unsigned char* bitmapImage = new unsigned char[imageSize];

	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	fread(bitmapImage, 1, imageSize, filePtr);


	fclose(filePtr);

	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainLength];

	int k = 0;

	float heightFactor = 10.0f;

	for (int i = 0; i < hminfo.terrainWidth; i++)
	{
		for (int j = 0; j < hminfo.terrainLength; j++)
		{
			height = bitmapImage[k];

			index = (hminfo.terrainWidth * i) + j;

			hminfo.heightMap[index].x = (float)j;
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)i;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;


}

XMVECTOR Terrain::getNewNormal(int first, int second, int third)
{
	XMVECTOR v1 = XMLoadFloat3(&XMFLOAT3(this->vertecies[first].x, this->vertecies[first].y, this->vertecies[first].z));
	XMVECTOR v2 = XMLoadFloat3(&XMFLOAT3(this->vertecies[second].x, this->vertecies[second].y, this->vertecies[second].z));
	XMVECTOR v3 = XMLoadFloat3(&XMFLOAT3(this->vertecies[third].x, this->vertecies[third].y, this->vertecies[third].z));
	

	XMVECTOR result = XMVector3Cross(XMVectorSubtract(v2, v1), XMVectorSubtract(v3, v1));
	result = XMVector3Normalize(result);
	
	return result;
}

std::vector<TriangleVertex> Terrain::getVertecies() const
{
	return this->vertecies;
}

float Terrain::getY(int x, int z) const
{
	if (x < 0)
	{
		x = 0;
	}
	if (x >= this->TerrainWidth)
	{
		x = this->TerrainWidth - 1;
	}
	if (z < 0)
	{
		z = 0;
	}
	if (z >= this->TerrainWidth)
	{
		z = this->TerrainLength - 1;
	}
	return this->vertecies[this->connectedPoints[x][z][0]].y;
}

float Terrain::getY(float x, float z) const
{
	

	int intX = (int)floorf(x);
	int intZ = (int)floorf(z);

	if (intX < 0)
	{
		intX = 0;
	}
	if (intZ < 0)
	{
		intZ = 0;
	}

	if (intX > TerrainWidth - 2)
	{
		intX = TerrainWidth - 2;
	}
	if (intZ > TerrainLength - 2)
	{
		intZ = TerrainLength - 2;
	}



	float y0 = this->getY(intX, intZ);
	float y1 = this->getY(intX, intZ + 1);
	float y2 = this->getY(intX + 1, intZ);
	float y3 = this->getY(intX + 1, intZ + 1);

	XMVECTOR v1 = XMLoadFloat3(&XMFLOAT3(intX, y0, intZ));
	XMVECTOR v2 = XMLoadFloat3(&XMFLOAT3(intX, y1, intZ + 1));
	XMVECTOR v3 = XMLoadFloat3(&XMFLOAT3(intX + 1, y2, intZ));
	XMVECTOR v4 = XMLoadFloat3(&XMFLOAT3(intX + 1, y3, intZ + 1));

	XMVECTOR norm;



	if (x - intX < z - intZ)//vänster triangel
	{
		norm = XMVector3Cross(XMVectorSubtract(v1, v2), XMVectorSubtract(v4, v2));
	}
	else//höger triangel
	{
		norm = XMVector3Cross(XMVectorSubtract(v1, v3), XMVectorSubtract(v4, v3));
	}

	float d = -(XMVectorGetX(norm) * XMVectorGetX(v1) + XMVectorGetY(norm) * XMVectorGetY(v1) + XMVectorGetZ(norm) * XMVectorGetZ(v1));
	float newY = (x * XMVectorGetX(norm) + z * XMVectorGetZ(norm) + d) / -XMVectorGetY(norm);


	return newY;
}

int Terrain::getWidth() const
{
	return this->connectedPoints.size();
}

int Terrain::getLength() const
{
	return this->connectedPoints[0].size();
}
