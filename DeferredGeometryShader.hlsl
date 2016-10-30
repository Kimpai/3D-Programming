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

	edge1 = input[1].position.xyz - input[0].position.xyz;
	edge2 = input[2].position.xyz - input[0].position.xyz;

	faceNormal = cross(edge1, edge2);
	faceNormal = normalize(faceNormal); 

    for( uint i = 0; i < 3; i++ )
    {
		output.position = input[i].position;
		output.tex = input[i].tex;
		output.color = input[i].color;
		output.normal = input[i].normal;
		output.tangent = input[i].tangent;
		output.binormal = input[i].binormal;
		output.worldPosition = input[i].worldPosition;
		output.lightViewPosition = input[i].lightViewPosition;

		//if (faceNormal.z < 0.0f || faceNormal.y > 0.0f)
		//{

		OutputStream.Append( output );
		//}
    }
	
    OutputStream.RestartStrip();
}
