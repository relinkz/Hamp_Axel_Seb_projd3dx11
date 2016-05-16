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
};

[maxvertexcount(7)]
void main(triangle VS_OUT input[3], inout TriangleStream< VS_OUT > data)
{

	float4 pos = float4(0, -2, 0, 0);
	float4 norm2 = float4(1, 0, 1, 0);
	float4 toCamera = cameraPos - input[0].PosView;
	toCamera = normalize(toCamera);

	float angle = dot(toCamera, input[0].Norm);
	if (angle > -0.1f)
	{

		/*float4 normal = float4(cross(input[1].PosView - input[0].PosView, input[2].PosView - input[0].PosView), 0.0f);

		normal = normalize(normal);*/

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
			data.Append(output[i]);


		}
		data.RestartStrip();
	}


}