#include "Map.h"



Map::Map()
{
	/*this->wallPtr = nullptr;
	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			this->MakeSet(&nodes[x][y]);
		}
	}
	this->ProjectionMatrix = Matrix(XMMatrixPerspectiveFovLH
		(
			3.14f*0.45f,		// FOV
			640.0f / 480.0f,	//Aspect Ratio
			0.5f,				//near plane
			20.0f				//far plane
			));*/
}

Map::Map(Object * wallPtr)
{
	/*this->wallPtr = wallPtr;
	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			this->MakeSet(&nodes[x][y]);
		}
	}
	this->ProjectionMatrix = Matrix(XMMatrixPerspectiveFovLH
		(
			3.14f*0.45f,		// FOV
			640.0f / 480.0f,	//Aspect Ratio
			0.5f,				//near plane
			20.0f				//far plane
			));*/
}

Map::~Map()
{
}

void Map::Render(ID3D11DeviceContext* gDeviceContext, Camera* camera, ID3D11Buffer* worldSpaceBuffer)
{
	/*for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			Vector3 pos = this->wallPtr->getPosition();
			Matrix viewSpace = camera->getViewMatrix();
			if (this->nodes[x][y].cLeft == false)
			{
				this->renderWall(gDeviceContext, camera, worldSpaceBuffer, Vector3(x - 0.5f, 0.0f, y), Vector3(0.2f, 1.0f, 1.0f));


			}
			if (this->nodes[x][y].cUp == false)
			{
				this->renderWall(gDeviceContext, camera, worldSpaceBuffer, Vector3(x, 0.0f, y +0.5f), Vector3(1.0f, 1.0f, 0.2f));

			}
		}
	}*/
}

void Map::makeLabyrinth(Node** nodes, int width, int height)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			MakeSet(&nodes[x][y]);
		}
	}
	int lenght = width*height;
	int* arr = new int[lenght];
	int nr = 0;
	for (int i = 0; i < lenght; i++)
	{
		arr[i] = nr;
		nr++;
	}
	arr[0] = 0;
	int nrOfelements = lenght;
	for (int i = 0; i < lenght; i++)
	{
		int index = rand() % nrOfelements;
		int value = arr[index];

		int column;
		int row;

		column = value / height;
		row = value % height;
		if (column >= 1)
		{
			makeConnection(nodes, column, row, 0);
		}
		if (column < width - 1)
		{
			makeConnection(nodes, column, row, 1);
		}
		if (row >= 1)
		{
			makeConnection(nodes, column, row, 2);
		}
		if (row < height - 1)
		{
			makeConnection(nodes, column, row, 3);
		}


		arr[index] = arr[nrOfelements - 1];
		nrOfelements--;
	}

}

bool Map::addConnection(int x, int y)
{
	int direction = rand() % 4;

	bool result = false;
	//makeConnection(x, y, direction);

	return result;
}

bool Map::makeConnection(Node** nodes, int x, int y, int direction)
{
	bool result = false;
	switch (direction)
	{
	case(0) : //left
		result = Union(&nodes[x][y], &nodes[x - 1][y]);
		if (result)
		{
			nodes[x][y].cLeft = true;
		}
		break;
	case(1) : //right
		result = Union(&nodes[x][y], &nodes[x + 1][y]);
		if (result)
		{
			nodes[x + 1][y].cLeft = true;
		}
		break;
	case(2) : //up
		result = Union(&nodes[x][y], &nodes[x][y - 1]);
		if (result)
		{
			nodes[x][y].cUp = true;
		}
		break;
	case(3) ://down
		result = Union(&nodes[x][y], &nodes[x][y + 1]);
		if (result)
		{
			nodes[x][y + 1].cUp = true;
		}
		break;
	}
	return result;
}


//helpers
void Map::MakeSet(Node * node)
{
	node->parent = nullptr;
	node->cLeft = false;
	node->cUp = false;
}

Node * Map::FindSet(Node * node)
{
	if (node->parent == nullptr)
	{
		return node;
	}
	else
	{
		return node->parent = FindSet(node->parent);
	}
}

bool Map::Union(Node * node1, Node * node2)
{
	bool result = false;
	if (FindSet(node1) != FindSet(node2))
	{
		result = true;

		FindSet(node1)->parent = FindSet(node2);

	}

	return result;
}

void Map::renderWall(ID3D11DeviceContext * gDeviceContext, Camera * camera, ID3D11Buffer * worldSpaceBuffer, Vector3 pos, Vector3 scale)
{
	Matrix viewSpace = camera->getViewMatrix();

	Matrix m = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	Matrix worldSpace = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	worldSpace = m * worldSpace;

	Matrix WVP = m * worldSpace * viewSpace * this->ProjectionMatrix;

	this->updateWorldSpaceBuffer(gDeviceContext, worldSpaceBuffer, &worldSpace, &WVP);
	this->updateSRV(gDeviceContext);

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;
	//ID3D11Buffer* gVertexBuffer = this->wallPtr->getVertexBufferPointer();	//get the vertexBuffer from the object

	//gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);	//set the vertexBuffer
	gDeviceContext->VSSetConstantBuffers(0, 1, &worldSpaceBuffer);	//set the constant buffer for the DeferredVertexShader

	gDeviceContext->Draw(36, 0);
}

void Map::updateWorldSpaceBuffer(ID3D11DeviceContext* gDeviceContext, ID3D11Buffer * worldSpaceBuffer, Matrix * worldSpace, Matrix * WVP)
{
	worldSpace = &worldSpace->Transpose();
	WVP = &WVP->Transpose();
	worldMatrixBuffer updateWorldMatrices // set the MatrixBuffer
	{
		*WVP, *worldSpace, Matrix() , Matrix()
	};
	//update the worldMatrixBuffer
	Matrix* WVP_Ptr = nullptr;
	D3D11_MAPPED_SUBRESOURCE viewSpaceData;
	memset(&viewSpaceData, 0, sizeof(viewSpaceData));


	HRESULT test = gDeviceContext->Map(worldSpaceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &viewSpaceData);

	memcpy(viewSpaceData.pData, &updateWorldMatrices, sizeof(updateWorldMatrices));

	gDeviceContext->Unmap(worldSpaceBuffer, 0);
}

void Map::updateSRV(ID3D11DeviceContext * gDeviceContext)
{
	/*ID3D11ShaderResourceView* diffuseSRV = this->wallPtr->getDiffuseMapSRV();
	gDeviceContext->PSSetShaderResources(0, 1, &diffuseSRV);
	ID3D11ShaderResourceView* normalMapSRV = this->wallPtr->getNormalMapSRV();
	gDeviceContext->PSSetShaderResources(1, 1, &normalMapSRV);*/
}
