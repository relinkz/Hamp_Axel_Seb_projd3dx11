#include <windows.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "Parser.h"
#include "Object.h"
#include "Camera.h"
#include "LightHandler.h"
#include "QuadTree.h"
#include "shadowShaderClass.h"

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT CreateDirect3DContext(HWND wndHandle);

IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11RenderTargetView *deferredViews[4];

ID3D11RenderTargetView* gBackbufferRTV = nullptr;
TriangleVertex* triangleVertices = nullptr;

TriangleVertex* quadTiangle;

//dephbuffer
ID3D11DepthStencilView* gDepthBuffer = nullptr;
ID3D11Texture2D* pDepthStencil = nullptr;

ID3D11Texture2D* NormalStencil = nullptr;
ID3D11RenderTargetView* NormalRTV = nullptr;

ID3D11Texture2D* ColorStencil = nullptr;
ID3D11RenderTargetView* ColorRTV = nullptr;

ID3D11Texture2D* PositionStencil = nullptr;
ID3D11RenderTargetView* PositionRTV = nullptr;

ID3D11Texture2D* LightDepthStencil = nullptr;
ID3D11RenderTargetView* LightDepthRTV = nullptr;

ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* quadVertexBuffer = nullptr;
ID3D11Buffer* gIndexBuffer = nullptr;
ID3D11Buffer* gGeometryBuffer = nullptr;
ID3D11Buffer* testBuffer = nullptr;
ID3D11Buffer* lightBuffer = nullptr;
ID3D11Buffer* cameraBuffer = nullptr;

ID3D11Buffer* worldSpaceBuffer = nullptr;
ID3D11Buffer* worldSpaceBuffer2 = nullptr;

ID3D11GeometryShader *gGeometryShader = nullptr;
ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11InputLayout* quadLayout = nullptr;

ID3D11VertexShader* quadVertexShader = nullptr;
ID3D11VertexShader* vDeferredShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;
ID3D11PixelShader* pDeferredShader = nullptr;

Camera WorldCamera;
Object worldObject;
ShadowShaderClass shadowMap;

struct vertex
{
	float x;
	float y;
	float z;
	float u;
	float v;
};

struct worldMatrixBuffer
{
	XMFLOAT4X4 worldViewProj;
	XMFLOAT4X4 eyeSpace;
	XMFLOAT4X4 lightViewMatrix;
	XMFLOAT4X4 lightProjectionMatrix;
};
struct worldMatrixBuffer2
{
	XMFLOAT4X4 worldViewProj;
	XMFLOAT4X4 eyeSpace;
	XMFLOAT4X4 lightViewMatrix;
	XMFLOAT4X4 lightProjectionMatrix;

	XMFLOAT4 cameraPos;
	XMFLOAT4 lightPos;
};
vertex finalShit[6];
vector<Object> objects;
vector<Object*> objectsToDraw;

int nrOfObjects = 0;

QuadTree quadTree(Vector3(0, 0, 0), Vector3(10, 0, 10), 1, 10, 0, 10, 0);

int nrOfVertexDrawn = 0;

LightHandler pointLight = LightHandler();


using namespace DirectX;
using namespace DirectX::SimpleMath;

struct newPosLight
{
	Vector3 pos;
	float pad;
	float ambient;
	float diffuse;
	float strength;
};

newPosLight light
{
	Vector3(0,0,-2),
	0.0f,
	0.2f,
	0.8f,
	5.0f
};

struct cameraData
{
	Vector4 pos;
};

//Matrices
SimpleMath::Matrix* viewSpace = nullptr;
SimpleMath::Matrix* projectionSpace = nullptr;
SimpleMath::Matrix* worldSpace = nullptr; // need one worldSpace for each object in the world
Vector3 cameraPos = Vector3(0, 0, 20);
SimpleMath::Matrix* worldViewProj = nullptr;
Matrix* lightViewMatrix = nullptr;
Matrix* lightProjectionMatrix = nullptr;
SimpleMath::Matrix* eyeSpace = nullptr;
SimpleMath::Matrix* orthograficProjection = nullptr;


void createWorldMatrices()
{
	Matrix* WVP_Ptr = nullptr;

	viewSpace = new Matrix(DirectX::XMMatrixLookAtLH
		(
			Vector3(-2, 0, 0),	//Position
			Vector3(1, 2, 1),	//lookAtTarget
			Vector3(0, 1, 0)	//upVector
			));

	//ProjectionMatrix
	 projectionSpace = new Matrix(XMMatrixPerspectiveFovLH
		 (
			 3.14f*0.45f,		// FOV
			 640.0f / 480.0f,	//Aspect Ratio
			 0.5f,				//near plane
			 20.0f				//far plane
			 ));


	 worldSpace = new Matrix(XMMatrixTranslation(1.0f, 1.0f, 1.0f));

	 worldViewProj = new Matrix((*worldSpace) * (*viewSpace) * (*projectionSpace));

	 
	 eyeSpace = new Matrix((*worldSpace)); 

	 lightViewMatrix = new Matrix(XMMatrixTranslation(light.pos.x, light.pos.y, light.pos.z));
	 lightProjectionMatrix = new Matrix(*worldViewProj);

	 lightViewMatrix = &lightViewMatrix->Transpose();
	 lightProjectionMatrix = &lightProjectionMatrix->Transpose();
	 worldViewProj = &worldViewProj->Transpose();
	 eyeSpace = &eyeSpace->Transpose();
	

	 worldMatrixBuffer buffer
	 {
		 *worldViewProj, *eyeSpace, *lightViewMatrix , *lightProjectionMatrix
	 };
	 worldMatrixBuffer2 buffer2
	 {
		 *worldViewProj, *eyeSpace, *lightViewMatrix , *lightProjectionMatrix, Vector4(0,10,0,0), Vector4(0,10,0,0)
	 };

	 //buffers
	 D3D11_BUFFER_DESC viewSpaceDesc;
	 viewSpaceDesc.Usage					= D3D11_USAGE_DYNAMIC;
	 viewSpaceDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	 viewSpaceDesc.ByteWidth				= sizeof(worldMatrixBuffer);
	 viewSpaceDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	 viewSpaceDesc.MiscFlags				= 0;
	 viewSpaceDesc.StructureByteStride		= 0;

	 D3D11_SUBRESOURCE_DATA testa;
	 testa.pSysMem = &buffer;

	HRESULT test = gDevice->CreateBuffer(&viewSpaceDesc, &testa, &worldSpaceBuffer);	//creates the worldSpaceBuffer

	D3D11_SUBRESOURCE_DATA testa2;
	testa2.pSysMem = &buffer2;

	test = gDevice->CreateBuffer(&viewSpaceDesc, &testa2, &worldSpaceBuffer2);	//creates the worldSpaceBuffer

	cameraData cameraBufferData
	{
		WorldCamera.getCameraPos()
	};

	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.ByteWidth = sizeof(cameraData);
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &cameraBufferData;

	test = gDevice->CreateBuffer(&cameraBufferDesc, &data, &cameraBuffer); //creates the CameraBuffer
}

void CreateShaders()
{
	//CREATE ALL THE SHADERS
	HRESULT hr;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob* errorBlob = nullptr;
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"DeferredVertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_4_0",		// shader model (target)
		D3DCOMPILE_DEBUG,				// shader compile options
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vDeferredShader);
	

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);

	/*
	Createing the next vertexShader for quad
	*/

	ID3DBlob* pVS2 = nullptr;
	D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"vs_4_0",		// shader model (target)
		D3DCOMPILE_DEBUG,				// shader compile options
		0,				// effect compile options
		&pVS2,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);
	hr = gDevice->CreateVertexShader(pVS2->GetBufferPointer(), pVS2->GetBufferSize(), nullptr, &quadVertexShader);

	D3D11_INPUT_ELEMENT_DESC inputDescQuad[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = gDevice->CreateInputLayout(inputDescQuad, ARRAYSIZE(inputDescQuad), pVS2->GetBufferPointer(), pVS2->GetBufferSize(), &quadLayout);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();
	pVS2->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"DeferredPixel.hlsl", // filename		//L"PixelShader.hlsl"
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"ps_4_0",		// shader model (target)
		D3DCOMPILE_DEBUG,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	//create pixel shader
	ID3DBlob* pPS2 = nullptr;
	D3DCompileFromFile(
		L"PixelShader.hlsl", // filename		//L"PixelShader.hlsl"
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"ps_4_0",		// shader model (target)
		D3DCOMPILE_DEBUG,				// shader compile options
		0,				// effect compile options
		&pPS2,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	//create geometry shader
	ID3DBlob *pGS = nullptr;
	D3DCompileFromFile(
		L"GeometryShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_4_0",
		D3DCOMPILE_DEBUG,
		0,
		&pGS,
		&errorBlob
		);
	string error;

	

	if (errorBlob)
	{
		//OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		//error = (char*)errorBlob->GetBufferPointer();
	}

	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);

	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pDeferredShader);
	gDevice->CreatePixelShader(pPS2->GetBufferPointer(), pPS2->GetBufferSize(), nullptr, &gPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();
}

void createObjects()
{
#pragma region

	Parser fromFile;
	fromFile.progressFile("obj2.txt");
	fromFile.loadMaterial("box.mtl");
	int nrOfVert = 0;
	int counter = 0;
	vector<TriangleVertex> triangleVertices;

	fromFile.createList();
	nrOfVert = fromFile.getNrOfTriangleVertices();

	for (int i = 0; i < nrOfVert; i++)
	{
		//openGL
		triangleVertices.push_back(fromFile.popFirst());
	}
	//Convert
	for (int i = 0; i < nrOfVert; i++)
	{
		if (counter == 1)
		{
			TriangleVertex temp = triangleVertices[i];

			triangleVertices[i] = triangleVertices[i + 1];
			triangleVertices[i + 1] = temp;
		}
		else if (counter == 2)
			counter = -1;

		counter++;
	}

	nrOfVertexDrawn = triangleVertices.size();

	//creates the quad for DeferredRendering
	finalShit[0] = { -1.0f,  1.0f, 0.0f, 0.0f,0.0f };
	finalShit[1] = {  1.0f, -1.0f, 0.0f, 1.0f,1.0f };
	finalShit[2] = { -1.0f, -1.0f, 0.0f, 0.0f,1.0f };
	//				  1.0	 1.0
	finalShit[3] = { -1.0f,  1.0f, 0.0f, 0.0f,0.0f };
	finalShit[4] = {  1.0f,  1.0f, 0.0f, 1.0f,0.0f };
	finalShit[5] = {  1.0f, -1.0f, 0.0f, 1.0f,1.0f };
	//creates the vertexBuffer for the quad
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	bufferDesc.ByteWidth = sizeof(finalShit);
	
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = finalShit;

	HRESULT hr = gDevice->CreateBuffer(&bufferDesc, &data, &quadVertexBuffer);
	//--------


	//creates all the Objects
	/*int xMax = 6;
	int yMax = 3;
	int zMax = 6;
	for (int x = 0; x < xMax; x++)
	{
		for (int y = 0; y < yMax; y++)
		{
			for (int z = 0; z < zMax; z++)
			{
				if (x == 0 || x == xMax - 1 || y == 0 || y == yMax - 1 || z == 0 || z == zMax - 1)
				{
					//creates an object
					objects.push_back(Object(triangleVertices, Vector3((2.0f * x), (2.0f * y), (2.0f * z)), gDevice, fromFile.getImageFile(), "cube_box_NormalMap.png"));
					nrOfObjects++;
				}
			}
		}
	}*/
	objects.push_back(Object(triangleVertices, Vector3((1.0f), (0.0f), (1.0f)), gDevice, fromFile.getImageFile(), "cube_box_NormalMap.png"));
	objects.push_back(Object(triangleVertices, Vector3((3.0f), (0.0f), (1.0f)), gDevice, fromFile.getImageFile(), "cube_box_NormalMap.png"));

	//creating the floor
	for (int i = 0; i < 10; i++)
	{
		float x = i;
		for (int j = 0; j < 10; j++)
		{
			float y = j;
			objects.push_back(Object(triangleVertices, Vector3((x), (-2.0f), (y)), gDevice, fromFile.getImageFile(), "cube_box_NormalMap.png"));
		}
	}

	quadTree.setTreeData(objects); //splits all the objects into the quads in the quadtree
#pragma endregion

	
}

void SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)640;
	vp.Height = (float)480;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}

void UpdateCameraBuffer()
{
	HRESULT hr;
	cameraData newCameraData //set the cameraData for the camera Buffer
	{
		/*Vector4(WorldCamera.getCameraPos().x,
		WorldCamera.getCameraPos().y,
			WorldCamera.getCameraPos().z,
			0.0f)*/
		WorldCamera.getCameraPos()
	};
	//update the Camera Buffer
	D3D11_MAPPED_SUBRESOURCE oldCameraData;
	memset(&oldCameraData, 0, sizeof(oldCameraData));

	hr = gDeviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oldCameraData);

	memcpy(oldCameraData.pData, &newCameraData, sizeof(cameraData));

	gDeviceContext->Unmap(cameraBuffer, 0);
	//-----
}

void Render(Object object1)
{

	*worldSpace = object1.getWorldMatrix();		//get the worldMatrix of the object that we will render

	*viewSpace = WorldCamera.getViewMatrix();	//get the viewMatrix from the camera
	*worldViewProj = Matrix((*worldSpace) * (*viewSpace) * (*projectionSpace)); //create the WVP(WorldViewProjection) Matrix

	worldViewProj = &worldViewProj->Transpose(); //transpose the WVP matrix

	worldSpace = &worldSpace->Transpose();		//transpose the world Matrix

	worldMatrixBuffer updateWorldMatrices // set the MatrixBuffer
	{
		*worldViewProj, *worldSpace, *lightViewMatrix , *lightProjectionMatrix
	};
	//update the worldMatrixBuffer
	Matrix* WVP_Ptr = nullptr;
	D3D11_MAPPED_SUBRESOURCE viewSpaceData;
	memset(&viewSpaceData, 0, sizeof(viewSpaceData));


	HRESULT test = gDeviceContext->Map(worldSpaceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &viewSpaceData);
	
	memcpy(viewSpaceData.pData, &updateWorldMatrices, sizeof(updateWorldMatrices));

	gDeviceContext->Unmap(worldSpaceBuffer, 0);
	//----------

	UpdateCameraBuffer();

	ID3D11ShaderResourceView* diffuseSRV = object1.getDiffuseMapSRV();	//get the DiffuseMap from the object
	gDeviceContext->PSSetShaderResources(0, 1, &diffuseSRV);			//send the DiffuseMap to the DeferredPixelShader
	ID3D11ShaderResourceView* normalMapSRV = object1.getNormalMapSRV(); //get the NormalMap from the object
	gDeviceContext->PSSetShaderResources(1, 1, &normalMapSRV);			//send the NormalMap to the DeferredPixelShader

	gDeviceContext->GSSetConstantBuffers(0, 1, &cameraBuffer);			//send the cameraBuffer to the GeometryShader

	

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;
	gVertexBuffer = object1.getVertexBufferPointer();	//get the vertexBuffer from the object

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);	//set the vertexBuffer
	gDeviceContext->VSSetConstantBuffers(0, 1, &worldSpaceBuffer);	//set the constant buffer for the DeferredVertexShader


	gDeviceContext->Draw(nrOfVertexDrawn,0);	//render the object
}

void FirstRenderCall()
{
	//THIS IS THE START OF THE FIRST RENDERCALL
	HRESULT hr;
	float clearColor[] = { 0, 0, 0, 1 };
	//float whiteColor[] = { 1, 1, 1, 1 };
	//float grayColor[] = { 0.5, 0.5, 0.5 , 1 };

	//clear all the RTV
	gDeviceContext->ClearRenderTargetView(deferredViews[0], clearColor);
	gDeviceContext->ClearRenderTargetView(deferredViews[1], clearColor);
	gDeviceContext->ClearRenderTargetView(deferredViews[2], clearColor);
	gDeviceContext->ClearRenderTargetView(deferredViews[3], clearColor);
	//clear depthbuffer
	gDeviceContext->ClearDepthStencilView(gDepthBuffer, D3D11_CLEAR_DEPTH, 1, 0);

	//get all the object in the part of the quad tree that the camera is in (must be used)
	objectsToDraw = quadTree.getObjectsToDraw(WorldCamera.getCameraPos()); 

	//does viewfustrum culling on the objects gathered from the quadtree (does not have to be used)
	//objectsToDraw = WorldCamera.doFustrumCulling(objectsToDraw);

	//set the OutputMerger to use the 4 RTV
	gDeviceContext->OMSetRenderTargets(4, deferredViews, gDepthBuffer);

	//enable the shaders
	gDeviceContext->VSSetShader(vDeferredShader, NULL, 0);
	gDeviceContext->GSSetShader(gGeometryShader, NULL, 0);
	gDeviceContext->PSSetShader(pDeferredShader, NULL, 0);

	//gDeviceContext->PSSetConstantBuffers(0, 1, &worldSpaceBuffer);					//give the worldSpaceBuffer to the PixelShader
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//set the topology
	gDeviceContext->IASetInputLayout(gVertexLayout);								//set the input layout


	//renders all the individual objects and stores their values in the RTVs
	for (int i = 0; i < objectsToDraw.size(); i++)
	{
		if (objectsToDraw.at(i) != nullptr)
		{
			Render(*objectsToDraw.at(i));
		}
	}
	//FIRST RENDER CALL DONE
}

void SecondRenderCall()
{
	HRESULT hr;
	//THIS IS THE START OF THE SECOND REDENR CALL
	//sets the OutputMerger to use the RTV that is linked to the backbuffer
	gDeviceContext->OMSetRenderTargets(1, &deferredViews[0], gDepthBuffer);

	//enable the shaders taht will be used (no Geometry shader here)
	gDeviceContext->VSSetShader(quadVertexShader, NULL, 0);
	gDeviceContext->PSSetShader(gPixelShader, NULL, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);

	ID3D11ShaderResourceView* Position = nullptr;
	ID3D11ShaderResourceView* Normal = nullptr;
	ID3D11ShaderResourceView* Color = nullptr;
	ID3D11ShaderResourceView* shadowMapSRV = nullptr;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	gDeviceContext->PSSetConstantBuffers(0, 1, &worldSpaceBuffer2);


	UpdateCameraBuffer(); //update the CameraBuffer

	gDeviceContext->PSSetConstantBuffers(1, 1, &cameraBuffer);

	ID3D11Buffer* shadowCB = shadowMap.getLightBuffer();
	gDeviceContext->PSSetConstantBuffers(2, 1, &shadowCB);


	hr = gDevice->CreateShaderResourceView(PositionStencil, &SRVDesc, &Position);	//turn the RTV that stores the Positions into a 
	gDeviceContext->PSSetShaderResources(0, 1, &Position);							//ShaderResouresView and send it to the PixelShader

	hr = gDevice->CreateShaderResourceView(NormalStencil, &SRVDesc, &Normal);		//turn the RTV that stores the Normals into a 
	gDeviceContext->PSSetShaderResources(1, 1, &Normal);							//ShaderResouresView and send it to the PixelShader

	hr = gDevice->CreateShaderResourceView(ColorStencil, &SRVDesc, &Color);			//turn the RTV that stores the Colors into a 
	gDeviceContext->PSSetShaderResources(2, 1, &Color);								//ShaderResouresView and send it to the PixelShader

	shadowMapSRV = shadowMap.getShaderResourceView();
	gDeviceContext->PSSetShaderResources(3, 1, &shadowMapSRV);


	UINT32 vertexSize = sizeof(float) * 5;	//set the size of a vertex sizeof(float) * 5 the positions(x,y,z) and the UV coords(U,V) 
	UINT32 offset = 0; // offset 0


	gDeviceContext->IASetVertexBuffers(0, 1, &quadVertexBuffer, &vertexSize, &offset);	//set the new Vertexbuffer with the quad 

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		//set the topology
	gDeviceContext->IASetInputLayout(quadLayout);										//set inputlayout

	gDeviceContext->Draw(6, 0); //second RenderCall renders the 6 vertexes that makeup the quad
	//SECOND RENDERCALL DONE
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster

	if (wndHandle)
	{
		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context


		SetViewport(); //3. Sätt viewport

		CreateShaders(); //4. Skapa vertex- och pixel-shaders

		createWorldMatrices(); //initierar världsmatriserna här, vid frågor och klagomål, så ansvarar jag inte för detta!

		createObjects(); //5. Definiera triangelvertiser, 6. Skapa vertex buffer, 7. Skapa input layout

		ShowWindow(wndHandle, nCmdShow);


		try
		{
			shadowMap.initialize(gDevice, wndHandle, Vector3(0,10,0), Vector3(1,-1,0), Vector3(0,1,0));
		}
		catch (char* error)
		{
			MessageBox(wndHandle, L"shadowMap error", L"Read the error", MB_OK);
			system("PAUSE");
		}

		while (WM_QUIT != msg.message)
		{
			WorldCamera.Update(wndHandle); //update worldcamera
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{

				FirstRenderCall(); //DO THE FIRST RENDER CALL THAT RENDERS ALL THE OBJECTS AND STORES THEIR VALUES IN THE RTVs

				for (int i = 0; i < objectsToDraw.size(); i++)
				{
					if (objectsToDraw.at(i) != nullptr)
					{
						//updateBuffers(objectsToDraw.at(i));
						//RenderShadowMap(*objectsToDraw.at(i));
						shadowMap.updateBuffer(*objectsToDraw.at(i), gDeviceContext);
						shadowMap.render(*objectsToDraw.at(i), nrOfVertexDrawn, gDevice, gDeviceContext);
					}
				}

				SecondRenderCall(); //DO THE SECOND RENDERCALL THAT RENDERS TO A QUAD


				gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
				shadowMap.clearDepthBuffer(gDeviceContext);
			}
		}

		gVertexBuffer->Release();

		gVertexLayout->Release();
		vDeferredShader->Release();
		gPixelShader->Release();

		//gBackbufferRTV->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();
		DestroyWindow(wndHandle);
	}

	return (int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_D3D_DEMO",
		L"BTH Direct3D Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT CreateDirect3DContext(HWND wndHandle)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

															// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	//hr = gDevice->CreateDeferredContext(0, &gDeviceContext); //I have no clue what I am doing

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		//create dephbuffer
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = 640.0f;
		descDepth.Height = 480.0f;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 4;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 4;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		//creates the Descrition for the Texture2D used when creating the Textures used in DeferredRendering 
		D3D11_TEXTURE2D_DESC descDepth2;
		descDepth2.Width = 640.0f;
		descDepth2.Height = 480.0f;
		descDepth2.MipLevels = 1;
		descDepth2.ArraySize = 1;
		descDepth2.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		descDepth2.SampleDesc.Count = 4;
		descDepth2.SampleDesc.Quality = 0;
		descDepth2.Usage = D3D11_USAGE_DEFAULT;
		descDepth2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //must be used as RenderTargetViews and ShaderResources
		descDepth2.CPUAccessFlags = 0;
		descDepth2.MiscFlags = 0;

		//ZeroMemory(&descDepth2, sizeof(descDepth2));

		ID3D11Resource*	textureData;

		hr = gDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);	//creates the DepthStencil Texture2D
		hr = gDevice->CreateTexture2D(&descDepth2, NULL, &PositionStencil);	//creates the Position Texture2D used in DeferredRendering
		hr = gDevice->CreateTexture2D(&descDepth2, NULL, &NormalStencil);	//creates the Normal Texture2D used in DeferredRendering
		hr = gDevice->CreateTexture2D(&descDepth2, NULL, &ColorStencil);	//creates the Color Texture2D used in DeferredRendering


		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		memset(&descDSV, 0, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; //TEXTURE2DMS
		descDSV.Texture2D.MipSlice = 0;

		// use the back buffer address to create the render target
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		desc.Texture2D.MipSlice = 0;

		hr = gDevice->CreateRenderTargetView(pBackBuffer, NULL, &deferredViews[0]);			//connects the Textured2D with a RTV deferredView[0] = backbuffer
		hr = gDevice->CreateRenderTargetView(PositionStencil, &desc, &deferredViews[1]);	//connects the Textured2D with a RTV deferredView[1] = PositionTexture
		hr = gDevice->CreateRenderTargetView(NormalStencil, &desc, &deferredViews[2]);		//connects the Textured2D with a RTV deferredView[2] = NormalTexture
		hr = gDevice->CreateRenderTargetView(ColorStencil, &desc, &deferredViews[3]);		//connects the Textured2D with a RTV deferredView[3] = ColorTexture

		pBackBuffer->Release();

		hr = gDevice->CreateDepthStencilView
			(
				pDepthStencil, // Depth stencil texture
				&descDSV, // Depth stencil desc
				&gDepthBuffer
			);		// [out] Depth stencil view
						// set the render target as the back buffer
	
	}

	return hr;
}