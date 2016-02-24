cbuffer lightInfo : register(b0)
{
	float4 lightPos : SV_POSITION;
	float pad;
	float ambient;
	float diffuse;
	float strength;


}

struct VS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Color : COLOR;
};


VS_OUT VS_main(VS_IN input)
{
	//VS_OUT output = (VS_OUT)0;

	//output.Pos = float4(input.Pos, 1);

	//float4 pixelToLight = lightPos - float4(input.Pos, 1.0f);

	//float lightDistance = sqrt(pixelToLight.x * pixelToLight.x + pixelToLight.y * pixelToLight.y + pixelToLight.z * pixelToLight.z);

	//output.Color = input.Color * strength / lightDistance;// *ambient;

	
		//Color finalColor = this->c;
	VS_OUT output = (VS_OUT)0;
	/*Vec hitPoint = {0,0,0};
		Vec lightVector = { 0,0,0 };*/

	/*float red = 0;
	float green = 0;
	float blue = 0;*/


	//subtraction of points give vector
	//hitpoint.Pos = r.o + (r.d)*(h.t);
	//lightvector = hitpoint - light;
	//float3 pixelToLight = lightPos - input.Pos;

	//lightvector.Pos.normalize();
	//pixelToLight = normalize(pixelToLight);


	//float nDotL;
	////nDotL = dot(input.Pos, pixelToLight);
	//nDotL = 1.0f;

	/*if (nDotL < 0)
	{
		nDotL = 0;
	}*/

	/*red = ((input.Color.r * diffuse * nDotL) + input.Color.r * ambient);
	green = ((input.Color.g * diffuse * nDotL) + input.Color.g * ambient);
	blue = ((input.Color.b * diffuse * nDotL) + input.Color.b * ambient);*/

	float red = ((input.Color.r * 0.2) + input.Color.r * 0.2);
	float green = ((input.Color.g * diffuse) + input.Color.g * ambient);
	float blue = ((input.Color.b * diffuse) + input.Color.b * ambient);



	/*red = input.Color.r;
	green = input.Color.g;
	blue = input.Color.b;*/

	/*red = lightPos.x;
	green = lightPos.y;
	blue = lightPos.z;*/

	/*if (255 < red)
	{
		red = 255;
	}
	if (255 < green)
	{
		green = 255;
	}
	if (255 < blue)
	{
		blue = 255;
	}*/
	output.Color.r = red;
	output.Color.g = green;
	output.Color.b = blue;



	/*output.Color = input.Color;*/
	output.Pos = float4(input.Pos, 1.0f);
	return output;
}