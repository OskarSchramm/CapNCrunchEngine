#include "ShaderCommon.hlsli"

PixelInputLight LightVertexShader(VertexInputLight input)
{
    PixelInputLight output;
    float4 worldPosition;
    
    float4 vertexObjectPos = float4(input.position, 1.0f);
    float4 vertexWorldPos  = mul(modelMatrix, vertexObjectPos);
    float4 vertexClipPos   = mul(worldToClipMatrix, vertexWorldPos);
    output.position = vertexClipPos;
    
    float4 vertexObjectNormal = float4(input.normal, 0.0f);
    float4 vertexWorldNormal = mul(modelMatrix, vertexObjectNormal);
    output.normal = vertexWorldNormal;
    
    worldPosition = mul(vertexObjectPos, modelMatrix);
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
    output.viewDirection = normalize(output.viewDirection);
    
    output.uv = input.uv;
    
    return output;
}