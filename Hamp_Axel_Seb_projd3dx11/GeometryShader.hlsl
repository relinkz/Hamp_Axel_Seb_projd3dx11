cbuffer CameraData : register(b0)
{
	float4 cameraPos;
}
struct GSOutput
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};
struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
	float4 PosView : POSITION;
	float3 Tangent : TANGENT;
	uint objID : ID;

};

struct VS_IN
{
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
	float4 PosView : POSITION;
	uint objID : ID;
};

[maxvertexcount(7)]
void main(triangle VS_IN input[3], inout TriangleStream< VS_OUT > data)
{

	float4 pos = float4(0, -2, 0, 0);
	float4 norm2 = float4(1, 0, 1, 0);
	float4 toCamera = cameraPos - input[0].PosView;
	toCamera = normalize(toCamera);

	//input[1].objID = input[0].objID;
	//input[2].objID = input[0].objID;

	float angle = dot(toCamera, input[0].Norm);
	//backface culling
	if (angle > -1.1f)
	{
		//calculate the Tangent to be used in NormalMapping
		float3 edge1 = normalize(input[1].PosView - input[0].PosView);
		float3 edge2 = normalize(input[2].PosView - input[0].PosView);

		float2 texEdge1 = normalize(input[1].Tex - input[0].Tex);
		float2 texEdge2 = normalize(input[2].Tex - input[0].Tex);

		float det = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x);

		float3 tangent = float3(0, 0, 0);
		tangent.x = (texEdge2.y * edge1.x - texEdge1.y * edge2.x) * det;
		tangent.y = (texEdge2.y * edge1.y - texEdge1.y * edge2.y) * det;
		tangent.z = (texEdge2.y * edge1.z - texEdge1.y * edge2.z) * det;
		tangent = normalize(tangent);

		VS_OUT output[3];
		for (uint i = 0; i < 3; i++)
		{
			output[i].Pos = input[i].Pos;
			output[i].Tex = input[i].Tex;
			output[i].Norm = input[i].Norm;
			output[i].PosView = input[i].PosView;
			output[i].Tangent = tangent;
			output[i].objID = input[i].objID;
			data.Append(output[i]);


		}
		data.RestartStrip();
	}
}