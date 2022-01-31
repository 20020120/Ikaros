#pragma once
#include <DirectXMath.h>
#include <functional>
#include <memory>


// SpaceShipMoveConstant 宇宙船の移動関係の定数
namespace SMC
{
    static constexpr float to_head_length = 70.0f;
    static constexpr float to_back_length = -70.0f;
    static constexpr float to_right_length = 50.0f;
    static constexpr float to_left_length = -50.0f;
}


class BaseShipMove
{
public:
    BaseShipMove() = default;
    virtual ~BaseShipMove() = default;

public:
    virtual void Update(float, DirectX::XMFLOAT3&, DirectX::XMFLOAT3&, float) = 0;

protected: // 定数の定義
};


class ShipMoveState final
{
public:
    struct Transition final
    {
        BaseShipMove* next_move = {};                       // 遷移先
        std::function<bool(float)> Is = {};             // 遷移条件
    };

    struct MoveData final
    {
        size_t unique_id = {};                          // 識別番号

        std::unique_ptr<BaseShipMove> move = {};            // 更新する内容

        std::function<void()> Initialize = {};          // 初期化処理
        std::function<void()> Finalize = {};            // 終了処理

        std::vector<Transition> transitions = {};       // 次の遷移先とその条件
    };

public:
    ShipMoveState() = default;
    ~ShipMoveState() = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle, const float& speed_ratio)
    {
        for (auto& transition : moves[current_move_index].transitions)
        {
            // 遷移条件がそろっていたら遷移
            if (transition.Is(elapsedTime) == false) continue;

            // 現在の行動の終了処理
            if (moves[current_move_index].Finalize) moves[current_move_index].Finalize();
            // 更新する行動の番号に変更
            current_move_index = FindMoveIndex(transition.next_move);
            // 変更した行動の初期化処理
            if (moves[current_move_index].Initialize) moves[current_move_index].Initialize();

            break;
        }

        // 現在の行動で更新する
        moves[current_move_index].move->Update(elapsedTime, position, angle, speed_ratio);
    }

public:
    ShipMoveState& SetMove(BaseShipMove* move_)
    {
        const int index = static_cast<int>(FindMoveIndex(move_));
        if (index != -1)
        {
            current_move_index = static_cast<size_t>(index);
        }

        TryAddMove(move_);
        current_move_index = moves.size() - 1;

        return *this;
    }

    ShipMoveState& AddTransition(BaseShipMove* from, BaseShipMove* to, const std::function<bool(float)>& Is_)
    {

        TryAddMove(to);
        TryAddMove(from).transitions.emplace_back(Transition{ to,Is_ });
        return *this;
    }

    ShipMoveState& SetInitialize(BaseShipMove* move_, const std::function<void()>& initialize)
    {
        TryAddMove(move_).Initialize = initialize;
        return *this;
    }

    ShipMoveState& SetFinalize(BaseShipMove* move_, const std::function<void()>& finalize)
    {
        TryAddMove(move_).Finalize = finalize;
        return *this;
    }

    size_t& GetNowMove() { return moves[current_move_index].unique_id; }

private:
    MoveData& TryAddMove(BaseShipMove* move_)
    {
        const size_t index = FindMoveIndex(move_);
        if (index != -1)
        {
            return moves.at(index);
        }

        std::unique_ptr<BaseShipMove> move;
        move.reset(move_);
        moves.emplace_back(MoveData{ moves_size++,std::move(move) });
        return moves.back();
    }

    size_t FindMoveIndex(const BaseShipMove* move)
    {
        const auto it = std::find_if(moves.begin(), moves.end(), [&move](const MoveData& data) {return move == data.move.get(); });
        if (it == moves.end()) return -1;

        return it - moves.begin();
    }

private:
    size_t moves_size;
    size_t current_move_index;
    std::vector<MoveData> moves;
};