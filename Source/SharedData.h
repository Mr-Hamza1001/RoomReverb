/*
  ==============================================================================

    SharedData.h
    Created: 23 Jul 2024 2:44:42pm
    Author:  jstan

  ==============================================================================
*/

#pragma once

#include <atomic>
#include <vector>
#include <juce_core/juce_core.h>

struct SharedData
{
    std::vector<float> someVector;

    std::mutex vectorMutex;

};

class SharedDataSingleton
{
public:
    static SharedData& getInstance()
    {
        static SharedData instance;
        return instance;
    }

private:
    SharedDataSingleton() = default;
    ~SharedDataSingleton() = default;
    SharedDataSingleton(const SharedDataSingleton&) = delete;
    SharedDataSingleton& operator=(const SharedDataSingleton&) = delete;
};