#pragma once
#include <cassert>
#include<DirectXMath.h>
#include <algorithm>

//実体を削除する関数
template <typename T>
inline void safe_delete(T*& p)
{
    if (p != nullptr)
    {
        delete (p);
        (p) = nullptr;
    }
}


//最大値・最小値に収める関数 
template<typename T>
inline T clamp(T value,const T Max,const T Min)
{
    assert(Max >= Min);
    return value > Max ? Max : value < Min ? Min : value;
}

//累乗を求める関数
template<typename T>
inline T User_pow(T num, int times)
{
    T v = num;
    for (int i = 0; i < times; ++i)
    {
        num *= v;
    }

    return num;
}

namespace Calcu3D
{
    //****************************************************************
    // 
    // A地点からB地点へのベクトルを求める関数
    // 
    //****************************************************************
    inline DirectX::XMVECTOR MakeVectorAtoB(const DirectX::XMFLOAT3& A, const DirectX::XMFLOAT3& B,
        bool isNomalized = false)
    {
        using namespace DirectX;
        XMVECTOR a = XMLoadFloat3(&A);
        XMVECTOR b = XMLoadFloat3(&B);
        XMVECTOR atob = b - a;

        return isNomalized ?
            XMVector3Normalize(atob) : atob;
    }

    template <class T>
    inline T Pow(T num,int Count_Max)
    {
        num = num * num;
        return Count_Max > 0 ? Pow(num, Count_Max - 1) : num;
    }

    //****************************************************************
    // 
    // 正規化
    // 
    //****************************************************************
    //--------------------<Float2>--------------------//
    inline DirectX::XMFLOAT2 Normalize(DirectX::XMFLOAT2 Vec)
    {
        DirectX::XMVECTOR V = DirectX::XMLoadFloat2(&Vec);
        V = DirectX::XMVector2Normalize(V);
        DirectX::XMFLOAT2 Ret{};
        DirectX::XMStoreFloat2(&Ret, V);
        return Ret;
    }

    //--------------------<Float3>--------------------//
    inline DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3 Vec)
    {
        DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&Vec);
        V = DirectX::XMVector3Normalize(V);
        DirectX::XMFLOAT3 Ret{};
        DirectX::XMStoreFloat3(&Ret, V);
        return Ret;
    }

    //--------------------<Float4>--------------------//
    inline DirectX::XMFLOAT4 Normalize(DirectX::XMFLOAT4 Vec)
    {
        DirectX::XMVECTOR V = DirectX::XMLoadFloat4(&Vec);
        V = DirectX::XMVector4Normalize(V);
        DirectX::XMFLOAT4 Ret{};
        DirectX::XMStoreFloat4(&Ret, V);
        return Ret;
    }


    //****************************************************************
    // 
    // 一次元の補完
    // 
    //****************************************************************

#undef min
#undef max
    inline float LerpFloat(float Begin,float End,float Ratio)
    {
        Ratio = std::min(1.0f, std::max(Ratio, 0.0f)); // 補完率を0.0f~1.0fの間に矯正
        return (Begin * (1.0f - Ratio)) + (End * Ratio);
    }

    //****************************************************************
    // 
    // 三次元の補完
    // 
    //****************************************************************

    inline DirectX::XMFLOAT2 LerpFloat2(DirectX::XMFLOAT2 Begin,DirectX::XMFLOAT2 End,float Ratio)
    {
        if (Ratio <= 0.0f) return Begin;
        if (Ratio >= 1.0f) return End;

        DirectX::XMVECTOR B = DirectX::XMLoadFloat2(&Begin);
        DirectX::XMVECTOR E = DirectX::XMLoadFloat2(&End);

        DirectX::XMVECTOR R = DirectX::XMVectorLerp(B, E, Ratio);
        DirectX::XMFLOAT2 Ret{};
        DirectX::XMStoreFloat2(&Ret, R);
        return Ret;
    }

    //****************************************************************
    // 
    // 三次元の補完
    // 
    //****************************************************************

    inline DirectX::XMFLOAT3 LerpFloat3(DirectX::XMFLOAT3 Begin,DirectX::XMFLOAT3 End,float Ratio)
    {
        if (Ratio <= 0.0f) return Begin;
        if (Ratio >= 1.0f) return End;

        DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&Begin);
        DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&End);

        DirectX::XMVECTOR R = DirectX::XMVectorLerp(B, E, Ratio);
        DirectX::XMFLOAT3 Ret{};
        DirectX::XMStoreFloat3(&Ret, R);
        return Ret;
    }

    //****************************************************************
    // 
    // 四次元の補完
    // 
    //****************************************************************
    inline DirectX::XMFLOAT4 LerpFloat4(DirectX::XMFLOAT4 Begin, DirectX::XMFLOAT4 End, float Ratio)
    {
        if (Ratio <= 0.0f) return Begin;
        if (Ratio >= 1.0f) return End;

        DirectX::XMVECTOR B = DirectX::XMLoadFloat4(&Begin);
        DirectX::XMVECTOR E = DirectX::XMLoadFloat4(&End);

        DirectX::XMVECTOR R = DirectX::XMVectorLerp(B, E, Ratio);
        DirectX::XMFLOAT4 Ret{};
        DirectX::XMStoreFloat4(&Ret, R);
        return Ret;
    }


    template<class T>
    inline void Shuffle(std::vector<T> v)
    {
        int i = v.size();
        while (i > 1) {
            const int j = rand() % i;
            --i;
            const T t = v[i];
            v[i] = v[j];
            v[j] = t;
        }
    }


    //--------------------<ベジエ曲線>--------------------//
    inline auto BezierCurve(std::vector<DirectX::XMFLOAT3> ControlPoints,float Ratio) // 最大数は４
    {
        
        // 要素数が4を超えたら止める
        assert(static_cast<int>(ControlPoints.size()) <= 4);

        // もし制御点が一つならそれをかえす
        if(static_cast<int>(ControlPoints.size())<=1)
        {
            return ControlPoints[0];
        }

        // 補完率が0.0f以下の時
        if(Ratio<=0.0f)
        {
            return  ControlPoints[0];
        }
        if (Ratio >= 1.0f)
        {
            return ControlPoints.back();
        }

        std::vector<DirectX::XMFLOAT3> C_Point2{};

        
        for(int i=0;i< static_cast<int>(ControlPoints.size())-1;i++)
        {
            // 始点と終点から補完
            DirectX::XMVECTOR Begin = DirectX::XMLoadFloat3(&ControlPoints[i]);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&ControlPoints[i + 1]);
            DirectX::XMVECTOR L = DirectX::XMVectorLerp(Begin, End, Ratio);
            DirectX::XMFLOAT3 l{};
            DirectX::XMStoreFloat3(&l, L);
            C_Point2.emplace_back(l);
        }

        // もし制御点が一つなら
        if(static_cast<int>(C_Point2.size())<=1)
        {
            return C_Point2[0];
        }

        std::vector<DirectX::XMFLOAT3> C_Point3{};

        for (int i = 0; i < static_cast<int>(C_Point2.size()) - 1; i++)
        {
            // 始点と終点から補完
            DirectX::XMVECTOR Begin = DirectX::XMLoadFloat3(&C_Point2[i]);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&C_Point2[i + 1]);
            DirectX::XMVECTOR L = DirectX::XMVectorLerp(Begin, End, Ratio);
            DirectX::XMFLOAT3 l{};
            DirectX::XMStoreFloat3(&l, L);
            C_Point3.emplace_back(l);
        }

        // もし制御点が一つなら
        if (static_cast<int>(C_Point3.size()) <= 1)
        {
            return C_Point3[0];
        }

      
        // 始点と終点から補完
        DirectX::XMVECTOR Begin = DirectX::XMLoadFloat3(&C_Point3[0]);
        DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&C_Point3[1]);
        DirectX::XMVECTOR L = DirectX::XMVectorLerp(Begin, End, Ratio);
        DirectX::XMFLOAT3 ret{};
        DirectX::XMStoreFloat3(&ret, L);
        return ret;
        

    }


    inline DirectX::XMFLOAT3 ConvertDegreeToRadian(float degX=0.0f, float degY=0.0f, float degZ=0.0f)
    {
        const DirectX::XMFLOAT3 ret
        {
            DirectX::XMConvertToRadians(degX),
            DirectX::XMConvertToRadians(degY),
            DirectX::XMConvertToRadians(degZ)
        };

        return ret;
    }

    //****************************************************************
    // 
    // 内積
    // 
    //****************************************************************

    inline float Float3Dot(const DirectX::XMFLOAT3 A, const DirectX::XMFLOAT3 B)
    {
        const DirectX::XMVECTOR V0 = DirectX::XMLoadFloat3(&A);
        const DirectX::XMVECTOR V1 = DirectX::XMLoadFloat3(&B);
        const DirectX::XMVECTOR Ans = DirectX::XMVector3Dot(V0, V1);
        float ans{};
        DirectX::XMStoreFloat(&ans, Ans);
        return ans;

    }

    inline float Float2Dot(const DirectX::XMFLOAT2 A, const DirectX::XMFLOAT2 B)
    {
        const DirectX::XMVECTOR V0 = DirectX::XMLoadFloat2(&A);
        const DirectX::XMVECTOR V1 = DirectX::XMLoadFloat2(&B);
        const DirectX::XMVECTOR Ans = DirectX::XMVector2Dot(V0, V1);
        float ans{};
        DirectX::XMStoreFloat(&ans, Ans);
        return ans;

    }


    //****************************************************************
    // 
    // 回転値をクランプ
    // 
    //****************************************************************
    inline float ClampRotate(float Radian)
    {
        float ret{ Radian };
        if(Radian>DirectX::XMConvertToRadians(360.0f))
        {
            ret -= DirectX::XMConvertToRadians(360.0f);
        }
        else if(Radian<DirectX::XMConvertToRadians(-360.0f))
        {
            ret += DirectX::XMConvertToRadians(360.0f);
        }

        return ret;
    }


    inline DirectX::XMFLOAT3 GetFrontVec(const DirectX::XMFLOAT3 rad)
    {
        const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(rad.x, rad.y, rad.z);
        DirectX::XMFLOAT3 front{};
        DirectX::XMVector3Normalize(transform.r[2]);
        DirectX::XMStoreFloat3(&front, transform.r[2]);
        return front;
    }


    inline float GetRadianTwoVectors(DirectX::XMFLOAT2 VecA,DirectX::XMFLOAT2 VecB)
    {
        // 正規化する
       const DirectX::XMFLOAT2 NVecA = Calcu3D::Normalize(VecA);
       const DirectX::XMFLOAT2 NVecB = Calcu3D::Normalize(VecB);

        // 内積
        const float dot = Calcu3D::Float2Dot(NVecA, NVecB);
        const float rad = acosf(dot);
        return rad;
    }

}

//モデルの座標系を変換する関数
enum class  WorldCoordinate
{
    RHS_YUP,
    LHS_YUP,
    RHS_ZUP,
    LHS_ZUP

};

inline DirectX::XMFLOAT4X4 f_SetWorldCoordinate(WorldCoordinate type, DirectX::XMMATRIX S, DirectX::XMMATRIX R, DirectX::XMMATRIX T)
{

    const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
       { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    // 0:RHS Y-UP
       { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 1:LHS Y-UP
       { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
       { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
    };

    const float scalefactor = 1.0f;

    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[static_cast<int>(type)])
     * DirectX::XMMatrixScaling(scalefactor, scalefactor, scalefactor) };

    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, C * S * R * T);

    return world;
}