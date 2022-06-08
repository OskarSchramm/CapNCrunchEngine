cbuffer ObjectBuffer : register(b0)
{
    float4x4 modelMatrix;
};

cbuffer FrameBuffer : register(b1)
{
    float4x4 worldToClipMatrix;
};

cbuffer LightBuffer : register(b2)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

cbuffer CameraBuffer : register(b3)
{
    float3 cameraPosition;
    float padding;
};

struct VertexInputLight
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PixelInputLight
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 viewDirection : TEXCOORD1;
};