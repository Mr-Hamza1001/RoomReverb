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
    juce::Vector3D<float> roomSize{ 5.0f, 5.0f, 5.0f };

    std::vector<float> walls{
        //Position            //Texture                //ID
        -0.5f, -0.5f, -0.5f,  0.0f,       0.0f,        0.0f,
         0.5f, -0.5f, -0.5f,  roomSize.x, 0.0f,        0.0f,
         0.5f,  0.5f, -0.5f,  roomSize.x, roomSize.y,  0.0f,
         0.5f,  0.5f, -0.5f,  roomSize.x, roomSize.y,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,       roomSize.y,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,       0.0f,        0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
         0.5f, -0.5f,  0.5f,  roomSize.x, 0.0f,        0.0f,
         0.5f,  0.5f,  0.5f,  roomSize.x, roomSize.y,  0.0f,
         0.5f,  0.5f,  0.5f,  roomSize.x, roomSize.y,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,       0.0f,        0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
        -0.5f,  0.5f, -0.5f,  roomSize.z, 0.0f,        0.0f,
        -0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
        -0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        0.0f,

         0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        0.0f,
         0.5f,  0.5f, -0.5f,  roomSize.z, 0.0f,        0.0f,
         0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
         0.5f, -0.5f, -0.5f,  roomSize.z, roomSize.y,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,       roomSize.y,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,       0.0f,  0.0f
    };

    std::vector<float> floor{
        //Position            //Texture                //ID
        -0.5f, -0.5f, -0.5f,  0.0f,       roomSize.z,  1.0f,
         0.5f, -0.5f, -0.5f,  roomSize.x, roomSize.z,  1.0f,
         0.5f, -0.5f,  0.5f,  roomSize.x, 0.0f,        1.0f,
         0.5f, -0.5f,  0.5f,  roomSize.x, 0.0f,        1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,       0.0f,        1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,       roomSize.z,  1.0f
    };

    std::vector<float> ceiling{
        //Position            //Texture                //ID
        -0.5f,  0.5f, -0.5f,  0.0f,       roomSize.z,  2.0f,
         0.5f,  0.5f, -0.5f,  roomSize.x, roomSize.z,  2.0f,
         0.5f,  0.5f,  0.5f,  roomSize.x, 0.0f,        2.0f,
         0.5f,  0.5f,  0.5f,  roomSize.x, 0.0f,        2.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,       0.0f,        2.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,       roomSize.z,  2.0f
    };

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