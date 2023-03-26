struct VertexInfo {
        [[vk::location(0)]] float3 position : POSITION0;
        [[vk::location(1)]] float3 normal : NORMAL0;
        [[vk::location(2)]] float4 color : COLOR0;
        [[vk::location(3)]] float2 uv : TEXCOORD0;
        [[vk::location(4)]] float tilingFactor : PSIZE;
        [[vk::location(5)]] float entityId : ENTITYID;
};


struct v2p
{
        float4 position : SV_POSITION;
        [[vk::location(0)]] float4 color : COLOR;
        [[vk::location(1)]] float4 normal : NORMAL;
        [[vk::location(2)]] float4 uv : TEXCOORD0;
};

struct UBO
{
        float4x4 viewProjection;
        float4x4 model;
        float4x4 transposedInvertedModel;
};

cbuffer ubo : register(b0) { UBO ubo; }


v2p main(in VertexInfo input)
{
        v2p output;
        output.position = mul(ubo.viewProjection, mul(ubo.model, float4(input.position, 1.0f)));
        output.color = input.color;
        output.normal = float4(mul((float3x3)ubo.transposedInvertedModel, input.normal), 1.0f);
        output.uv = float4(input.uv * input.tilingFactor, 1.0f, 1.0f);

        return output;
}



