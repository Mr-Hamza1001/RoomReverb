/*
  ==============================================================================

    ProcessReflections.cpp
    Created: 25 Jul 2024 10:28:05am
    Author:  jstan

  ==============================================================================
*/

#include "ProcessReflections.h"
#include "Spherical.h"
#include "SharedData.h"
#include <iostream>
#include <vector>
#include <algorithm>

ProcessReflections::ProcessReflections() : juce::Thread("ProcessReflections") {}

void ProcessReflections::run()
{
    // Your thread code here
    DBG("Process Reflections Thread is running...");
    //juce::Thread::sleep(1000); // Sleep for 1 second

    roomSetup();
    processRoom();

}

ProcessReflections::~ProcessReflections()
{
}

void ProcessReflections::roomSetup()
{
    auto& sharedData = SharedDataSingleton::getInstance();
    std::lock_guard<std::mutex> lock(sharedData.vectorMutex);

    roomPos = sharedData.roomPos;
    roomSize = sharedData.roomSize;
    listenerPos = sharedData.listenerPos;
	listenerSize = sharedData.listenerSize;
	soundSourcePos = sharedData.soundSourcePos;

	//Room model translations
	modelRoom = modelRoom.translation(roomPos);
	modelRoom = modelRoom.scaled(roomSize);
	modelRoom = modelRoom.transpose();

	//Listener model translations
	modelListener = modelListener.translation(listenerPos);
	modelListener = modelListener.scaled(listenerSize);
	modelListener = modelListener.transpose();

	//Construct a set of vertices from the OpenGL room vertices
	boxVertices = sharedData.floor;
	boxVertices.insert(boxVertices.end(), sharedData.walls.begin(), sharedData.walls.end());
	boxVertices.insert(boxVertices.end(), sharedData.ceiling.begin(), sharedData.ceiling.end());

	speedOfSound = sharedData.speedOfSound;
	additionalRays = sharedData.additionalRays;
}

void ProcessReflections::processRoom()
{
    // Your method implementation
    DBG("Process Room method called from thread!");

	//Open CSV file for writing
	cSVFile.open("data_dump.csv");

	Ray ray;
	random.setSeed(1);

	/***************************************************************/
	//Pass 1
	/***************************************************************/
	float polar, azimuth;
	juce::Vector3D<float> zeroVector = juce::Vector3D<float>(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 2 * POLAR_SUBDIVISIONS; i++) { //azimuth
		for (int j = 0; j < POLAR_SUBDIVISIONS; j++) { //polar
			//polar = ((float)M_PI * j) / POLAR_SUBDIVISIONS;
			//azimuth = ((float)M_PI * i) / POLAR_SUBDIVISIONS;
			polar = (juce::MathConstants<float>::pi / 2) - asin(1 - 2 * random.nextFloat());
			azimuth = random.nextFloat() * 2.0 * juce::MathConstants<float>::pi;
			Spherical rayDirectionS(1.0f, azimuth, polar);
			Cartesian rayDirectionC = rayDirectionS.sph_to_car();
			juce::Vector3D<float>rayDirection = juce::Vector3D<float>(rayDirectionC.get_x(), rayDirectionC.get_y(), rayDirectionC.get_z());
			rayVectors[i][j][0][0] = soundSourcePos;
			rayVectors[i][j][0][1] = rayDirection;

			//cSVFile << i << "," << j << "," << "0" << ",";
			//cSVFile << rayVectors[i][j][0][0].x << "," << rayVectors[i][j][0][0].y << "," << rayVectors[i][j][0][0].z << ",";
			//cSVFile << rayVectors[i][j][0][1].x << "," << rayVectors[i][j][0][1].y << "," << rayVectors[i][j][0][1].z << "\n";
		}
	}

	juce::Vector3D<float> rayPoint, rayDirection, rayNormal, rayReflect;
	for (int i = 0; i < 2 * POLAR_SUBDIVISIONS; i++) { //azimuth
		for (int j = 0; j < POLAR_SUBDIVISIONS; j++) { //polar
			for (int k = 1; k < NUM_REFLECTIONS; k++) {
				ray.origin = rayVectors[i][j][k - 1][0];
				ray.direction = rayVectors[i][j][k - 1][1];

				//Perform ray cast with listener box
				// Set initial distance to something far, far away.
				float result = FLT_MAX;

				// Traverse triangle list and find the intersecting triangles.
				float distance = 0.0f;
				bool test = false;
				rayNormal = zeroVector;
				rayReflect = zeroVector;

				juce::Vector3D<float> v0, v1, v2;
				int index;
				juce::Vector3D<float>intersect;
				juce::Vector3D<float> pos;
				//Build and transform triangle vertices
				for (size_t l = 0; l < 36; l += 3)
				{
					index = boxIndices[l + 0];
					v0.x = boxVertices[index * 6 + 0];
					v0.y = boxVertices[index * 6 + 1];
					v0.z = boxVertices[index * 6 + 2];

					index = boxIndices[l + 1];
					v1.x = boxVertices[index * 6 + 0];
					v1.y = boxVertices[index * 6 + 1];
					v1.z = boxVertices[index * 6 + 2];

					index = boxIndices[l + 2];
					v2.x = boxVertices[index * 6 + 0];
					v2.y = boxVertices[index * 6 + 1];
					v2.z = boxVertices[index * 6 + 2];

					// Transform triangle to world/listener space.
					transformVector(v0, modelListener);
					transformVector(v1, modelListener);
					transformVector(v2, modelListener);

					Triangle triangle;
					triangle.v0 = v0;
					triangle.v1 = v1;
					triangle.v2 = v2;


					//Test to see if the ray intersects this triangle.
					test = intersectRayTriangle(ray, triangle, distance, intersect);
					if (test) {
						// Keep the result if it's closer than any intersection we've had so far.
						if (distance > 1e-4)
						{
							result = distance;
							pos = intersect;
							//rayNormal = ((v1 - v0) ^ (v2 - v0)).normalised();
						}
					}

				}

				if (result < FLT_MAX) {
					//Store listener intersection data
					listenerVectors[i][j][k-1][0] = pos;
					listenerVectors[i][j][k-1][1] = ray.direction.normalised();

					//cSVFile << i << "," << j << "," << k << ",";
					//cSVFile << listenerVectors[i][j][k][0].x << "," << listenerVectors[i][j][k][0].y << "," << listenerVectors[i][j][k][0].z << ",";
					//cSVFile << listenerVectors[i][j][k][1].x << "," << listenerVectors[i][j][k][1].y << "," << listenerVectors[i][j][k][1].z << "\n";
				}
				else
				{
					listenerVectors[i][j][k-1][0] = zeroVector;
					listenerVectors[i][j][k-1][1] = zeroVector;
				}

				//Perform ray cast with the room
				// Set initial distance to something far, far away.
				result = FLT_MAX;

				// Traverse triangle list and find the intersecting triangles.
				distance = 0.0f;
				test = false;
				rayNormal = zeroVector;
				rayReflect = zeroVector;

				for (size_t l = 0; l < 36; l += 3)
				{
					index = boxIndices[l + 0];
					v0.x = boxVertices[index * 6 + 0];
					v0.y = boxVertices[index * 6 + 1];
					v0.z = boxVertices[index * 6 + 2];

					index = boxIndices[l + 1];
					v1.x = boxVertices[index * 6 + 0];
					v1.y = boxVertices[index * 6 + 1];
					v1.z = boxVertices[index * 6 + 2];

					index = boxIndices[l + 2];
					v2.x = boxVertices[index * 6 + 0];
					v2.y = boxVertices[index * 6 + 1];
					v2.z = boxVertices[index * 6 + 2];

					// Transform triangle to world/listener space.
					transformVector(v0, modelRoom);
					transformVector(v1, modelRoom);
					transformVector(v2, modelRoom);

					Triangle triangle;
					triangle.v0 = v0;
					triangle.v1 = v1;
					triangle.v2 = v2;

						
					//Test to see if the ray intersects this triangle.
					test = intersectRayTriangle(ray, triangle, distance, intersect);
					if (test) {
						// Keep the result if it's closer than any intersection we've had so far.
						if (distance > 1e-4)
						{
							result = distance;
							pos = intersect;
							rayNormal = ((v1 - v0) ^ (v2 - v0)).normalised();
						}
					}

				}

				if (result < FLT_MAX) {
					rayReflect = reflect(ray.direction.normalised(), rayNormal);

					//juce::Vector3D<float> pos = intersect;
					rayVectors[i][j][k][0] = pos;
					rayVectors[i][j][k][1] = rayReflect;

				}
				//console() << resultP << ", " << rayNormal << endl;

				//cSVFile << i << "," << j << "," << k << ",";
				//cSVFile << rayVectors[i][j][k][0].x << "," << rayVectors[i][j][k][0].y << "," << rayVectors[i][j][k][0].z << ",";
				//cSVFile << rayVectors[i][j][k][1].x << "," << rayVectors[i][j][k][1].y << "," << rayVectors[i][j][k][1].z << "\n";
				//cSVFile << rayNormal.x << "," << rayNormal.y << "," << rayNormal.z << "\n";
			}
		}
	}

	//Calculate distances ray has travelled and number of reflections when it hits the listener box to get impulse response
	//Check to see if any reflections pass through sphere during entire path
	double accDistance = 0.0;
	double listenerDistance = 0.0;
	bool bReflectionCaught = false;
	juce::Vector3D<float> vd;
	for (int i = 0; i < 2 * POLAR_SUBDIVISIONS; i++) //azimuth
	{
		for (int j = 0; j < POLAR_SUBDIVISIONS; j++) //polar
		{
			bReflectionCaught = false;
			for (int k = 0; k < NUM_REFLECTIONS; k++) 
			{
				listenerDistances[i][j][k][0] = 0.0f; //zero out value to start with
				if (listenerVectors[i][j][k][0].x != 0.0f && listenerVectors[i][j][k][0].y != 0.0f && listenerVectors[i][j][k][0].z != 0.0f) 
				{
					bReflectionCaught = true;
				}
			}

			if (bReflectionCaught) {
				//Add up distances, and if present, add to distance array
				accDistance = 0.0;
				for (int k = 0; k < NUM_REFLECTIONS - 1; k++) 
				{
					if (listenerVectors[i][j][k][0].x != 0.0f && listenerVectors[i][j][k][0].y != 0.0f && listenerVectors[i][j][k][0].z != 0.0f) 
					{
						vd = listenerVectors[i][j][k][0] - rayVectors[i][j][k][0];
						listenerDistance = vd.length();
						listenerDistances[i][j][k][0] = accDistance + listenerDistance;
					}
					vd = rayVectors[i][j][k][0] - rayVectors[i][j][k + 1][0];
					accDistance += vd.length();
				}
			}
		}
	}

	//Determine contents of listener array, populated with delay time (ms) and attenuation
	int count = 0;
	for (int i = 0; i < 2 * POLAR_SUBDIVISIONS; i++)
	{
		for (int j = 0; j < POLAR_SUBDIVISIONS; j++)
		{
			for (int k = 0; k < NUM_REFLECTIONS; k++)
			{
				if (listenerVectors[i][j][k][0].x != 0.0f && listenerVectors[i][j][k][0].y != 0.0f && listenerVectors[i][j][k][0].z != 0.0f)
				{
					floatListenerArray[count][0] = 0;
					floatListenerArray[count][1] = i;
					floatListenerArray[count][2] = j;
					floatListenerArray[count][3] = k;
					floatListenerArray[count][4] = listenerDistances[i][j][k][0] *1000.0f / speedOfSound; //convert to time delay
					Cartesian dirC(listenerVectors[i][j][k][1].x, -listenerVectors[i][j][k][1].z, -listenerVectors[i][j][k][1].y);
					Spherical dirS = dirC.car_to_sph();
					floatListenerArray[count][5] = dirS.get_theta();
					floatListenerArray[count][6] = dirS.get_phi();

					cSVFile << i << "," << j << "," << k << ",";
					cSVFile << floatListenerArray[count][4] << "," << floatListenerArray[count][5] << "," << floatListenerArray[count][6] << "\n";
					count++;
				}
			}
		}
	}

	/***************************************************************/
	//Pass 2
	/***************************************************************/
	juce::Vector3D<float> originalDirection;
	random2.setSeed(2);
	for (int i = 0; i < count; i++)
	{
		//Get original ray direction
		originalDirection = rayVectors[(int)floatListenerArray[i][1]][(int)floatListenerArray[i][2]][0][1];
		//Convert to Spherical coordinates
		Cartesian origDirC(originalDirection.x, originalDirection.y, originalDirection.z);
		Spherical origDirS = origDirC.car_to_sph();
		for (int j = 0; j < additionalRays; j++)
		{
			//Calculate distribution range from original number of rays
			polar = origDirS.get_phi() + (asin(1 - 2 * random2.nextFloat())) / POLAR_SUBDIVISIONS;
			azimuth = origDirS.get_theta() + (2 * juce::MathConstants<float>::pi * (0.5f - random2.nextFloat())) / (2 * POLAR_SUBDIVISIONS);
			azimuth = fmodf(azimuth, 2 * juce::MathConstants<float>::pi);
			Spherical rayDirectionS(1.0f, azimuth, polar);
			Cartesian rayDirectionC = rayDirectionS.sph_to_car();
			juce::Vector3D<float>rayDirection = juce::Vector3D<float>(rayDirectionC.get_x(), rayDirectionC.get_y(), rayDirectionC.get_z());
			rayVectors2[i][j][0][0] = soundSourcePos;
			rayVectors2[i][j][0][1] = rayDirection;

			//cSVFile << i << "," << j << "," << "0" << ",";
			//cSVFile << rayVectors2[i][j][0][0].x << "," << rayVectors2[i][j][0][0].y << "," << rayVectors2[i][j][0][0].z << ",";
			//cSVFile << rayVectors2[i][j][0][1].x << "," << rayVectors2[i][j][0][1].y << "," << rayVectors2[i][j][0][1].z << "\n";
		}
	}

	for (int i = 0; i < count; i++) { //azimuth
		for (int j = 0; j < additionalRays; j++) { //polar
			for (int k = 1; k < NUM_REFLECTIONS; k++) {
				ray.origin = rayVectors2[i][j][k - 1][0];
				ray.direction = rayVectors2[i][j][k - 1][1];

				//Perform ray cast with listener box
				// Set initial distance to something far, far away.
				float result = FLT_MAX;

				// Traverse triangle list and find the intersecting triangles.
				float distance = 0.0f;
				bool test = false;
				rayNormal = zeroVector;
				rayReflect = zeroVector;

				juce::Vector3D<float> v0, v1, v2;
				int index;
				juce::Vector3D<float>intersect;
				juce::Vector3D<float> pos;
				//Build and transform triangle vertices
				for (size_t l = 0; l < 36; l += 3)
				{
					index = boxIndices[l + 0];
					v0.x = boxVertices[index * 6 + 0];
					v0.y = boxVertices[index * 6 + 1];
					v0.z = boxVertices[index * 6 + 2];

					index = boxIndices[l + 1];
					v1.x = boxVertices[index * 6 + 0];
					v1.y = boxVertices[index * 6 + 1];
					v1.z = boxVertices[index * 6 + 2];

					index = boxIndices[l + 2];
					v2.x = boxVertices[index * 6 + 0];
					v2.y = boxVertices[index * 6 + 1];
					v2.z = boxVertices[index * 6 + 2];

					// Transform triangle to world/listener space.
					transformVector(v0, modelListener);
					transformVector(v1, modelListener);
					transformVector(v2, modelListener);

					Triangle triangle;
					triangle.v0 = v0;
					triangle.v1 = v1;
					triangle.v2 = v2;


					//Test to see if the ray intersects this triangle.
					test = intersectRayTriangle(ray, triangle, distance, intersect);
					if (test) {
						// Keep the result if it's closer than any intersection we've had so far.
						if (distance > 1e-4)
						{
							result = distance;
							pos = intersect;
							//rayNormal = ((v1 - v0) ^ (v2 - v0)).normalised();
						}
					}

				}

				if (result < FLT_MAX) {
					//Store listener intersection data
					listenerVectors2[i][j][k-1][0] = pos;
					listenerVectors2[i][j][k-1][1] = ray.direction.normalised();

					//cSVFile << i << "," << j << "," << k << ",";
					//cSVFile << listenerVectors[i][j][k][0].x << "," << listenerVectors[i][j][k][0].y << "," << listenerVectors[i][j][k][0].z << ",";
					//cSVFile << listenerVectors[i][j][k][1].x << "," << listenerVectors[i][j][k][1].y << "," << listenerVectors[i][j][k][1].z << "\n";
				}
				else
				{
					listenerVectors2[i][j][k-1][0] = zeroVector;
					listenerVectors2[i][j][k-1][1] = zeroVector;
				}

				//Perform ray cast with the room
				// Set initial distance to something far, far away.
				result = FLT_MAX;

				// Traverse triangle list and find the intersecting triangles.
				distance = 0.0f;
				test = false;
				rayNormal = zeroVector;
				rayReflect = zeroVector;

				for (size_t l = 0; l < 36; l += 3)
				{
					index = boxIndices[l + 0];
					v0.x = boxVertices[index * 6 + 0];
					v0.y = boxVertices[index * 6 + 1];
					v0.z = boxVertices[index * 6 + 2];

					index = boxIndices[l + 1];
					v1.x = boxVertices[index * 6 + 0];
					v1.y = boxVertices[index * 6 + 1];
					v1.z = boxVertices[index * 6 + 2];

					index = boxIndices[l + 2];
					v2.x = boxVertices[index * 6 + 0];
					v2.y = boxVertices[index * 6 + 1];
					v2.z = boxVertices[index * 6 + 2];

					// Transform triangle to world/listener space.
					transformVector(v0, modelRoom);
					transformVector(v1, modelRoom);
					transformVector(v2, modelRoom);

					Triangle triangle;
					triangle.v0 = v0;
					triangle.v1 = v1;
					triangle.v2 = v2;


					//Test to see if the ray intersects this triangle.
					test = intersectRayTriangle(ray, triangle, distance, intersect);
					if (test) {
						// Keep the result if it's closer than any intersection we've had so far.
						if (distance > 1e-4)
						{
							result = distance;
							pos = intersect;
							rayNormal = ((v1 - v0) ^ (v2 - v0)).normalised();
						}
					}

				}

				if (result < FLT_MAX) {
					rayReflect = reflect(ray.direction.normalised(), rayNormal);

					//juce::Vector3D<float> pos = intersect;
					rayVectors2[i][j][k][0] = pos;
					rayVectors2[i][j][k][1] = rayReflect;

				}
				//console() << resultP << ", " << rayNormal << endl;

				//cSVFile << i << "," << j << "," << k << ",";
				//cSVFile << rayVectors2[i][j][k][0].x << "," << rayVectors2[i][j][k][0].y << "," << rayVectors2[i][j][k][0].z << ",";
				//cSVFile << rayVectors2[i][j][k][1].x << "," << rayVectors2[i][j][k][1].y << "," << rayVectors2[i][j][k][1].z << "\n";
				//cSVFile << rayNormal.x << "," << rayNormal.y << "," << rayNormal.z << "\n";
			}
		}
	}

	//Calculate distances ray has travelled and number of reflections when it hits the listener box to get impulse response
	//Check to see if any reflections pass through sphere during entire path
	accDistance = 0.0;
	listenerDistance = 0.0;
	bReflectionCaught = false;
	//juce::Vector3D<float> vd;
	for (int i = 0; i < count; i++) //azimuth
	{
		for (int j = 0; j < additionalRays; j++) //polar
		{
			bReflectionCaught = false;
			for (int k = 0; k < NUM_REFLECTIONS; k++)
			{
				listenerDistances2[i][j][k][0] = 0.0f; //zero out value to start with
				if (listenerVectors2[i][j][k][0].x != 0.0f && listenerVectors2[i][j][k][0].y != 0.0f && listenerVectors2[i][j][k][0].z != 0.0f)
				{
					bReflectionCaught = true;
				}
			}

			if (bReflectionCaught) {
				//Add up distances, and if present, add to distance array
				accDistance = 0.0;
				for (int k = 0; k < NUM_REFLECTIONS - 1; k++)
				{
					if (listenerVectors2[i][j][k][0].x != 0.0f && listenerVectors2[i][j][k][0].y != 0.0f && listenerVectors2[i][j][k][0].z != 0.0f)
					{
						vd = listenerVectors2[i][j][k][0] - rayVectors2[i][j][k][0];
						listenerDistance = vd.length();
						listenerDistances2[i][j][k][0] = accDistance + listenerDistance;
					}
					vd = rayVectors2[i][j][k][0] - rayVectors2[i][j][k + 1][0];
					accDistance += vd.length();
				}
			}
		}
	}

	//Determine contents of listener array, populated with delay time (ms) and attenuation
	int count2 = 0;
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < additionalRays; j++)
		{
			for (int k = 0; k < NUM_REFLECTIONS; k++)
			{
				if (listenerVectors2[i][j][k][0].x != 0.0f && listenerVectors2[i][j][k][0].y != 0.0f && listenerVectors2[i][j][k][0].z != 0.0f)
				{
					floatListenerArray2[count2][0] = 0;
					floatListenerArray2[count2][1] = i;
					floatListenerArray2[count2][2] = j;
					floatListenerArray2[count2][3] = k;
					floatListenerArray2[count2][4] = listenerDistances2[i][j][k][0] *1000.0f / speedOfSound;
					Cartesian dirC(listenerVectors2[i][j][k][1].x, -listenerVectors2[i][j][k][1].z, -listenerVectors2[i][j][k][1].y);
					Spherical dirS = dirC.car_to_sph();
					floatListenerArray2[count2][5] = dirS.get_theta();
					floatListenerArray2[count2][6] = dirS.get_phi();

					cSVFile << i << "," << j << "," << k << ",";
					cSVFile << floatListenerArray2[count2][4] << "," << floatListenerArray2[count2][5] << "," << floatListenerArray2[count2][6] << "\n";
					count2++;
				}
			}
		}
	}

	//Generate impulse response from combined listener arrays
	//Copy arrays to a vector
	std::vector<std::vector<float>> listenerVector1, listenerVector2;
	//Copy first array
	for (size_t i = 0; i < sizeof(floatListenerArray) / sizeof(floatListenerArray[0]); ++i) 
	{
		listenerVector1.push_back(std::vector<float>(floatListenerArray[i], floatListenerArray[i] + sizeof(floatListenerArray[i]) / sizeof(float)));
	}

	//Copy second array
	for (size_t i = 0; i < sizeof(floatListenerArray2) / sizeof(floatListenerArray2[0]); ++i) 
	{
		listenerVector2.push_back(std::vector<float>(floatListenerArray2[i], floatListenerArray2[i] + sizeof(floatListenerArray2[i]) / sizeof(float)));
	}

	listenerVector1.resize(count);
	listenerVector2.resize(count2);

	//Close CSV file
	cSVFile.close();


}

bool ProcessReflections::intersectRayTriangle(const Ray& ray, const Triangle& triangle, float& t, juce::Vector3D<float>& intersectionPoint)
{
    const float EPSILON = 1e-8;
    juce::Vector3D<float> edge1 = triangle.v1 - triangle.v0;
    juce::Vector3D<float> edge2 = triangle.v2 - triangle.v0;
    juce::Vector3D<float> h = ray.direction ^ edge2;
    float a = edge1 * h;

    if (a > -EPSILON && a < EPSILON) {
        return false; // This ray is parallel to this triangle.
    }

    float f = 1.0 / a;
    juce::Vector3D<float> s = ray.origin - triangle.v0;
    float u = f * (s * h);

    if (u < 0.0 || u > 1.0) {
        return false;
    }

    juce::Vector3D<float> q = s ^ edge1;
    float v = f * (ray.direction * q);

    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    // At this stage we can compute t to find out where the intersection point is on the line.
    t = f * (edge2 * q);

    if (t > EPSILON) { // ray intersection
        intersectionPoint = ray.origin + ray.direction * t;
        return true;
    }
    else { // This means that there is a line intersection but not a ray intersection.
        return false;
    }
}

juce::Vector3D<float> ProcessReflections::reflect(juce::Vector3D<float> line, juce::Vector3D<float> normal) 
{

	//Reflection equation: d - 2(d.n)n
	return line - (normal * (line * normal)) * (2.0f);
}

void ProcessReflections::transformVector(juce::Vector3D<float>& v, juce::Matrix3D<float> mat)
{
	jgs::Vector4D<float> v4D = jgs::Vector4D<float>(v.x, v.y, v.z, 1.0f);
	v4D = v4D.transformed(v4D, mat);
	v = juce::Vector3D<float>(v4D.x, v4D.y, v4D.z);
}