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

	this->connectedPoints.resize(this->TerrainWidth+1);
	for (int i = 0; i < this->connectedPoints.size(); i++)
	{
		this->connectedPoints[i].resize(this->TerrainLength+1);
	}

	for (int i = 0; i < this->TerrainWidth; i++)
	{
		for (int j = 0; j < this->TerrainLength; j++)
		{
			input.x = i + 0;
			input.z = j + 0;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.x = i + 0;
			input.z = j + 1;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.x = i + 1;
			input.z = j + 1;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.x = i + 0;
			input.z = j + 0;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

			input.x = i + 1;
			input.z = j + 1;
			this->vertecies.push_back(input);
			this->connectedPoints[input.x][input.z].push_back(index++);

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

		this->vertecies[i+1].nx = XMVectorGetX(normal);
		this->vertecies[i+1].ny = XMVectorGetY(normal);
		this->vertecies[i+1].nz = XMVectorGetZ(normal);

		this->vertecies[i+2].nx = XMVectorGetX(normal);
		this->vertecies[i+2].ny = XMVectorGetY(normal);
		this->vertecies[i+2].nz = XMVectorGetZ(normal);

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
				this->vertecies[this->connectedPoints[i][j][k]].y = this->hminfo.heightMap[i+j].y;
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
	return XMVector3Cross(XMVectorSubtract(v2, v1), XMVectorSubtract(v3, v1));
}

std::vector<TriangleVertex> Terrain::getVertecies() const
{
	return this->vertecies;
}

float Terrain::getY(int x, int z) const
{
	return this->vertecies[this->connectedPoints[x][z][0]].y;
}

int Terrain::getWidth() const
{
	return this->connectedPoints.size();
}

int Terrain::getLength() const
{
	return this->connectedPoints[0].size();
}
