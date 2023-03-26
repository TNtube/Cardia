struct v2p
{
        [[vk::location(0)]] float4 color : COLOR;
        [[vk::location(1)]] float4 normal : NORMAL;
        [[vk::location(2)]] float4 uv : TEXCOORD0;
};

struct PS_OUTPUT
{
        float4 outputColor : SV_Target0;
        int entityId : SV_Target1;
};

// uniform float3 u_ViewPosition;
Texture2D u_Texture : register(t1);
SamplerState sampler_Texture;


PS_OUTPUT main(in v2p input) {
        if (input.color.a == 0) {
                discard;
        }

        PS_OUTPUT output;

        output.outputColor = u_Texture.Sample(sampler_Texture, input.uv.xy) * input.color;
        output.entityId = -1;

        return output;
}
