// ----------------------------------------------------------
// テスト用シェーダー
// 頂点に VertexPN を使う
// ----------------------------------------------------------

// 今回追加した定数バッファ
cbuffer CBParam : register(b0)
{
    float4 baseColor;
};

// ----------------------------------------------------------
// 頂点
// ----------------------------------------------------------
// カメラ定数バッファ
cbuffer CBPerCamera : register(b8)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    float3 cameraPosW;
    float cameraNear;
    float3 cameraForwardW;
    float cameraFar;
    float4 time; // (t, dt, 1/dt, frameCount)
};

// オブジェクト定数バッファ
cbuffer CBPerObject : register(b9)
{
    row_major float4x4 world;
};

// 頂点シェーダーへ入力するデータ
struct VSInput
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
};


// 頂点シェーダーから出力するデータ
struct PSInput
{
    float4 pos : SV_POSITION; // 頂点の座標(射影座標系)
    float4 color : COLOR0;
};

// 頂点シェーダー
PSInput VS(VSInput vin)
{
    PSInput Out;
    float4 p = float4(vin.pos.xyz, 1);
    
    float4 local = p;

    p = mul(p, world);

    float4 worldPos = p;

    float l = length(worldPos.xz);
    worldPos.y += sin(l * -0.5 + time.x);

    p = mul(worldPos, view);
    p = mul(p, projection);

    Out.pos = p;
    
    float c = worldPos.y * 0.2 + 0.5;
    Out.color = baseColor * float4(c, c, c, 1);

    return Out;
}

 
// ----------------------------------------------------------
// ピクセル
// ----------------------------------------------------------

// ピクセルシェーダー
float4 PS(PSInput In) : SV_Target0
{
    // テクスチャの色を出力
    return In.color;
}
