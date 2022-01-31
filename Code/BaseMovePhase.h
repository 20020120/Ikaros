#pragma once

#include <DirectXMath.h>
#include <functional>
#include <map>
#include <memory>

class BaseMove
{
public:
    BaseMove() = default;
    virtual ~BaseMove() = default;

public:
    virtual void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle) = 0;

protected: // 定数の定義
    static constexpr uint64_t max_rush_count = 3;
    static constexpr float turn_z_axis_size = 60.0f;
    static constexpr float turn_y_axis_size = 30.0f;
    
};


class MoveState final
{
public:
    struct Transition final
    {
        BaseMove* next_move = {};                       // 遷移先
        std::function<bool(float)> Is = {};             // 遷移条件
    };

    struct MoveData final
    {
        size_t unique_id = {};                          // 識別番号

        std::unique_ptr<BaseMove> move = {};            // 更新する内容

        std::function<void()> Initialize = {};          // 初期化処理
        std::function<void()> Finalize = {};            // 終了処理

        std::vector<Transition> transitions = {};       // 次の遷移先とその条件
    };

public:
    MoveState() = default;
    ~MoveState() = default;

public:
    void Update(float elapsedTime, DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& angle)
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
        moves[current_move_index].move->Update(elapsedTime, position, angle);
    }

public:
    MoveState& SetMove(BaseMove* move_)
    {
        const int index = static_cast<int>(FindMoveIndex(move_));
        if (index != -1)
        {
            current_move_index = static_cast<size_t>(index);
            return *this;
        }

        TryAddMove(move_);
        current_move_index = moves.size() - 1;

        return *this;
    }

    MoveState& AddTransition(BaseMove* from, BaseMove* to, const std::function<bool(float)>& Is_)
    {

        TryAddMove(to);
        TryAddMove(from).transitions.emplace_back(Transition{ to,Is_ });
        return *this;
    }

    MoveState& SetInitialize(BaseMove* move_, const std::function<void()>& initialize)
    {
        TryAddMove(move_).Initialize = initialize;
        return *this;
    }

    MoveState& SetFinalize(BaseMove* move_, const std::function<void()>& finalize)
    {
        TryAddMove(move_).Finalize = finalize;
        return *this;
    }

    size_t& GetNowMove() { return moves[current_move_index].unique_id; }

private:
    MoveData& TryAddMove(BaseMove* move_)
    {
        const size_t index = FindMoveIndex(move_);
        if(index != -1)
        {
            return moves.at(index);
        }

        std::unique_ptr<BaseMove> move;
        move.reset(move_);
        moves.emplace_back(MoveData{ moves_size++,std::move(move) });
        return moves.back();
    }

    size_t FindMoveIndex(const BaseMove* move)
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