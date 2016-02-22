#include <windows.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "Parser.h"
#include "Camera.h"


HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT CreateDirect3DContext(HWND wndHandle);

IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11RenderTargetView* gBackbufferRTV = nullptr;
TriangleVertex* triangleVertices = nullptr;

//dephbuffer
ID3D11DepthStencilView* gDepthBuffer = nullptr;
ID3D11Texture2D* pDepthStencil = nullptr;

ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* gIndexBuffer = nullptr;

ID3D11Buffer* worldSpaceBuffer = nullptr;

ID3D11GeometryShader *gGeometryShader = nullptr;
ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11VertexShader* gVertexShader = nullptr;
ID3D11PixelShader* gPixelShader = nullptr;
Camera WorldCamera;
int nrOfVertexDrawn = 0;


using namespace DirectX;
using namespace DirectX::SimpleMath;

//Matrices
SimpleMath::Matrix* viewSpace;
SimpleMath::Matrix* projectionSpace;
SimpleMath::Matrix* worldSpace; // need one worldSpace for each object in the world
Vector3 cameraPos = Vector3(0, 0, -2);
SimpleMath::Matrix* worldViewProj = nullptr;

void createWorldMatrices()
{
	Matrix* WVP_Ptr = nullptr;
	//ViewSpace
	//viewSpace = &WorldCamera.getViewMatrix();

	viewSpace = new Matrix(DirectX::XMMatrixLookAtLH
		(
			Vector3(2, 0, -2),	//Position
			Vector3(0, 0, 0),	//lookAtTarget
			Vector3(0, 1, 0)	//upVector
			));

	//viewSpace = &WorldCamera.getViewMatrix();
	//ProjectionMatrix
	 projectionSpace = new Matrix(XMMatrixPerspectiveFovLH
		 (
			 3.14f*0.45f,		// FOV
			 640.0f / 480.0f,	//Aspect Ratio
			 0.5f,				//near plane
			 20.0f				//far plane
			 ));

	 worldSpace = new Matrix();

	 worldViewProj = new Matrix((*worldSpace) * (*viewSpace) * (*projectionSpace));
	 worldViewProj = &worldViewProj->Transpose();

	 //buffers
	 D3D11_BUFFER_DESC viewSpaceDesc;
	 memset(&viewSpaceDesc, 0, sizeof(viewSpace));
	 viewSpaceDesc.Usage					= D3D11_USAGE_DYNAMIC;
	 viewSpaceDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	 viewSpaceDesc.ByteWidth				= sizeof(Matrix);
	 viewSpaceDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	 viewSpaceDesc.MiscFlags				= 0;
	 viewSpaceDesc.StructureByteStride		= 0;

	 D3D11_SUBRESOURCE_DATA testa;
	 testa.pSysMem = worldViewProj;

	 gDevice->CreateBuffer(&viewSpaceDesc, &testa, &worldSpaceBuffer);

	 //D3D11_MAPPED_SUBRESOURCE viewSpaceData;
	 //memset(&viewSpaceData, 0, sizeof(viewSpace));

	 
	// HRESULT test = gDeviceContext->Map(worldSpaceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &viewSpaceData);

	 //WVP_Ptr = (Matrix*)viewSpaceData.pData;
	 //WVP_Ptr = (&worldViewProj->Transpose());
	 //unsigned int nrOfBuffers = 1; 
	
	// gDeviceContext->Unmap(worldSpaceBuffer, 0);
	
	 
	 // gDeviceContext->Unmap(worldSpaceBuffer, 0);

	 //gDevice->CreateMappedBuffer(&viewSpaceDesc, &viewSpaceData, &worldSpaceBuffer)

	 
	
	 

	

	 //WVP_Ptr = (Matrix*)viewSpaceData.pData;
	
}
void CreateShaders()
{
	//create vertex shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"PixelShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"ps_4_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
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
		0,
		0,
		&pGS,
		nullptr
		);

	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);

	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();
}

void CreateTriangleData()
{

	Parser fromFile;
	
	fromFile.progressFile("sphere1.txt");
	int nrOfVert = 0;
	//fromFile2.progressFile("obj2.txt");
	int counter = 0;
	vector<TriangleVertex> triangleVertices;

	//TriangleVertex triangleVertices[36];
	

	fromFile.createList();
	nrOfVert = fromFile.getNrOfTriangleVertices();
	//TriangleVertex* triangleVertices = new TriangleVertex[nrOfVert];
	//triangleVertices = new TriangleVertex[nrOfVert];
	//fromFile2.createList();
	for (int i = 0; i < nrOfVert; i++)
	{
		//openGL
		triangleVertices.push_back(fromFile.popFirst());
		//triangleVertices.push_back(fromFile.popFirst());
	}

	//Convert
	for (int i = 0; i < nrOfVert; i++)
	{
		if (counter == 1)
		{
			//swap(triangleVertices[i], triangleVertices[i + 1]);
			TriangleVertex temp = triangleVertices[i];

			triangleVertices[i] = triangleVertices[i+1];
			triangleVertices[i+1] = temp;
			
		}
		else if (counter == 2)
			counter = -1;
			
		counter++;
	}

	nrOfVertexDrawn = triangleVertices.size();

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(*triangleVertices.data()) * nrOfVertexDrawn;  //triangleVertices

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices.data();
	gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBuffer);

	//delete triangleVertices;
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

void Render()
{

	*viewSpace = WorldCamera.getViewMatrix();
	*worldSpace = DirectX::XMMatrixRotationY(3.14f);
	*worldViewProj = Matrix((*worldSpace) * (*viewSpace) * (*projectionSpace));
	worldViewProj = &worldViewProj->Transpose();


	Matrix* WVP_Ptr = nullptr;
	D3D11_MAPPED_SUBRESOURCE viewSpaceData;
	memset(&viewSpaceData, 0, sizeof(viewSpace));

	HRESULT test = gDeviceContext->Map(worldSpaceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &viewSpaceData);
	memcpy(viewSpaceData.pData, worldViewProj->m, sizeof(*worldViewProj));
	//WVP_Ptr = (Matrix*)viewSpaceData.pData;
	//WVP_Ptr = &worldViewProj->Invert();
	//WVP_Ptr = worldViewProj;
	//unsigned int nrOfBuffers = 1;

	gDeviceContext->Unmap(worldSpaceBuffer, 0);


	// gDeviceContext->Unmap(worldSpaceBuffer, 0);

	// clear the back buffer to a deep blue

	float clearColor[] = { 0, 0, 0, 1 };
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	//clear depthbuffer
	gDeviceContext->ClearDepthStencilView(gDepthBuffer, D3D11_CLEAR_DEPTH, 1, 0);

	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	//gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	//gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	UINT32 vertexSize = sizeof(TriangleVertex);
	//UINT32 vertexSize = sizeof(float) * 9;// får inte vara 8 av någon anledngin
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->VSSetConstantBuffers(0, 1, &worldSpaceBuffer);
	//gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);


	//update constantbuffers

	//gDeviceContext->VSSetConstantBuffers(0, 1, &worldSpaceBuffer);
	//gDeviceContext->GSSetConstantBuffers(0, 1, &worldSpaceBuffer);

	gDeviceContext->Draw(nrOfVertexDrawn,0);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster

	if (wndHandle)
	{
		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context

		SetViewport(); //3. Sätt viewport

		CreateShaders(); //4. Skapa vertex- och pixel-shaders

		createWorldMatrices(); //initierar världsmatriserna här, vid frågor och klagomål, så ansvarar jag inte för detta!

		CreateTriangleData(); //5. Definiera triangelvertiser, 6. Skapa vertex buffer, 7. Skapa input layout

		ShowWindow(wndHandle, nCmdShow);


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
				Render(); //8. Rendera

				gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
			}
		}

		gVertexBuffer->Release();

		gVertexLayout->Release();
		gVertexShader->Release();
		gPixelShader->Release();

		gBackbufferRTV->Release();
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
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 4;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		hr = gDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		memset(&descDSV, 0, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		descDSV.Texture2D.MipSlice = 0;

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();

		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, NULL);

		gDevice->CreateDepthStencilView
			(
				pDepthStencil, // Depth stencil texture
				&descDSV, // Depth stencil desc
				&gDepthBuffer
				);		// [out] Depth stencil view

	}
	return hr;
}