#include "DirectXCommon.h"
#include <string>
#include <cassert>

DirectXCommon::~DirectXCommon()
{
	Finalize();
}

void DirectXCommon::Initialize(WinApp* winApp)
{
	// nullptr�`�F�b�N
	assert(winApp);

	this->winApp = winApp;

	// DXGI�f�o�C�X������
	if (!InitializeDXGIDevice()) {
		assert(0);
	}

	// �R�}���h�֘A������
	if (!InitializeCommand()) {
		assert(0);
	}

	// �X���b�v�`�F�[���̐���
	if (!CreateSwapChain()) {
		assert(0);
	}

	// �����_�[�^�[�Q�b�g����
	if (!CreateFinalRenderTargets()) {
		assert(0);
	}

	// �t�F���X����
	if (!CreateFence()) {
		assert(0);
	}

#pragma region  ���_�o�b�t�@�̐���
	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resdesc = {};

	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices);	// ���_���̃T�C�Y
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* vertBuff = nullptr;

	result = _dev->CreateCommittedResource
	(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
#pragma endregion

#pragma region ���_���̃R�s�[
	// ���_���̃}�b�v
	Vertex* vertMap = nullptr;

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(std::begin(vertices), std::end(vertices), vertMap);

	vertBuff->Unmap(0, nullptr);
#pragma endregion

#pragma region ���_�o�b�t�@�r���[�̍쐬

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();	// �o�b�t�@�̉��z�A�h���X
	vbView.SizeInBytes = sizeof(vertices);		// �S�o�C�g��
	vbView.StrideInBytes = sizeof(vertices[0]);	// 1���_�̃o�C�g��
#pragma endregion

#pragma region �C���f�b�N�X�o�b�t�@�̐���
	ID3D12Resource* idxBuff = nullptr;

	// �o�b�t�@�̃T�C�Y�ȊO�̐ݒ���g���܂킷
	resdesc.Width = sizeof(indices);

	result = _dev->CreateCommittedResource
	(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&idxBuff)
	);


#pragma endregion

#pragma region �C���f�b�N�X���̃R�s�[
	unsigned short* mappedIdx = nullptr;
	idxBuff->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(std::begin(indices), std::end(indices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

#pragma endregion 

#pragma region �C���f�b�N�X�o�b�t�@�r���[�̍쐬

	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);
	
#pragma endregion 

#pragma region �e�N�X�`���o�b�t�@�̍쐬

	// �����_���e�N�X�`���̏�����
	const int texWidth = 256;	// �������s�N�Z����
	const int texDataCount = texWidth * texWidth;	// �z��̗v�f��
	DirectX::XMFLOAT4* textureData = new DirectX::XMFLOAT4[texDataCount];	// �e�N�X�`���f�[�^�z��

	for (int i = 0; i < texDataCount; i++)
	{
		//textureData[i].x = rand() % 256;
		//textureData[i].y = rand() % 256;
		//textureData[i].z = rand() % 256;
		//textureData[i].w = 255;
		textureData[i].x = 1;
		textureData[i].y = 1;
		textureData[i].z = 0;
		textureData[i].w = 0;
	}

	// �q�[�v�ݒ�
	heapprop.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// ���C�g�o�b�N
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	// �]����CPU���璼�ڍs��
	heapprop.CreationNodeMask = 0;	// �P��A�_�v�^�[�̂���0
	heapprop.VisibleNodeMask = 0;

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resDesc.Width = texWidth;	// ��
	resDesc.Height = texWidth;	// ����
	resDesc.DepthOrArraySize = 1;	// 2D�z�񂶂�Ȃ��̂�1
	resDesc.SampleDesc.Count = 1;	// �A���`�G�C���A�V���O���Ȃ�
	resDesc.SampleDesc.Quality = 0;	// �Œ�N�I���e�B
	resDesc.MipLevels = 1;	// �~�b�v�}�b�v���Ȃ��̂�1
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`���p
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// ���C�A�E�g�͌��肵�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	// ���Ƀt���O�Ȃ�

	// ���\�[�X�̐���
	ID3D12Resource* texbuff = nullptr;

	result = _dev->CreateCommittedResource
	(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,		// �e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&texbuff)
	);

	// �f�[�^�]��
	result = texbuff->WriteToSubresource
	(
		0,
		nullptr,		// �S�̈�ɃR�s�[
		textureData,	// ���f�[�^�A�h���X
		sizeof(DirectX::XMFLOAT4) * texWidth,		// 1���C���T�C�Y
		sizeof(DirectX::XMFLOAT4) * texDataCount	// �S�T�C�Y
	);

	delete[] textureData;	// �]�����ς񂾂̂Ō��f�[�^���

#pragma endregion

#pragma region �e�N�X�`���p�V�F�[�_�[���\�[�X�r���[
	// �f�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};

	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// �V�F�[�_�[���猩����悤��
	descHeapDesc.NodeMask = 0;	// �}�X�N0
	descHeapDesc.NumDescriptors = 1;	// �r���[1��
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// �V�F�[�_�[���\�[�X�r���[�p
	
	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&texDescHeap));

	// �V�F�[�_�[���\�[�X�r���[�����
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// RGBA(0.0f~0.1f�ɐ��K��)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;	// �~�b�v�}�b�v�͎g��Ȃ��̂�1

	// �V�F�[�_�[���\�[�X�r���[�̐���
	_dev->CreateShaderResourceView
	(
		texbuff,	// �r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc,	// �e�N�X�`���ݒ���
		texDescHeap->GetCPUDescriptorHandleForHeapStart()	// �q�[�v�̂ǂ��Ɋ��蓖�Ă邩
	);


#pragma endregion

#pragma region �V�F�[�_�[�̓ǂݍ��݂Ɛ���
	ID3DBlob* vsBlob = nullptr;		// �V�F�[�_�[�ێ��p
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	result = D3DCompileFromFile
	(
		L"BasicVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSmain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		&errorBlob
	);
	if (FAILED(result))
	{
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());

		errstr += "\n";

		// �G���[���e���E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	result = D3DCompileFromFile
	(
		L"BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSmain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errorBlob
	);
	if (FAILED(result))
	{
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());

		errstr += "\n";

		// �G���[���e���E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

#pragma endregion

#pragma region ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		// ���W
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		// uv
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};
#pragma endregion 


#pragma region �O���t�B�b�N�p�C�v���C���X�e�[�g�̍쐬
	// �V�F�[�_�[�̃Z�b�g
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipline = {};

	gPipline.pRootSignature = nullptr;

	gPipline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gPipline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gPipline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gPipline.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�ƃ��X�^���C�U�\�X�e�[�g�̐ݒ�
	gPipline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// �܂��A���`�G�C���A�X���g��Ȃ��̂�false
	gPipline.RasterizerState.MultisampleEnable = false;
	gPipline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// �J�����O���Ȃ�
	gPipline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// ���g��h��Ԃ�
	gPipline.RasterizerState.DepthClipEnable = true;			// �[�x�����̃N���b�s���O

	// �u�����h�X�e�[�g�̐ݒ�
	gPipline.BlendState.AlphaToCoverageEnable = false;
	gPipline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	gPipline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// ���̓��C�A�E�g�̐ݒ�
	gPipline.InputLayout.pInputElementDescs = inputLayout;		// ���C�A�E�g�擪�A�h���X
	gPipline.InputLayout.NumElements = _countof(inputLayout);	// ���C�A�E�g�z��̗v�f��
	gPipline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;		// �J�b�g����
	gPipline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// �O�p�`�ō\��

	// �����_�[�^�[�Q�b�g�̐ݒ�
	gPipline.NumRenderTargets = 1;		// ����1�̂�
	gPipline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	// 0~1�ɐ��K�����ꂽRGBA

	// �A���`�G�C���A�V���O�̂��߂̃T���v�����ݒ�
	gPipline.SampleDesc.Count = 1;	// �T���v�����O��1�s�N�Z��1��
	gPipline.SampleDesc.Quality = 0;	// �Œ�N�I���e�B

	// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
	result = _dev->CreateGraphicsPipelineState(&gPipline, IID_PPV_ARGS(&_piplineState));

#pragma endregion 

#pragma region ���[�g�p�����[�^�[�̍쐬
	// �f�X�N���v�^�[�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descTblRange = {};

	descTblRange.NumDescriptors = 1;	// �e�N�X�`��1��
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// ��ʂ̓e�N�X�`��
	descTblRange.BaseShaderRegister = 0;	// 0�ԃX���b�g����
	descTblRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparam = {};

	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// �s�N�Z���V�F�[�_�[���猩����
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;	// �f�X�N���v�^�����W�̃A�h���X
	rootparam.DescriptorTable.NumDescriptorRanges = 1;	// �f�X�N���v�^�����W��

#pragma endregion

#pragma region �T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};

	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// �������̌J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// �c�����̌J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// ���s�����̌J��Ԃ�
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// �{�[�_�[�͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// ���`���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;	// �~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;				// �~�b�v�}�b�v�ŏ��l
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// �s�N�Z���V�F�[�_�[���猩����
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	// �T���v�����O���Ȃ�

#pragma endregion

#pragma region	���[�g�V�O�l�`��
	// D3D12_ROOT_SIGNAAATURE_DESC�̐ݒ�
	
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootparam;	// ���[�g�p�����[�^�[�̐擪�A�h���X
	rootSignatureDesc.NumParameters = 1;	// ���[�g�p�����[�^�[�̐�
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// �o�C�i���R�[�h�̍쐬
	ID3D10Blob* rootSigBlob = nullptr;

	result = D3D12SerializeRootSignature
	(
		&rootSignatureDesc,				// ���[�g�V�O�l�`���ݒ�
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// ���[�g�V�O�l�`���o�[�W����
		&rootSigBlob,
		&errorBlob
	);

	// ���[�g�V�O�l�`���I�u�W�F�N�g�̍쐬
	result = _dev->CreateRootSignature
	(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature)
	);

	rootSigBlob->Release();

	// �p�C�v���C���X�e�[�g�̐���
	gPipline.pRootSignature = rootsignature;

	result = _dev->CreateGraphicsPipelineState(&gPipline, IID_PPV_ARGS(&_piplineState));
#pragma endregion

#pragma region �r���[�|�[�g�ƃV�U�[��`

	// �r���[�|�[�g�̐ݒ�
	viewport.Width = WinApp::WINDOW_WIDTH;
	viewport.Height = WinApp::WINDOW_HEIGHT;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	// �V�U�[��`�̐ݒ�
	// �؂蔲�����W
	scissorrect.top = 0;
	scissorrect.left = 0;
	scissorrect.right = scissorrect.left + WinApp::WINDOW_WIDTH;
	scissorrect.bottom = scissorrect.top + WinApp::WINDOW_HEIGHT;

#pragma endregion
}

void DirectXCommon::Finalize()
{
}

void DirectXCommon::PreDraw()
{
	result = _cmdAllocator->Reset();	// �R�}���h�A���P�[�^�[�̃N���A
	bbIdx = _swapchain->GetCurrentBackBufferIndex();	// ���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X

#pragma region ���\�[�X�o���A��ύX

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// �J��
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// ���ɂȂ�
	barrierDesc.Transition.pResource = _backBuffers[bbIdx];		// �o�b�N�o�b�t�@���\�[�X
	barrierDesc.Transition.Subresource = 0;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		// �\������
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	// �`���
	_cmdList->ResourceBarrier(1, &barrierDesc);		// �o���A�w����s

#pragma endregion

#pragma region ��ʃN���A�R�}���h
	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

	ClearRenderTarget();

#pragma endregion

}

void DirectXCommon::PostDraw()
{
#pragma region ���\�[�X�o���A��߂�
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;		// �`�悩��
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;				// �\����
	_cmdList->ResourceBarrier(1, &barrierDesc);		// �o���A�w����s

#pragma endregion

#pragma region �`��R�}���h
	_cmdList->Close();	// ���߂̃N���[�Y

	ID3D12CommandList* cmdLists[] = { _cmdList };	// ���s����R�}���h��p�z��
	_cmdQueue->ExecuteCommandLists(1, cmdLists);	// �R�}���h���X�g�̎��s

	_cmdQueue->Signal(_fence, ++_fenceVal);
	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	// �C�x���g�n���h���̎擾
		_fence->SetEventOnCompletion(_fenceVal, event);
		WaitForSingleObject(event, INFINITE);	// �C�x���g����������܂ő҂�������
		CloseHandle(event);		// �C�x���g�n���h�������
	}

	_cmdAllocator->Reset();	// �L���[���N���A
	_cmdList->Reset(_cmdAllocator, nullptr);	// �ĂуR�}���h���X�g�����߂鏀��

	_swapchain->Present(1, 0);	// �o�b�t�@���t���b�v

#pragma endregion

}

void DirectXCommon::ClearRenderTarget()
{
	bbIdx = _swapchain->GetCurrentBackBufferIndex();	// ���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X

	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);	// ��ʃN���A


	_cmdList->SetPipelineState(_piplineState);	// �p�C�v���C���X�e�[�g�̃Z�b�g

	_cmdList->RSSetViewports(1, &viewport);

	_cmdList->RSSetScissorRects(1, &scissorrect);

	_cmdList->SetGraphicsRootSignature(rootsignature);		// ���[�g�V�O�l�`���̐ݒ�

	_cmdList->SetDescriptorHeaps(1, &texDescHeap);	// �f�B�X�N���v�^�[�q�[�v�̎w��

	_cmdList->SetGraphicsRootDescriptorTable(0, texDescHeap->GetGPUDescriptorHandleForHeapStart());	// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��

	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// �v���~�e�B�u�`��̐ݒ�R�}���h

	_cmdList->IASetVertexBuffers(0, 1, &vbView);	// ���_�o�b�t�@�̐ݒ�R�}���h

	_cmdList->IASetIndexBuffer(&ibView);	// �C���f�b�N�X�o�b�t�@�̐ݒ�R�}���h

	_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);	// �`��R�}���h

}

bool DirectXCommon::InitializeDXGIDevice()
{
#ifdef DEBUG
	// �f�o�b�O���C���[���I����
	ID3D12Debug* debugContoroller;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugContoroller))))
	{
		debugContoroller->EnableDebugLayer();
	}

	//debugContoroller->Release();
#endif // DEBUG

	// �t�B�[�`���[���x����A���Ń`�F�b�N����p�z��
	D3D_FEATURE_LEVEL levels[4] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	_dxgiFactory = nullptr;

	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));	// DXGIFactory��_dxgiFactory�ɓ����
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	std::vector<IDXGIAdapter*> adapters;	// �A�_�v�^�[�̗񋓗p
	IDXGIAdapter* tmpAdapter = nullptr;		// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);	// �A�_�v�^�[�̐����I�u�W�F�N�g�擾
		std::wstring strDesc = adesc.Description;

		// �T�������A�_�v�^�[�̖��O���m�F
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	D3D_FEATURE_LEVEL featureLevel;

	_dev = nullptr;
	result = S_FALSE;
	for (auto lv : levels)
	{
		// �f�o�C�X��������������
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			break;	// �����\�ȃo�[�W���������������甲����
		}
	}
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::InitializeCommand()
{
	result = S_FALSE;

	// �R�}���h���X�g�ƃR�}���h�A���P�[�^�[�̐���
	_cmdAllocator = nullptr;
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	_cmdList = nullptr;
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �R�}���h�L���[�[�̐ݒ�Ɛ���
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;				// �^�C���A�E�g�Ȃ�
	cmdQueueDesc.NodeMask = 0;										// �A�_�v�^�[���P�����g��Ȃ��Ƃ��͂O�ł���
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;				// �R�}���h���X�g�ƍ��킹��

	_cmdQueue = nullptr;
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));		// �L���[����
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::CreateSwapChain()
{
	result = S_FALSE;

	// �X���b�v�`�F�[���̐ݒ�Ɛ���
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = WinApp::WINDOW_WIDTH;
	swapchainDesc.Height = WinApp::WINDOW_HEIGHT;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;									// �\�Ɨ��œ��
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;					// �o�b�N�o�b�t�@�͐L�яk�݉\
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// �t���b�v������j��
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;			// ���Ɏw��Ȃ�
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �E�B���h�E�ƃt���X�N���[���̐؂�ւ��\

	_swapchain = nullptr;
	result = _dxgiFactory->CreateSwapChainForHwnd
	(
		_cmdQueue,
		winApp->GetHwnd(),
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::CreateFinalRenderTargets()
{
	result = S_FALSE;

	// �f�X�N���v�^�q�[�v���쐬
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// �����_�[�^�[�Q�b�g�r���[
	//heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;						// ���\��2��
	//heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// ���Ɏw��Ȃ�

	rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// �X���b�v�`�F�[���̃������ƕR�Â���
	DXGI_SWAP_CHAIN_DESC swcDesc = {};

	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	_backBuffers.resize(swcDesc.BufferCount);
	for (int i = 0; i < swcDesc.BufferCount; i++)
	{
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));	// �X���b�v�`�F�C������o�b�t�@���擾
		if (FAILED(result)) {
			assert(0);
			return result;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();	// �f�X�N���v�^�q�[�v�̃n���h�����擾
		handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(heapDesc.Type);	// ���\�̃A�h���X�̂���
		_dev->CreateRenderTargetView(_backBuffers[i], nullptr, handle);			// RTV�̐���
	}

	return true;
}

bool DirectXCommon::CreateFence()
{
	// �t�F���X�̐���
	_fence = nullptr;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}
