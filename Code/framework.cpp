#include "framework.h"
#include<stdlib.h>
#include"TextureLoder.h"
#include"Audio.h"
#include"EffectManager.h"
#include "GameSystem.h"
#include "Option.h"


framework::framework(HWND hwnd) : hwnd(hwnd)
	,Device()
	,Immediate_Context()
	,Swap_Chain()
    ,RenderTargetView()
    ,Depth_Stencil_View()
{

}

//UNIT01
bool framework::initialize()
{
	HRESULT hr{ S_OK };

	UINT Create_Device_Flags{ 0 };
#ifdef _DEBUG
	Create_Device_Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//どのバージョンのモデルを使うかの宣言
	D3D_FEATURE_LEVEL Feature_Levels{ D3D_FEATURE_LEVEL_11_0 };

	//****************************************************************************************************************************************************
	//スワップチェーン
	//参考URL：DXGI＿SWAP＿CHAIN＿DESC構造
	//https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/ns-dxgi1_2-dxgi_swap_chain_desc1
	//https://www.yttm-work.jp/directx/directx_0012.html
	//http://kskborn.blog24.fc2.com/blog-entry-49.html

	DXGI_SWAP_CHAIN_DESC Swap_Chain_Desc{};
	Swap_Chain_Desc.BufferCount = 1;

	Swap_Chain_Desc.BufferDesc.Width = SCREEN_WIDTH;  //ゲーム画面の解像度の幅
	Swap_Chain_Desc.BufferDesc.Height = SCREEN_HEIGHT;  //ゲーム画面の解像度の高さ

	Swap_Chain_Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //表示形式の設定

	Swap_Chain_Desc.BufferDesc.RefreshRate.Numerator = 60;  //一秒間に画面が更新される回数（分子）
	Swap_Chain_Desc.BufferDesc.RefreshRate.Denominator = 1;//一秒間に画面が更新される回数（分母）

	Swap_Chain_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //
	Swap_Chain_Desc.OutputWindow = hwnd;

	Swap_Chain_Desc.SampleDesc.Count = 1;  //1ピクセル当たりのマルチサンプリング数
	Swap_Chain_Desc.SampleDesc.Quality = 0;  //高ければ高いほど画質（？）が良くなり処理が重くなる

	Swap_Chain_Desc.Windowed = !FULLSCREEN;   //フルスクリーンかどうか
	
	//完了チェック
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, Create_Device_Flags,
		&Feature_Levels, 1, D3D11_SDK_VERSION, &Swap_Chain_Desc,
		Swap_Chain.GetAddressOf()
		,Device.GetAddressOf(),
		NULL, Immediate_Context.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//****************************************************************************************************************************************************


	//****************************************************************************************************************************************************
	//レンダーターゲットビュー
	//画面にレンダリングされているバックバッファではなく一時的な中間バッファにレンダリングできるようにする。
	ID3D11Texture2D* Back_Buffer{};
	hr = Swap_Chain->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&Back_Buffer));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = Device->CreateRenderTargetView(Back_Buffer, NULL, RenderTargetView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	Back_Buffer->Release();
	//****************************************************************************************************************************************************


	//****************************************************************************************************************************************************
	//
	//深度ステンシルビュー
	//
	//****************************************************************************************************************************************************
	ID3D11Texture2D* Depth_Stencil_Buffer{};
	D3D11_TEXTURE2D_DESC Texture2d_Desc{};

	Texture2d_Desc.Width = SCREEN_WIDTH;     //テクスチャの幅
	Texture2d_Desc.Height = SCREEN_HEIGHT;  //テクスチャの高さ

	Texture2d_Desc.MipLevels = 1;   //ミップマップレベルの最大数
	Texture2d_Desc.ArraySize = 1;   //テクスチャ配列内のテクスチャの数

	Texture2d_Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;  //テクスチャのマルチサンプリングを指定する構造
	//詳細URL：DXGI_FORMAT列挙
	//https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format

	Texture2d_Desc.SampleDesc.Count = 1;
	Texture2d_Desc.SampleDesc.Quality = 0;

	Texture2d_Desc.Usage = D3D11_USAGE_DEFAULT;   //テクスチャの読み取り方法と書き込み方法を識別する値
	Texture2d_Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;  //パイプライン（って何ですか）
	//詳細URL：D3D11_BIND_FLAG列挙 
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_bind_flag

	Texture2d_Desc.CPUAccessFlags = 0;

	hr = Device->CreateTexture2D(&Texture2d_Desc, NULL, &Depth_Stencil_Buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	D3D11_DEPTH_STENCIL_VIEW_DESC Depth_Stencil_View_Desc{};
	Depth_Stencil_View_Desc.Format = Texture2d_Desc.Format;
	Depth_Stencil_View_Desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	Depth_Stencil_View_Desc.Texture2D.MipSlice = 0;


	hr = Device->CreateDepthStencilView(Depth_Stencil_Buffer, &Depth_Stencil_View_Desc, Depth_Stencil_View.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	Depth_Stencil_Buffer->Release();
	//****************************************************************************************************************************************************

	//****************************************************************************************************************************************************
	//ビューポートの設定（表示領域の設定）
	//詳細URL：CD3D11_VIEWPORT構造
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-cd3d11_viewport
	CD3D11_VIEWPORT ViewPort{};
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.Width = static_cast<float>(SCREEN_WIDTH);
	ViewPort.Height = static_cast<float>(SCREEN_HEIGHT);
	ViewPort.MinDepth = 0.0f;
	ViewPort.MaxDepth = 1.0f;

	Immediate_Context->RSSetViewports(1, &ViewPort);

	//*************************************************************************
	//
	// サンプラー
	//
	//*************************************************************************
	D3D11_SAMPLER_DESC Sampler_Desc;
	Sampler_Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	Sampler_Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	Sampler_Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	Sampler_Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Sampler_Desc.MipLODBias = 0;
	Sampler_Desc.MaxAnisotropy = 16;
	Sampler_Desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	
	Sampler_Desc.BorderColor[0] = 0;
	Sampler_Desc.BorderColor[1] = 0;
	Sampler_Desc.BorderColor[2] = 0;
	Sampler_Desc.BorderColor[3] = 0;
	Sampler_Desc.MinLOD = 0;
	Sampler_Desc.MaxLOD = D3D11_FLOAT32_MAX;

	

	hr = Device->CreateSamplerState(&Sampler_Desc, Sampler_States[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Sampler_Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = Device->CreateSamplerState(&Sampler_Desc, Sampler_States[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Sampler_Desc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = Device->CreateSamplerState(&Sampler_Desc, Sampler_States[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Immediate_Context->PSSetSamplers(0, 1, Sampler_States[0].GetAddressOf());
	Immediate_Context->PSSetSamplers(1, 1, Sampler_States[1].GetAddressOf());
	Immediate_Context->PSSetSamplers(2, 1, Sampler_States[2].GetAddressOf());


	//****************************************************************
	// 
	// 深度ステンシルバッファ
	// 
	//****************************************************************
	D3D11_DEPTH_STENCIL_DESC Depth_Stencil_Desc{};
	Depth_Stencil_Desc.DepthEnable = TRUE;
	Depth_Stencil_Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Depth_Stencil_Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = Device->CreateDepthStencilState(&Depth_Stencil_Desc, Depth_Stencil_Status[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Depth_Stencil_Desc.DepthEnable =FALSE;
	Depth_Stencil_Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	Depth_Stencil_Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = Device->CreateDepthStencilState(&Depth_Stencil_Desc, Depth_Stencil_Status[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//****************************************************************
	// 
	// ブレンドステート設定
	// 
	//****************************************************************

	D3D11_BLEND_DESC Blend_Desc{};
	Blend_Desc.AlphaToCoverageEnable = FALSE;
	Blend_Desc.IndependentBlendEnable = FALSE;
	Blend_Desc.RenderTarget[0].BlendEnable = TRUE;
	Blend_Desc.RenderTarget[1].BlendEnable = TRUE;

    //通常時
	Blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	Blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	//減算
	//Blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	//Blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	//加算
	//Blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//Blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;

	//加算(透過あり)
	//Blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//Blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	//乗算
	//Blend_Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//Blend_Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	Blend_Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;


	Blend_Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Blend_Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	Blend_Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	Blend_Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Blend_Desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
	Blend_Desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
	Blend_Desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Blend_Desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = Device->CreateBlendState(&Blend_Desc,Blend_Status[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//****************************************************************
	// 
	// シーン定数バッファの作成
	// 
	//****************************************************************

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(SceneConstance);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = Device->CreateBuffer(&buffer_desc, nullptr, ConstantsBuffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	

	//****************************************************************
	// 
	// ラスタライザーステート
	// 
	//****************************************************************

	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_rasterizer_desc

	D3D11_RASTERIZER_DESC rasterizerdesc{};
	rasterizerdesc.FillMode = D3D11_FILL_SOLID;
	rasterizerdesc.CullMode = D3D11_CULL_BACK;
	rasterizerdesc.FrontCounterClockwise = true;
	rasterizerdesc.DepthBias = 0;
	rasterizerdesc.DepthBiasClamp = 0;
	rasterizerdesc.SlopeScaledDepthBias = 0;
	rasterizerdesc.DepthClipEnable = true;
	rasterizerdesc.ScissorEnable = false;
	rasterizerdesc.MultisampleEnable = false;
	rasterizerdesc.AntialiasedLineEnable = false;

	hr = Device->CreateRasterizerState(&rasterizerdesc, RasterizerState[0].GetAddressOf());

	rasterizerdesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerdesc.CullMode = D3D11_CULL_BACK;
	rasterizerdesc.AntialiasedLineEnable = TRUE;
    hr = Device->CreateRasterizerState(&rasterizerdesc, RasterizerState[1].GetAddressOf()); 
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerdesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerdesc.CullMode = D3D11_CULL_NONE;
	rasterizerdesc.AntialiasedLineEnable = TRUE;
    hr = Device->CreateRasterizerState(&rasterizerdesc, RasterizerState[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerdesc.FillMode = D3D11_FILL_SOLID;
	rasterizerdesc.CullMode = D3D11_CULL_NONE;
	rasterizerdesc.FrontCounterClockwise = true;
	hr = Device->CreateRasterizerState(&rasterizerdesc, RasterizerState[3].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	//****************************************************************
	// 
	// 初期化
	// 
	//****************************************************************

	//--------------------<レンダーターゲットを初期化>--------------------//
	FrameBufferVec[Main] = std::make_unique<FrameBuffer>(Device.Get(), 1280.0f, 720.0f);
	FrameBufferVec[Luminance] = std::make_unique<FrameBuffer>(Device.Get(), 1280.0f, 720.0f);
	RenderBuffer[0] = std::make_unique<Fullscreenquad>(Device.Get(), "./resources/Shaders/FullScreen_PS.cso");
	RenderBuffer[1] = std::make_unique<Fullscreenquad>(Device.Get(), "./resources/Shaders/Fullscreen2_Ps.cso");
	CreatePsFromCso(Device.Get(), "./resources/Shaders/LuminateExtraction_PS.cso", BloomPixelShader[0].GetAddressOf());
	CreatePsFromCso(Device.Get(), "./resources/Shaders/Blur_PS.cso", BloomPixelShader[1].GetAddressOf());

	SceneManager::Instance().SetDevice(Device.Get());
	SceneManager::Instance().Initialize();
	
	EffectManager::Instance().Initialize(Immediate_Context.Get());

	OptionSystem::Instance().Initialize();

	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	GamePad::Instance().SetFirstUpdate();
	GamePad::Instance().Update(elapsed_time);
	SceneManager::Instance().Update(elapsed_time);
	

}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr{ S_OK };

    const FLOAT Color[]{ 0.0f,0.0f,0.0f,1.0f };

	std::lock_guard<std::mutex> lock(SceneManager::Instance().GetMutex());

	ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	Immediate_Context->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	Immediate_Context->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	Immediate_Context->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);


	Immediate_Context->ClearRenderTargetView(RenderTargetView.Get(), Color);
	Immediate_Context->ClearRenderTargetView(FrameBufferVec[Main]->GetRenderTarget().Get(), Color);
	Immediate_Context->ClearRenderTargetView(FrameBufferVec[Luminance]->GetRenderTarget().Get(), Color);
	Immediate_Context->ClearDepthStencilView(Depth_Stencil_View.Get(),   //クリアするステンシルビューへのポインター
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,                         //クリアするデータの種類を特定する
		1.0f,                                                            //この値を使って深度バッファをクリアする（0.0f~1.0fの間にクランプされる）
		0);                                                              //この値を使ってステンシルバッファをクリアする



	//****************************************************************
	// 
	// ビューポートを設定
	// 
	//****************************************************************

	D3D11_VIEWPORT VPs[] = {
		FrameBufferVec[Main]->GetViewPort(),
		FrameBufferVec[Luminance]->GetViewPort(),
	};

	Immediate_Context->RSSetViewports(_countof(VPs), VPs);
	//****************************************************************
	// 
	// 複数のレンダーターゲットを設定
	// 
	//****************************************************************
	ID3D11RenderTargetView* RTVs[8] = {
		//FrameBufferVec[Main]->GetRenderTarget().Get(),
		RenderTargetView.Get(),
		FrameBufferVec[Luminance]->GetRenderTarget().Get()
	};

	Immediate_Context->OMSetRenderTargets(_countof(RTVs), RTVs, Depth_Stencil_View.Get());
	//Immediate_Context->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), Depth_Stencil_View.Get());
	
	Immediate_Context->OMSetBlendState(Blend_Status[0].Get(), nullptr, 0xFFFFFFFF);

	Immediate_Context->OMSetDepthStencilState(Depth_Stencil_Status[0].Get(), 0);
	
	Immediate_Context->RSSetState(RasterizerState[3].Get());
	
	

	SceneManager::Instance().Render(Immediate_Context.Get());

	//Immediate_Context->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), nullptr);

	//// 最終的な描画

	//RenderBuffer[0]->Blit(Immediate_Context.Get(), FrameBufferVec[Luminance]->GetShaderResourceView(0).GetAddressOf(), 0, 1, BloomPixelShader[0].Get());

	// //--------------------<二枚の画像を合成>--------------------//
	ID3D11ShaderResourceView* SRVs[2]
	{
		FrameBufferVec[Main]->GetShaderResourceView(0).Get(),
		FrameBufferVec[Luminance]->GetShaderResourceView(0).Get(),
	};

	//RenderBuffer[1]->Blit(Immediate_Context.Get(), SRVs, 0, 2, BloomPixelShader[1].Get());




	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		
		ImGui::RenderPlatformWindowsDefault();
	}

	
	


	UINT Synic_Imterval{ 0 };

	Swap_Chain->Present(Synic_Imterval, 0);
}

bool framework::uninitialize()
{
	OptionSystem::Instance().Finalize();
	SceneManager::Instance().Finalize();
	EffectManager::Instance().Finalize();
	return true;
}




framework::~framework()
{
	
}