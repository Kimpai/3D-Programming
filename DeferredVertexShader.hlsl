cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

struct VertexInputType
{
	float4 position:POSITION;
	float2 tex:TEXCOORD0;
	float4 color:COLOR;
	float3 normal:NORMAL;
	float3 tangent:TANGENT;
	float3 binormal:BINORMAL;
};

struct GeometryInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
	float4 color:COLOR;
	float4 normal:NORMAL;
	float4 tangent:TANGENT;
	float4 binormal:BINORMAL;
	float4 worldPosition:TEXCOORD1;
	float4 lightViewPosition:TEXCOORD2;
};

//Vertex Shader//
GeometryInputType DVS (VertexInputType input)
{
	GeometryInputType output;

	 //Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	//Calc the position of the vertex against the world, view and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//Calculate the position of the vertex as viewed by the light source(s)
	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	//Calculate the position of the vertex in the world
	output.worldPosition = mul(input.position, worldMatrix);

	//Store the input tex coords for the pixel shader to use
	output.tex = input.tex;

	//Set output color to input color
	output.color = input.color;

	//Calculate the normal vector against the world matrix only
	output.normal = mul(float4(input.normal, 1.0f), worldMatrix);
	output.normal = normalize(output.normal);

	//Calculate the tangent vector against the world matrix only
	output.tangent = mul(float4(input.tangent, 1.0f), worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(float4(input.binormal, 1.0f), worldMatrix);
	output.binormal = normalize(output.binormal);

	return output;
}