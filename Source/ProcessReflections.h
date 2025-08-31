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
#include <iostream>
#include <fstream>
#include "jgs_Vector4D.h"
#include "ExMatrix3D.h"
#include <JuceHeader.h>
#include <juce_core/juce_core.h>

struct Ray {
    juce::Vector3D<float> origin, direction;
};

struct Triangle {
    juce::Vector3D<float> v0, v1, v2;
};

class ProcessReflections : public juce::Thread
{
public:
    ProcessReflections();
    ~ProcessReflections();
    void run() override;
    void roomSetup();
    void pass1();
    void pass2();
    void populateIR();

private:
    juce::Vector3D<float> roomPos, roomSize, listenerPos, listenerSize, soundSourcePos;
    ExMatrix3D<float> modelRoom, modelListener;
    int count, count2;

    std::vector<float> boxVertices;
    unsigned int boxIndices[36] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3,   // second triangle
    4, 5, 7,   // first triangle
    5, 6, 7,   // second triangle
    8, 9, 11,   // first triangle
    9, 10, 11,   // second triangle
    12, 13, 15,   // first triangle
    13, 14, 15,   // second triangle
    16, 17, 19,   // first triangle
    17, 18, 19,   // second triangle
    20, 21, 23,   // first triangle
    21, 22, 23,   // second triangle
    };

    std::ofstream cSVFile;

    static const int POLAR_SUBDIVISIONS = 80;
    static const int NUM_REFLECTIONS = 15;
    juce::Vector3D<float> rayVectors[2 * POLAR_SUBDIVISIONS][POLAR_SUBDIVISIONS][NUM_REFLECTIONS][2]{};
    juce::Vector3D<float> listenerVectors[2 * POLAR_SUBDIVISIONS][POLAR_SUBDIVISIONS][NUM_REFLECTIONS][2]{};
    float listenerDistances[2 * POLAR_SUBDIVISIONS][POLAR_SUBDIVISIONS][NUM_REFLECTIONS][1]{}; // azimuth, polar, reflection count, distance
    float floatListenerArray[10000][7]{};
    juce::Vector3D<float> rayVectors2[2000][10][NUM_REFLECTIONS][2]{};
    juce::Vector3D<float> listenerVectors2[2000][10][NUM_REFLECTIONS][2]{};
    float listenerDistances2[2000][10][NUM_REFLECTIONS][1]{}; // azimuth, polar, reflection count, distance
    float floatListenerArray2[100000][7]{};

    juce::Random random, random2;
    float speedOfSound, rollOff, delayBucketSize;
    int additionalRays, numberPolarBuckets;

    bool intersectRayTriangle(const Ray& ray, const Triangle& triangle, float& t, juce::Vector3D<float>& intersectionPoint);
    juce::Vector3D<float> reflect(juce::Vector3D<float> line, juce::Vector3D<float> normal);
    void transformVector(juce::Vector3D<float>& v, ExMatrix3D<float> mat);
};