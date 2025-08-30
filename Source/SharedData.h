/*
 * Copyright (c) 2025 James G. Stanier
 *
 * This file is part of RoomReverbPlugin.
 *
 * This software is dual-licensed under:
 *   1. The GNU General Public License v3.0 (GPLv3)
 *   2. A commercial license (contact j.stanier766(at)gmail.com for details)
 *
 * You may use this file under the terms of the GPLv3 as published by
 * the Free Software Foundation. For proprietary/commercial use,
 * please see the LICENSE-COMMERCIAL file or contact the copyright holder.
 */

#pragma once

#include <atomic>
#include <vector>
#include <juce_core/juce_core.h>

struct SharedData
{
    //std::vector<float> someVector;

    juce::Vector3D<float> roomSize, roomPos, listenerPos, listenerSize, soundSourcePos;
    float speedOfSound, rollOff, delayBucketSize;
    int additionalRays, numberPolarBuckets;

    std::vector<float> walls{
        //Position            //Texture    //ID
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
    };

    std::vector<float> floor{
        //Position            //Texture    //ID
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  1.0f,
    };

    std::vector<float> ceiling{
        //Position            //Texture    //ID
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  2.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  2.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  2.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  2.0f,
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