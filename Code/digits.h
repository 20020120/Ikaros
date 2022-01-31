#pragma once

#include <cassert>
#include <vector>
#include "User_Function.h"



// 数値クラス ※テンプレートクラス

template <typename T>
class Digits final
{
public:
    Digits(T default_start_digits_/*初期値*/, size_t max_digits_place_/*最大桁数*/) : default_start_digits(default_start_digits_), max_digits_place(max_digits_place_) {};
    ~Digits() = default;

public: // Get関数
    [[nodiscard]] T GetDigit() { return digit; }

public: // Set関数
    void Reset()
    {
        digit = default_start_digits;
    }

    void Add(const T& add)
    {
        if (add < 0) return;

        digit += add;
    }

    void Subtract(const T& subtract)
    {
        if (subtract > 0) return;

        digit += subtract;
    }

    void Set(const T& set)
    {
        // 安全装置なし、必要な場合はこの関数の呼び出し前に制限処理を追加する事

        digit = set;
    }

public:
    // 桁ごとに数値を分解
    void SliceDigits(std::vector<T>& digits_place)
    {
        T digits_ = this->digit;

        // i = 0 なら１の位, i = 1 なら１０の位 ...
        for (size_t i = digits_place.size(); i > 0; --i)
        {
            size_t index = i - 1;
            float f_index = static_cast<float>(index);

            int place = static_cast<int>(static_cast<float>(digits_) * pow(0.1f, f_index));
            digits_place.at(index) = static_cast<T>(place);
            digits_ += static_cast<T>(-digits_place.at(index) * pow(10, f_index));
        }

        if (static_cast<int>(digits_) <= 0) return;
        // 計算結果が０でなければエラー
        assert(false);
    }

public: // 定数
    const T default_start_digits;       // 初期値
    const size_t max_digits_place;      // 最大桁数

protected: // 変数
    T digit = default_start_digits;     // 数値

};