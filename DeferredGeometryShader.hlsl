cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
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

struct PixelInputType
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

[maxvertexcount(6)]
void DGS ( triangle GeometryInputType input[3], inout TriangleStream<PixelInputType> OutputStream )
{	
    PixelInputType output;
	float3 edge1, edge2;
	float3 faceNormal;
	float3 viewDirection;
	float temp;

	edge1 = input[1].worldPosition.xyz - input[0].worldPosition.xyz;
	edge2 = input[2].worldPosition.xyz - input[0].worldPosition.xyz;

	faceNormal = cross(edge1, edge2);
	faceNormal = normalize(faceNormal);
	
	viewDirection = input[0].worldPosition.xyz - cameraPosition;
	viewDirection = normalize(viewDirection);

	temp = dot(viewDirection, faceNormal);

    for(int i = 0; i < 3; i++ )
    {
		output.position = input[i].position;
		output.tex = input[i].tex;
		output.color = input[i].color;
		output.normal = input[i].normal;
		output.tangent = input[i].tangent;
		output.binormal = input[i].binormal;
		output.worldPosition = input[i].worldPosition;
		output.lightViewPosition = input[i].lightViewPosition;

		if (temp < 0.0f)
		{
			OutputStream.Append(output);
		}
    }

	OutputStream.RestartStrip();
}
