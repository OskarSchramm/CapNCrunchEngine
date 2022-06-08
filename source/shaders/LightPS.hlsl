#include "ShaderCommon.hlsli"

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

float4 LightPixelShader(PixelInputLight input) : SV_TARGET
{
    //float4 textureColor;
    //float3 lightDir;
    //float lightIntensity;
    float4 color;
    //float3 reflection;
    //float4 specular;
    
    //textureColor = shaderTexture.Sample(SampleType, input.uv);
    
    //color = ambientColor;
    
    //specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //lightDir = -lightDirection;
    
    //lightIntensity = saturate(dot(input.normal, lightDir));
    
    //if(lightIntensity > 0.0f)
    //{
    //    color += (diffuseColor * lightIntensity);
        
    //    color = saturate(color);
        
    //    reflection = normalize(2 * lightIntensity * input.normal - lightDir);
        
    //    specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    //}

    //color = color * textureColor;
    //color = saturate(color + specular);
    
    color = input.position;
    
    return color;
}