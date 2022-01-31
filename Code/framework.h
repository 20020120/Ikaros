#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include<memory>

#include "misc.h"
#include "high_resolution_timer.h"

#include<d3d11.h>

#include"Sprite.h"
#include"SpriteBatch.h"
#include"gAudio.h"


#include<wrl.h>
using namespace Microsoft::WRL;
#include"GeometricPrimitive.h"
#include"SkinnedMesh.h"

#include"Framebuffer.h"
#include"Fullscreenquad.h"

//****************************************************************
// 
// シーンマネージャー
// 
//****************************************************************
#include"SceneManager.h"



#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include"Scene.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];


CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"蒼装のイカロス" };

class framework
{
public:
	CONST HWND hwnd;


	framework(HWND hwnd);
	~framework();

	framework(const framework&) = delete;
	framework& operator=(const framework&) = delete;
	framework(framework&&) noexcept = delete;
	framework& operator=(framework&&) noexcept = delete;

	int run()
	{
		MSG msg{};

		if (!initialize())
		{
			return 0;
		}


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);

		// imgui ドッキングのフラグ    // Enable docking(available in imgui `docking` branch at the moment)
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// リリース時imgui.iniを出力させない
#ifdef NDEBUG
		io.IniFilename = 0;
#endif
		
		//ImGuiWindowFlags GuiFlags{};
		//GuiFlags |= ImGuiWindowFlags_NoMove;
		
		// メインウィンドウの外に出す  // Enable Multi-Viewport / Platform Windows
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		io.ConfigViewportsNoDecoration = true; // imguiウィンドウをメインウィンドウの外に出したとき全画面、最小化ができる: false
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(Device.Get(), Immediate_Context.Get());
		ImGui::StyleColorsDark();


		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();
				update(tictoc.time_interval());
				render(tictoc.time_interval());
			}
		}


		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();


#if 1
		BOOL fullscreen = 0;
		Swap_Chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			Swap_Chain->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{

		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }

		if (SceneManager::Instance().GetEnd())
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;

		case WM_MOUSEHWHEEL:
		{

			int zDelta = GET_WHEEL_DELTA_WPARAM(wparam);

			//ノッチ数を求める
			int nNotch = zDelta / WHEEL_DELTA;
			if (nNotch > 0)
			{
				int a = 0;
			}
			else if (nNotch < 0)
			{
				int a = 0;
			}

		}
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool uninitialize();

private:
	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };



	//*********************************
	//UNIT０１
	//*********************************
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

	
	Microsoft::WRL::ComPtr<ID3D11Device>  Device;    // デバイス
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> Immediate_Context; // デバイスコンテキスト
	Microsoft::WRL::ComPtr<IDXGISwapChain>  Swap_Chain;
	Microsoft::WRL::ComPtr<ID3D11BlendState>  Blend_Status[4];  // ブレンドステート
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader[8];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> Sampler_States[3];


	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> Depth_Stencil_View;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  Depth_Stencil_Status[4];

	//****************************************************************
	// 
	// UNIT11 ： シーン定数バッファの作成
	// 
	//****************************************************************

	struct SceneConstance
	{
		DirectX::XMFLOAT4X4 ViewProjection{};
		DirectX::XMFLOAT4 LightDirection{};
		DirectX::XMFLOAT4 CameraPosition{};
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantsBuffers[8];

	std::unique_ptr<GeometricPrimitive> geometric_primitive[8];

	//カメラ
	DirectX::XMFLOAT4 eye  {0.0f,0.0f,-10.0f,1.0f};
	DirectX::XMFLOAT4 focus{0.0f,0.0f,0.0f,1.0f};
	DirectX::XMFLOAT4 up   {0.0f,1.0f,0.0f,0.0f};
	

	//Light 
	DirectX::XMFLOAT4 LightDirection{ };

	//カメラ位置
	DirectX::XMFLOAT4 CameraPosition{};

	//GeometriPrimitive
	DirectX::XMFLOAT3 Position{};
	DirectX::XMFLOAT3 Scale{0.1f,0.1f,0.1f};
	DirectX::XMFLOAT3 Angle{};



	//****************************************************************
	// 
	// UNIT12
	// 
	//****************************************************************

	ComPtr<ID3D11RasterizerState> RasterizerState[4];
	

	//****************************************************************
	// 
	// マルチレンダーターゲット
	// 
	//****************************************************************
	enum BufferType
	{
		Main,      // メイン
	    Luminance, // 輝度
	};

	std::unique_ptr<FrameBuffer> FrameBufferVec[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	// ブルームエフェクト
	Microsoft::WRL::ComPtr<ID3D11PixelShader> BloomPixelShader[2]{}; // ブルーム用のピクセルシェーダー
	std::unique_ptr<Fullscreenquad> RenderBuffer[2]{}; // 最終出力


	// Audio
	Audio audio;
	
	void calculate_frame_stats()
	{
		if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}
};

