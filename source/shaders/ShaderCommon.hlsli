Texture2D grassTexture : register(t0);
Texture2D rockTexture : register(t1);
Texture2D snowTexture : register(t2);

Texture2D grassNormal : register(t3);
Texture2D rockNormal : register(t4);
Texture2D snowNormal : register(t5);

Texture2D grassMaterial : register(t6);
Texture2D rockMaterial : register(t7);
Texture2D snowMaterial : register(t8);

TextureCube cubeMap : register(t9);

SamplerState sampleState : register(s0);

cbuffer FrameBuffer : register(b0)
{
    float4x4 modelToClipMatrix;
    float4 directionalLightColor;
    float3 directionalLightDir;
    float totalTime;
    float3 cameraPosition;
    float padding;
};

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelMatrix;
};

cbuffer LightBuffer : register(b2)
{
    float4 skyColor;
    float4 groundColor;
}