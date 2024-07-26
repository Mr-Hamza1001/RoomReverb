/*
  ==============================================================================

    ProcessReflections.h
    Created: 25 Jul 2024 10:28:05am
    Author:  jstan

  ==============================================================================
*/

#pragma once
#include <iostream>
#include <fstream>
#include "jgs_Vector4D.h"
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
    void run() override;
    void roomSetup();
    void processRoom();
    bool intersectRayTriangle(const Ray& ray, const Triangle& triangle, float& t, juce::Vector3D<float>& intersectionPoint);
    juce::Vector3D<float> reflect(juce::Vector3D<float> line, juce::Vector3D<float> normal);

private:
    juce::Vector3D<float> roomPos, roomSize, listenerPos, soundSourcePos;
    juce::Matrix3D<float> modelRoom;
    jgs::Vector4D<float> test;

    std::vector<float> roomVertices;
    unsigned int roomIndices[36] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3,   // second triangle
    4, 5, 7,   // first triangle
    5, 6, 7,   // second triangle
    8, 9, 11,   // first triangle
    9, 10, 11,   // second triangle
    12, 13, 15,   // first triangle
    13, 14, 15,    // second triangle
    16, 17, 19,   // first triangle
    17, 18, 19,   // second triangle
    20, 21, 23,   // first triangle
    21, 22, 23,   // second triangle
    };

    std::ofstream cSVFile;

    static const int POLAR_SUBDIVISIONS = 4;
    static const int NUM_REFLECTIONS = 15;
    juce::Vector3D<float> rayVectors[2 * POLAR_SUBDIVISIONS][POLAR_SUBDIVISIONS][NUM_REFLECTIONS][2];
    juce::Vector3D<float> listenerVectors[2 * POLAR_SUBDIVISIONS][POLAR_SUBDIVISIONS][NUM_REFLECTIONS][2];
    juce::Random random;
};