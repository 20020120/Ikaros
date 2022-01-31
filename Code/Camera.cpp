#include "Camera.h"
#include"misc.h"
#include<imgui.h>
#include "User_Function.h"
#include "GameSystem.h"

Camera::Camera(ID3D11Device* d)
{
	HRESULT hr = { S_OK };
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
	hr = d->CreateBuffer(&buffer_desc, nullptr, ConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	


	//****************************************************************
	// 
	// 各変数の初期化
	// 
	//****************************************************************

	//カメラ
	Focus = { 0.0f,0.0f,0.0f };
	Up = { 0.0f,1.000001f,0.0f };
	LightDirection = { 1.0f,1.0f,1.0f,0.0f };
	LightPower = { 2.0f };
	Target = {};
	attentionBoss = false;
	BossPosition = {};
#if 0
	Eye = eyeTop;//縦スクロール
	Angle.x = DirectX::XMConvertToRadians(90.0f);//縦スクロール
#else
	Eye = eyeSide;//横スクロール
	Angle.y = DirectX::XMConvertToRadians(-90.0f);//横スクロール

#endif

	changeCameraDirState = 0;//縦横切り替えるときのステート

	mCameraShakeFunc = [this]()
	{
		camera_shake = true;
	};
}

Camera::~Camera()
{

}


void Camera::Update(float elapsedTime)
{
	if(IsTargetPlayer)
	{
		DirectX::XMFLOAT3 eye{};
		constexpr float range = 10.0f;
		Eye.x = Target.x + range*0.1f;
		Eye.y = Target.y - range;
		Eye.z = Target.z;

		Focus = Target;

	}

	else
	{
		//--------------------<TargetからEyeを算出>--------------------//

		DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(Angle.x, Angle.y, Angle.z);

		DirectX::XMFLOAT3 flont;
		DirectX::XMStoreFloat3(&flont, transform.r[2]);

		DirectX::XMFLOAT3 eye{};
		constexpr float range = 65.0f;
		eye.x = Focus.x - (flont.x * range);
		eye.y = Focus.y - (flont.y * range);
		eye.z = Focus.z - (flont.z * range);


		// カメラシェイク
		Shake(elapsedTime, eye);

		//演出
		// ボスを優先
		if (attentionBoss)
		{
			AttentionBoss(elapsedTime);
		}
		else
		{
			AttentionPlayer(elapsedTime);
		}


		//縦と横切り替える
		ChangeCameraDir(elapsedTime);
		// ボスによって

		SetLookAtLH(eye, Target, Up);
	}
}

void Camera::FirstPoseUpdate(float elapsedTime)
{
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(Angle.x, Angle.y, Angle.z);

	DirectX::XMFLOAT3 flont;
	DirectX::XMStoreFloat3(&flont, transform.r[2]);

	DirectX::XMFLOAT3 eye{};
	constexpr float range = 65.0f;
	eye.x = Focus.x - (flont.x * range);
	eye.y = Focus.y - (flont.y * range);
	eye.z = Focus.z - (flont.z * range);

	SetLookAtLH(eye, Target, Up);
}

void Camera::f_CameraUpdate(ID3D11DeviceContext* dc)
{
	using namespace DirectX;
	//****************************************************************
	// 
	// UNIT16 カメラ位置
	// 
	//****************************************************************
	{
		D3D11_VIEWPORT viewport;
		UINT num_viewports{ 1 };
		dc->RSGetViewports(&num_viewports, &viewport);

		float aspect_ratio{ viewport.Width / viewport.Height };

		XMMATRIX P{ XMMatrixPerspectiveFovLH(XMConvertToRadians(45), aspect_ratio, 0.1f, 100000.0f) };
		DirectX::XMStoreFloat4x4(&Projection, P);
		XMVECTOR e = DirectX::XMLoadFloat3(&Eye);
		XMVECTOR f = DirectX::XMLoadFloat3(&Focus);
		XMVECTOR u = DirectX::XMLoadFloat3(&Up);


		XMMATRIX V{ XMMatrixLookAtLH(e, f, u) };

		
		// LightDirectionを正規化	
		DirectX::XMVECTOR LD{ DirectX::XMLoadFloat4(&this->LightDirection) };
		LD = DirectX::XMVector4Normalize(LD);
		DirectX::XMStoreFloat4(&this->LightDirection, LD);


		XMStoreFloat4x4(&data.ViewProjection, V * P);
		data.CameraPosition = this->CameraPosition;
		dc->UpdateSubresource(ConstantBuffer.Get(), 0, 0, &data, 0, 0);

		//定数バッファを頂点シェーダにバインド
		dc->VSSetConstantBuffers(1, 1, ConstantBuffer.GetAddressOf());

		//定数バッファをピクセルシェーダーにバインド
		dc->PSSetConstantBuffers(1, 1, ConstantBuffer.GetAddressOf());
	}
}

void Camera::TitleConstantBufferUpdate(ID3D11DeviceContext* dc)
{
	using namespace DirectX;
	//****************************************************************
	// 
	// UNIT16 カメラ位置
	// 
	//****************************************************************
	{
		D3D11_VIEWPORT viewport;
		UINT num_viewports{ 1 };
		dc->RSGetViewports(&num_viewports, &viewport);

		float aspect_ratio{ viewport.Width / viewport.Height };

		XMMATRIX P{ XMMatrixPerspectiveFovLH(XMConvertToRadians(45), aspect_ratio, 0.1f, 100000.0f) };
		DirectX::XMStoreFloat4x4(&Projection, P);
		XMVECTOR e = DirectX::XMLoadFloat3(&Eye);
		XMVECTOR f = DirectX::XMLoadFloat3(&Focus);
		XMVECTOR u = DirectX::XMLoadFloat3(&Up);


		XMMATRIX V{ XMMatrixLookAtLH(e, f, u) };
		DirectX::XMStoreFloat4x4(&View, V);

		// LightDirectionを正規化	
		DirectX::XMVECTOR LD{ DirectX::XMLoadFloat4(&this->LightDirection) };
		LD = DirectX::XMVector4Normalize(LD);
		DirectX::XMStoreFloat4(&this->LightDirection, LD);


		XMStoreFloat4x4(&data.ViewProjection, V * P);
		data.CameraPosition = { Eye.x,Eye.y,Eye.z,0.0f };
		dc->UpdateSubresource(ConstantBuffer.Get(), 0, 0, &data, 0, 0);

		//定数バッファを頂点シェーダにバインド
		dc->VSSetConstantBuffers(1, 1, ConstantBuffer.GetAddressOf());

		//定数バッファをピクセルシェーダーにバインド
		dc->PSSetConstantBuffers(1, 1, ConstantBuffer.GetAddressOf());
	}
}



void Camera::f_Gui()
{
#if _DEBUG
	ImGui::Begin("Camera");
	if (ImGui::Button("ChangeCameraDir"))ChangeView();

	ImGui::Checkbox("MouseLock", &Locked);

	ImGui::SliderFloat4("Eye", &Eye.x, -20.0f, 20.0f);
	ImGui::SliderFloat4("Focus", &Focus.x, -20.0f, 20.0f);

	ImGui::SliderFloat4("LightDirection", &LightDirection.x, -1.0f, 1.0f);
	ImGui::SliderFloat4("Position", &CameraPosition.x, -100.0f, 100.0f);

	ImGui::DragFloat3("Angle", &Angle.x,0.1f);

	ImGui::InputFloat3("Front", &Forward.x);
	ImGui::InputFloat3("Right", &Right.x);
	if(!OpenMenu&&ImGui::Button("OpenSetting"))  OpenMenu = true;
	if (OpenMenu && ImGui::Button("CloseSetting")) OpenMenu = false;

	if (!camera_shake && ImGui::Button("Not Shake")) SetCameraShake();
	if (camera_shake && ImGui::Button("Shaking")) {}

	ImGui::Checkbox("AttentionPlayer", &attentionPlayer);
	ImGui::SliderFloat("Ratio", &ratio, 0.0f, 1.0f);
	ImGui::SliderFloat("TargetX", &targetX, -70.0f, 0.0f);

	ImGui::Checkbox("IsFlyingFinisher", &IsTargetPlayer);

	ImGui::SliderInt("ChangeCameraDirState", &changeCameraDirState, 0, 5);
	ImGui::DragFloat3("Up", &Up.x);
	
	ImGui::End();
#endif

}



void Camera::f_UpdateCameraForMouse()
{
	//--------------------<メニューが開いていたらカメラを動かせない>--------------------//
	if (OpenMenu) return;
	if (Locked) return;


	//****************************************************************
	// 
	// マウスによるカメラの制御
	// 
	//****************************************************************

	

	
}

void Camera::f_SettingMouse()
{
	
	ImGui::Text("Setting");
	ImGui::DragFloat2("sensitivity", &sensitivity.x, 0.0f, 1.0f);
	ImGui::Checkbox("Reverse_Axis[X]", &ReverseX);
	ImGui::Checkbox("Reverse_Axis[Y]", &ReverseY);

	if (ImGui::Button("ResetCamera"))
	{
		Eye = { 0.0f,0.0f,-10.0f };
		Focus = { 0.0f,0.0f,0.0f };
		Up = { 0.0f,1.0f,0.0f };
	}
	
}


void Camera::DemoCameraUpdate(float elapsedTime)
{
	// デモ用のカメラシェイク
	DemoShake(elapsedTime, Eye);
}




void Camera::SetTarget(DirectX::XMFLOAT3 target)
{
	Target = DirectX::XMFLOAT3(target.x, target.y, target.z);
}

void Camera::SetTarget_FlyingFinisher(DirectX::XMFLOAT3 target)
{
	IsTargetPlayer = true;
	Target = target;
}



void Camera::SetLookAtLH(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	DirectX::XMFLOAT3 f = focus;
	if (eye.z > focus.z)
		f.z = eye.z + 0.001f;

	DirectX::XMMATRIX L_View = 
		DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eye),
								  DirectX::XMLoadFloat3(&f),
								  DirectX::XMLoadFloat3(&up));
	DirectX::XMStoreFloat4x4(&View, L_View);

	//ビュー行列を逆行列化してワールド行列に戻す
	DirectX::XMMATRIX w = DirectX::XMMatrixInverse(nullptr, L_View);
	DirectX::XMFLOAT4X4 World;
	DirectX::XMStoreFloat4x4(&World, w);

	//カメラの方向を取り出す
	this->Right.x = World._11;
	this->Right.y = World._12;
	this->Right.z = World._13;

	if(this->Right.x < 0.0f)
	{
		this->Right.x = World._11;
	}

	this->Up.x = World._21;
	this->Up.y = World._22;
	this->Up.z = World._23;

	this->Forward.x = World._31;
	this->Forward.y = World._32;
	this->Forward.z = World._33;

	//視点・注視点を設定
	this->Eye = eye;
	this->Focus = focus;

	constexpr float range = 65.0f;
	CameraPosition.x = Focus.x - (Forward.x * range);
	CameraPosition.y = Focus.y - (Forward.y * range);
	CameraPosition.z = Focus.z - (Forward.z * range);



}

void Camera::SetAngle(DirectX::XMFLOAT3 angle)
{
	Angle = angle;
}


void Camera::ChangeView()
{
	changeCameraDir = true;
	GameSystem::Instance().SetChangingView(true);
}

void Camera::UpdateLastPerformance(float elapsedTime,int State)
{
	if (State >= 2 && State <= 6)
	{
		// プレイヤーに正面から注目
		Target = { 0.0f,0.0f,0.0f };
		Angle = { 0.0f,DirectX::XMConvertToRadians(-180.0f),0.0f };
		Up = { 0.0f,1.0f,0.0f };
		Focus = Target;
	}

	if(State>=7)
	{
		// プレイヤーに正面から注目
		Target = { 0.0f,0.0f,70.0f };
		Angle = { 0.0f,DirectX::XMConvertToRadians(-90.0f),0.0f };
		Up = { 0.0f,1.0f,0.0f };
		Focus = Target;
	}




	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(Angle.x, Angle.y, Angle.z);

	DirectX::XMFLOAT3 flont;
	DirectX::XMStoreFloat3(&flont, transform.r[2]);

	DirectX::XMFLOAT3 eye{};
	constexpr float range = 35.0f;
	eye.x = Focus.x - (flont.x * range);
	eye.y = Focus.y - (flont.y * range);
	eye.z = Focus.z - (flont.z * range);

	SetLookAtLH(eye, Target, Up);

}

void Camera::Shake(float elapsedTime, DirectX::XMFLOAT3& eye)
{
	if (camera_shake == false) return;

	// カメラの向きに合わせて揺れを決定
	using camera_dir				= GameSystem::CAMERA_DIRECTION;
	const camera_dir now_camera_dir = GameSystem::Instance().GetCameraDirection();
	(now_camera_dir == camera_dir::SIDE) ? ShakeZY()
                                        : ShakeZX();

	shake_timer += -1 * elapsedTime;

	if (shake_timer < 0.0f) ShakeInit();


	// シェイク分の加算     ※基本 0
	eye.x += shake_power.x;
	eye.y += shake_power.y;
	eye.z += shake_power.z;
}


void Camera::ShakeZY()
{
	constexpr float shake_value_range_Z = 15.0f;    // z軸 15の幅 (-7.5f ~ 7.5f)
	constexpr float shake_value_range_Y = 5.0f;     // y軸  5の幅 (-2.5f ~ 2.5f)

	// 0x7fff = 定数 RAND_MAX
	float rand_max = 1.0f / (float)0x7fff;

	// 実数値で乱数生成
	//shake_power.x = (float)rand() * rand_max * shake_value_range_X - shake_value_range_X * 0.5f;
	shake_power.y = (float)rand() * rand_max * shake_value_range_Y - shake_value_range_Y * 0.5f;
	shake_power.z = (float)rand() * rand_max * shake_value_range_Z - shake_value_range_Z * 0.5f;

}


void Camera::ShakeZX()
{
	constexpr float shake_value_range_Z = 15.0f;    // z軸 15の幅 (-7.5f ~ 7.5f)
	constexpr float shake_value_range_X = 5.0f;     // x軸  5の幅 (-2.5f ~ 2.5f)

	// 0x7fff = 定数 RAND_MAX
	float rand_max = 1.0f / (float)0x7fff;

	// 実数値で乱数生成
	shake_power.x = (float)rand() * rand_max * shake_value_range_X - shake_value_range_X * 0.5f;
	//shake_power.y = (float)rand() * rand_max * shake_value_range_Y - shake_value_range_Y * 0.5f;
	shake_power.z = (float)rand() * rand_max * shake_value_range_Z - shake_value_range_Z * 0.5f;

}


void Camera::ShakeInit()
{
	camera_shake = false;
	shake_timer = DEFAULT_SHAKE_TIMER;
	shake_power = {};
}


void Camera::DemoShake(float elapsedTime, DirectX::XMFLOAT3& eye)
{
	if (demo_camera_shake == false) return;

	eye = origin_demo_eye;

	ShakeXY(elapsedTime);


	demo_shake_timer += -1 * elapsedTime;


	if (demo_shake_timer < 0.0f) DemoShakeInit();


	// シェイク分の加算     ※基本 0
	eye.x += demo_shake_power.x;
	eye.y += demo_shake_power.y;
	eye.z += demo_shake_power.z;
}


void Camera::ShakeXY(float elapsedTime)
{
	constexpr float shake_value_range_X = 0.2f;    // x軸 0.2の幅 (-0.1f  ~ 0.1f)
	constexpr float shake_value_range_Y = 0.1f;    // y軸 0.1の幅 (-0.05f ~ 0.05f)

	const float ratio = 1.0f - (demo_shake_timer / DEFAULT_DEMO_SHAKE_TIMER);
	//const float pow_ratio = Calcu3D::Pow(ratio, 3);

	demo_shake_range.x = Calcu3D::LerpFloat(0.0f, shake_value_range_X, /*pow_*/ratio);
	demo_shake_range.y = Calcu3D::LerpFloat(0.0f, shake_value_range_Y, /*pow_*/ratio);



	// 0x7fff = 定数 RAND_MAX
	float rand_max = 1.0f / (float)0x7fff;

	// 実数値で乱数生成
	demo_shake_power.x = (float)rand() * rand_max * demo_shake_range.x - demo_shake_range.x * 0.5f;
	demo_shake_power.y = (float)rand() * rand_max * demo_shake_range.y - demo_shake_range.y * 0.5f;
	//demo_shake_power.z = (float)rand() * rand_max * shake_value_range_Z - shake_value_range_Z * 0.5f;

}


void Camera::DemoShakeInit()
{
	demo_camera_shake = false;
	demo_shake_timer = DEFAULT_DEMO_SHAKE_TIMER;
	demo_shake_range = {};
	demo_shake_power = {};
	origin_demo_eye = {};
}


void Camera::AttentionPlayer(float elapsedTime)
{
	const float r = 3.5f * elapsedTime;

    switch (GameSystem::Instance().GetCameraDirection())
    {
    case GameSystem::CAMERA_DIRECTION::SIDE:
		if (attentionPlayer)
		{
			Target.x = Calcu3D::LerpFloat(Target.x, targetX+30.0f, r);
			Target.y = Calcu3D::LerpFloat(Target.y, playerPos.y+5.0f, r);
			Target.z = Calcu3D::LerpFloat(Target.z, playerPos.z+5.0f, r);

			Eye.x = Calcu3D::LerpFloat(Eye.x, playerPos.x, r);
			Eye.y = Calcu3D::LerpFloat(Eye.y, playerPos.y, r);
			Eye.z = Calcu3D::LerpFloat(Eye.z, playerPos.z+20.0f, r);
			Focus.z= Calcu3D::LerpFloat(Focus.z, 20.0f, r);

		}
		else
		{
			Target.x = Calcu3D::LerpFloat(Target.x, 0.0f, r);
			Target.y = Calcu3D::LerpFloat(Target.y, 0.0f, r);
			Target.z = Calcu3D::LerpFloat(Target.z, 0.0f, r);

			Eye.y = Calcu3D::LerpFloat(Eye.y, 0.0f, r);
			Eye.z = Calcu3D::LerpFloat(Eye.z, 0.0f, r);

		}
		break;
    case GameSystem::CAMERA_DIRECTION::TOP:

		if (attentionPlayer)
		{
			Target.x = Calcu3D::LerpFloat(Target.x, targetX + 30.0f, r);
			Target.y = Calcu3D::LerpFloat(Target.y, playerPos.y + 5.0f, r);
			Target.z = Calcu3D::LerpFloat(Target.z, playerPos.z + 5.0f, r);

			Eye.x = Calcu3D::LerpFloat(Eye.x, playerPos.x, r);
			Eye.y = Calcu3D::LerpFloat(Eye.y, playerPos.y, r);
			Eye.z = Calcu3D::LerpFloat(Eye.z, playerPos.z + 20.0f, r);
			Focus.z = Calcu3D::LerpFloat(Focus.z, 20.0f, r);

			Angle.x = Calcu3D::LerpFloat(Angle.x, 0.0f, r);
			Angle.y = Calcu3D::LerpFloat(Angle.y, -1.571f, r);

			// ＋Y軸がカメラの上方向になるように矯正
			Up = Calcu3D::LerpFloat3(DirectX::XMFLOAT3{ 0.0f,1.000001f,0.0f }, DirectX::XMFLOAT3{ 0.0f,0.0f,1.00000001f }, r);
		}
		else
		{
			Target.x = Calcu3D::LerpFloat(Target.x, 0.0f, r);
			Target.y = Calcu3D::LerpFloat(Target.y, 0.0f, r);
			Target.z = Calcu3D::LerpFloat(Target.z, 0.0f, r);

			Eye.y = Calcu3D::LerpFloat(Eye.y, 0.0f, r);
			Eye.z = Calcu3D::LerpFloat(Eye.z, 0.0f, r);
			if (!changeCameraDir)
			{
				Angle.x = Calcu3D::LerpFloat(Angle.x, 1.571f, r);
				Angle.y = Calcu3D::LerpFloat(Angle.y, 0.0f, r);
				// ＋Z軸がカメラの上方向になるように矯正
				Up = Calcu3D::LerpFloat3(DirectX::XMFLOAT3{ 0.0f,0.0f,1.00000001f },
					DirectX::XMFLOAT3{ 0.0f,1.000001f,0.0f }, r);
			}
		}

		break;
    default:
		break;;
    }


}

void Camera::AttentionBoss(float elapsedTime)
{
	switch (GameSystem::Instance().GetCameraDirection())
	{
    case GameSystem::CAMERA_DIRECTION::SIDE:
		Target = Calcu3D::LerpFloat3(Target, BossPosition, 10.0f * elapsedTime);
		Angle.y = Calcu3D::LerpFloat(Angle.y, 0.0f, 10.0f * elapsedTime);
		break;
    case GameSystem::CAMERA_DIRECTION::TOP:
		Target = Calcu3D::LerpFloat3(Target, BossPosition, 10.0f * elapsedTime);
		Angle.x = Calcu3D::LerpFloat(Angle.x, 0.0f, 10.0f * elapsedTime);

		break;
    default: ;
	}
}



//--------------------<縦シューティングか横シューティングか切り替える>--------------------//
void Camera::ChangeCameraDir(float elapsedTime)
{
	if(changeCameraDir)
	{
		if(GameSystem::Instance().GetCameraDirection()==GameSystem::CAMERA_DIRECTION::SIDE)
		{
			ChangeCameraDirToUp(elapsedTime);
		}
		else if (GameSystem::Instance().GetCameraDirection() == GameSystem::CAMERA_DIRECTION::TOP)
		{
			ChangeCameraDirToSide(elapsedTime);
		}
	}
}

void Camera::ChangeCameraDirToSide(float elapsedTime)//横スクロールへ
{
	DirectX::XMFLOAT3 eyeTarget = playerPos;
	float ratioSpeed = 2.0f * elapsedTime;
	static float ratio = 0.0f;

	switch(changeCameraDirState)
	{
	case 0://playerの後ろへ
	{
			changeCameraDirState++;

		break;
	}
	case 1://side viewへ
	{
		Target.x = Calcu3D::LerpFloat(Target.x, 0.0f, ratio);
		Target.y = Calcu3D::LerpFloat(Target.y, 0.0f, ratio);
		Target.z = Calcu3D::LerpFloat(Target.z, 0.0f, ratio);

		Eye.x = Calcu3D::LerpFloat(Eye.x, eyeSide.x, ratio);
		Eye.y = Calcu3D::LerpFloat(Eye.y, eyeSide.y, ratio);
		Eye.z = Calcu3D::LerpFloat(Eye.z, eyeSide.z, ratio);

		Angle.y = Calcu3D::LerpFloat(Angle.y, DirectX::XMConvertToRadians(-90.0f), ratio);//90°->0°へ


		Angle.x = Calcu3D::LerpFloat(DirectX::XMConvertToRadians(90.0f), 0.0f, ratio);//90°->0°へ
	        // ＋Z軸がカメラの上方向になるように矯正
		Up = Calcu3D::LerpFloat3(DirectX::XMFLOAT3{ 0.0f,0.0f,1.00000001f },
			DirectX::XMFLOAT3{ 0.0f,1.000001f,0.0f }, ratio);

		ratio += ratioSpeed;

		if (ratio >= 1.0f)
		{
			changeCameraDirState++;
			ratio = ratioSpeed;
		}
		break;
	}
	case 2:
	{
		GameSystem::Instance().SetCameraDirection(GameSystem::CAMERA_DIRECTION::SIDE);

		Eye = eyeSide;
		Target = { 0,0,0 };
		Angle.y = DirectX::XMConvertToRadians(-90.0f);
		Angle.x = 0.0f;
		changeCameraDir = false;
		changeCameraDirState = 0;
		GameSystem::Instance().SetChangingView(false);
		break;
	}
	}
}
void Camera::ChangeCameraDirToUp(float elapsedTime)//縦スクロールへ
{
	DirectX::XMFLOAT3 eyeTarget = playerPos;
    float ratioSpeed = 2.0f*elapsedTime;
	static float ratio = 0.0f;

	switch (changeCameraDirState)
	{
	case 0://playerの後ろへ
	{
		Target.x = Calcu3D::LerpFloat(Target.x, 0.0f, ratio);
		Target.y = Calcu3D::LerpFloat(Target.y, 0.0f, ratio);
		Target.z = Calcu3D::LerpFloat(Target.z, 0.0f, ratio);

		Eye.x = Calcu3D::LerpFloat(Eye.x, eyeTop.x, ratio);
		Eye.y = Calcu3D::LerpFloat(Eye.y, eyeTop.y - 0.0001f, ratio);
		Eye.z = Calcu3D::LerpFloat(Eye.z, eyeTop.z, ratio);

		Angle.x = Calcu3D::LerpFloat(0.0f, DirectX::XMConvertToRadians(90.0f), ratio);//90°->0°へ
			// ＋Y軸がカメラの上方向になるように矯正
		Up = Calcu3D::LerpFloat3(DirectX::XMFLOAT3{ 0.0f,1.000001f,0.0f }, DirectX::XMFLOAT3{ 0.0f,0.0f,1.00000001f }, ratio);

		ratio += ratioSpeed;

		if (ratio >= 1.0f)
		{
			changeCameraDirState++;
			ratio = ratioSpeed;
		}
		break;
	}
	case 1://top viewへ
	{

		changeCameraDirState++;
		break;
	}
	case 2:
	{
		GameSystem::Instance().SetCameraDirection(GameSystem::CAMERA_DIRECTION::TOP);

		Eye = eyeTop;
		Target = { 0,0,0 };
		Angle.x = DirectX::XMConvertToRadians(90.0f);
		Angle.y = 0.0f;
		changeCameraDir = false;
		changeCameraDirState = 0;
		GameSystem::Instance().SetChangingView(false);
		break;
	}
	}

}

/*#if 1
	Eye = { 0.0f,-10.0f,20.0f };//縦スクロール
	Angle.x = DirectX::XMConvertToRadians(90.0f);//縦スクロール
#else
	Eye = { 0.0f,20.0f,-10.0f };//横スクロール
	Angle.y = DirectX::XMConvertToRadians(-90.0f);//横スクロール*/