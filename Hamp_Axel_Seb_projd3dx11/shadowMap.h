#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"

using namespace DirectX;

class ShadowMap
{
public:
	ShadowMap();
	ShadowMap(ID3D11Device* device, UINT width, UINT height);
	virtual ~ShadowMap();

	ID3D11ShaderResourceView* getDepthMapSRV() const;
	SimpleMath::Matrix getOrtographProj() const;

	void bindDsvandSetNullRenderTarget(ID3D11DeviceContext* deviceContext);
private:
	UINT width;
	UINT height;

	ID3D11ShaderResourceView* DepthMapSRV;
	ID3D11DepthStencilView* DepthMapDSV;
	

	D3D11_VIEWPORT viewPort;
};

