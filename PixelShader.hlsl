Texture2D shaderTexture:register(t0);
Texture2D normalTexture:register(t1);
Texture2D pixelTexture:register(t2);
Texture2D depthTexture:register(t3);
Texture2D lightPosTexture:register(t4);

SamplerState SampleTypePoint:register(s0);

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};


cbuffer LightPositionBuffer
{
	float4 lightPosition;
};

struct PixelInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
};

float4 PS (PixelInputType input):SV_TARGET
{
	float4 diffuse, ambient, specular;
	float d, att, bias; 
	float depthValue, lightDepthValue;
	float4 normal;
	float3 lightDir, viewDir;
	float4 pixelPos, lightViewPos; 
	float4 color;
	float diffuseFactor, specularFactor;
	float3 reflection;
	float4 finalColor;
	float3 projectTexCoord;

	//Set the bias value for fixing the floating point precision issues
	bias = 0.001f;

	//Sample the color and normals from the geometry pass
	color = shaderTexture.Sample(SampleTypePoint, input.tex);
	normal = normalTexture.Sample(SampleTypePoint, input.tex);
	pixelPos = pixelTexture.Sample(SampleTypePoint, input.tex);
	lightViewPos = lightPosTexture.Sample(SampleTypePoint, input.tex);

	//Initialize the color vectors
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	projectTexCoord = lightViewPos.xyz / lightViewPos.w;

	projectTexCoord = projectTexCoord * 0.5f + 0.5f;

	//Get the depth value from the depth texture
	depthValue = depthTexture.Sample(SampleTypePoint, projectTexCoord).r;

	lightDepthValue = projectTexCoord.z;

	//Ambient
	ambient = ambientColor * color;

	//Calculate the light vector
	lightDir = lightPosition.xyz - pixelPos.xyz;

	//Get the distance of the light vector
	d = length(lightDir);

	//Normalize the lightvector
	lightDir = normalize(lightDir);

	//Calculate the view direction vector
	viewDir = cameraPosition.xyz - pixelPos.xyz;

	//Normalize the view direction vector
	viewDir = normalize(viewDir);

	if (d > 300.0f)
	{
		return ambient;
	}

	//Diffuse
	diffuseFactor = saturate(max(dot(normal.xyz, -lightDir), 0.0f));

	diffuse = diffuseFactor * diffuseColor * color;

	//Specular
	reflection = reflect(normal.xyz, lightDir);
	specularFactor = pow(saturate(dot(reflection, viewDir)), 32.0f);

	specular = specularFactor * specularColor * color; 

	//Output final pixelColor
	finalColor = saturate(ambient + diffuse + specular);

	return finalColor;
}