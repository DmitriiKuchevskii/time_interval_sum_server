#pragma once
//
// Created by dmitrii on 1/31/20.
//

#ifndef TEST_TIMEINTERVALDATASTORAGE_H
#define TEST_TIMEINTERVALDATASTORAGE_H

#include <vector>
#include <chrono>
#include <algorithm>
#include <cstring>

using namespace std::chrono;

template <typename DataType>
class TimeIntervalDataStorage
{
public:
    explicit TimeIntervalDataStorage(size_t intervalInMilliseconds = 60  * 1000, size_t maxUnusedSize = 1024) :
        kDataTimeIntervalInMilliseconds(intervalInMilliseconds)
      , kMaxUnusedSize(maxUnusedSize)
    {}

    void put(DataType number)
    {
        size_t timeNow = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        size_t boundTime = timeNow - kDataTimeIntervalInMilliseconds;
        auto it = std::upper_bound(m_data.begin(), m_data.end(), boundTime,
                                   [](const auto& p1, const auto& e2) { return p1 < e2.first; });

        if (it == m_data.end())
        {
            m_curSubstruct = 0;
            m_data = { { timeNow, number } };
            return;
        }
        else if (it != m_data.begin())
        {
            m_curSubstruct = (it - 1)->second;
            if (std::distance(m_data.begin(), it) > kMaxUnusedSize)
            {
                m_data = { it, m_data.end() };
                for (auto& val : m_data)
                    val.second -= m_curSubstruct;
                m_curSubstruct = 0;
            }
        }

        m_data.emplace_back(timeNow, m_data.back().second + number);
    }

    [[nodiscard]] DataType get() const
    {
        return m_data.back().second - m_curSubstruct;
    }

private:
    std::vector<std::pair<size_t, DataType>> m_data;
    const size_t kDataTimeIntervalInMilliseconds = 60 * 1000; // 1 min
    DataType m_curSubstruct =  0;
    const size_t kMaxUnusedSize = 1024; // ~20Kb (SIZE == 1024 * sizeof(std::pair<size_t, size_t>))
};


#endif //TEST_TIMEINTERVALDATASTORAGE_H
