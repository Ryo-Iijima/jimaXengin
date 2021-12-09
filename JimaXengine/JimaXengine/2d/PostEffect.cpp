#include "PostEffect.h"
using namespace Microsoft::WRL;

JimaXengine::DirectXCommon* JimaXengine::PostEffect::dxCommon = nullptr;
ComPtr<ID3D12DescriptorHeap> JimaXengine::PostEffect::descHeapSRV = {};


void JimaXengine::PostEffect::DrawCommands(const std::string& filename, const std::string& registername, const std::string& blendtype)
{
	PostEffect::dxCommon->GetCommandList()->SetPipelineState(pipelines[registername + blendtype].Get());
	PostEffect::dxCommon->GetCommandList()->SetGraphicsRootSignature(spriteRootSignature.Get());
	PostEffect::dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	PostEffect::dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	PostEffect::dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &spriteVBView);

	PostEffect::dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteConstBuff->GetGPUVirtualAddress());
	PostEffect::dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());

	PostEffect::dxCommon->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

JimaXengine::PostEffect::PostEffect()
{
	filename = "white1x1.png";
	position = Vector2(0, 0);
	size = Vector2(500, 500);
	angle = 0;
	scale = Vector2(500, 500);
	blendtype = "NOBLEND";
	anchor = Vector2(0, 0);
	color = Vector4(1, 1, 1, 1);

	CreateSprite();
}

void JimaXengine::PostEffect::Initialize(DirectXCommon* dxcommon)
{
	HRESULT result;

	dxCommon = dxcommon;

	// 基底の初期化もしておく
	Object2d::Initialize(dxcommon);


	// テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WinApp::WINDOW_WIDTH,
		WinApp::WINDOW_HEIGHT,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	// テクスチャバッファの生成
	result = dxcommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);
	assert(SUCCEEDED(result));

	{// テクスチャを赤クリア
		// 画素数
		const UINT pixelCount = WinApp::WINDOW_WIDTH * WinApp::WINDOW_HEIGHT;
		// 画像１行分のデータサイズ
		const UINT rowPitch = sizeof(UINT) * WinApp::WINDOW_WIDTH;
		// 画像全体のデータサイズ
		const UINT depthPitch = rowPitch * WinApp::WINDOW_HEIGHT;
		// 画像イメージ
		UINT* img = new UINT[pixelCount];
		for (int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

		// テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[] img;
	}

	// SRV用デスクリプタ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	result = Object2d::dxCommon->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	// SRV用デスクリプタヒープを生成
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	//シェーダーリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	dxCommon->GetDevice()->CreateShaderResourceView(
		texBuff.Get(),
		&srvDesc,
		descHeapSRV->GetCPUDescriptorHandleForHeapStart()
	);

}

void JimaXengine::PostEffect::Draw()
{
	D3D12_RESOURCE_DESC resDesc = Texture::GetTexture(filename)->GetDesc();
	float width = (float)resDesc.Width;
	float height = (float)resDesc.Height;

	float left = (0.0f - anchor.x) * width;
	float right = (1.0f - anchor.x) * width;
	float top = (0.0f - anchor.y) * height;
	float bottom = (1.0f - anchor.y) * height;

	SpriteVertex vertices[4];
	enum { LB, LT, RB, RT };
	vertices[LB] = { {  left, bottom, 0.0f },{ 0.0f, 1.0f } };
	vertices[LT] = { {  left,    top, 0.0f },{ 0.0f, 0.0f } };
	vertices[RB] = { { right, bottom, 0.0f },{ 1.0f, 1.0f } };
	vertices[RT] = { { right,    top, 0.0f },{ 1.0f, 0.0f } };

	SpriteVertex* vertMap = nullptr;
	spriteVertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];
	}
	spriteVertBuff->Unmap(0, nullptr);

	MatrixUpdate(position, angle, scale);
	TransferConstBuffer({ color.x,color.y,color.z,color.w });

	DrawCommands(filename, "Basic2D", blendtype);
}
