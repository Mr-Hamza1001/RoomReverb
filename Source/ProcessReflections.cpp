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

ProcessReflections::ProcessReflections() : juce::Thread("ProcessReflections") {}

void ProcessReflections::run()
{
    // Your thread code here
    DBG("Process Reflections Thread is running...");
    //juce::Thread::sleep(1000); // Sleep for 1 second

    roomSetup();
    processRoom();
}

void ProcessReflections::roomSetup()
{
    auto& sharedData = SharedDataSingleton::getInstance();
    std::lock_guard<std::mutex> lock(sharedData.vectorMutex);

    roomPos = sharedData.roomPos;
    roomSize = sharedData.roomSize;
    listenerPos = sharedData.listenerPos;
	soundSourcePos = sharedData.soundSourcePos;

	//modelRoom = juce::Matrix3D<float>();
	modelRoom = modelRoom.translation(roomPos);
	modelRoom = modelRoom.scaled(roomSize);
	modelRoom = modelRoom.transpose();

	roomVertices = sharedData.walls;
	roomVertices.insert(roomVertices.end(), sharedData.floor.begin(), sharedData.floor.end());
	roomVertices.insert(roomVertices.end(), sharedData.ceiling.begin(), sharedData.ceiling.end());

}

void ProcessReflections::processRoom()
{
    // Your method implementation
    DBG("Process Room method called from thread!");

	//Open CSV file for writing
	cSVFile.open("data_dump.csv");

	Ray ray;
	random.setSeed(1000);

	float polar, azimuth;
	for (int i = 0; i < 2 * POLAR_SUBDIVISIONS; i++) { //azimuth
		for (int j = 0; j < POLAR_SUBDIVISIONS; j++) { //polar
			//polar = ((float)M_PI * j) / POLAR_SUBDIVISIONS;
			//azimuth = ((float)M_PI * i) / POLAR_SUBDIVISIONS;
			int seed = random.getSeed();
			polar = (juce::MathConstants<float>::pi / 2) - asin(1 - 2 * random.nextFloat());
			azimuth = random.nextFloat() * 2.0 * juce::MathConstants<float>::pi;
			Spherical rayDirectionS(1.0f, azimuth, polar);
			Cartesian rayDirectionC = rayDirectionS.sph_to_car();
			juce::Vector3D<float>rayDirection = juce::Vector3D<float>(rayDirectionC.get_x(), rayDirectionC.get_y(), rayDirectionC.get_z());
			rayVectors[i][j][0][0] = soundSourcePos;
			rayVectors[i][j][0][1] = rayDirection;

			cSVFile << i << "," << j << "," << "0" << ",";
			cSVFile << rayVectors[i][j][0][0].x << "," << rayVectors[i][j][0][0].y << "," << rayVectors[i][j][0][0].z << ",";
			cSVFile << rayVectors[i][j][0][1].x << "," << rayVectors[i][j][0][1].y << "," << rayVectors[i][j][0][1].z << "\n";
		}
	}

	juce::Vector3D<float> rayPoint, rayDirection, rayNormal, rayReflect;
	for (int i = 0; i < 2 * POLAR_SUBDIVISIONS; i++) { //azimuth
		for (int j = 0; j < POLAR_SUBDIVISIONS; j++) { //polar
			for (int k = 1; k < NUM_REFLECTIONS; k++) {
				ray.origin = rayVectors[i][j][k - 1][0];
				ray.direction = rayVectors[i][j][k - 1][1];

				//Perform the line cast with the room
				// Set initial distance to something far, far away.
				float result = FLT_MAX;

				// Traverse triangle list and find the intersecting triangles.
				const size_t polycountR = roomVertices.size();

				float distance = 0.0f;
				bool test = false;
				rayNormal = juce::Vector3D<float>(0.0f, 0.0f, 0.0f);
				rayReflect = juce::Vector3D<float>(0.0f, 0.0f, 0.0f);

				juce::Vector3D<float> v0, v1, v2;
				int index;
				juce::Vector3D<float>intersect;
				//triMeshRoom->getTriangleVertices(l, &v0, &v1, &v2);
				for (size_t l = 0; l < 36; l += 3)
				{
					index = roomIndices[l + 0];
					v0.x = roomVertices[index * 6 + 0];
					v0.y = roomVertices[index * 6 + 1];
					v0.z = roomVertices[index * 6 + 2];

					index = roomIndices[l + 1];
					v1.x = roomVertices[index * 6 + 0];
					v1.y = roomVertices[index * 6 + 1];
					v1.z = roomVertices[index * 6 + 2];

					index = roomIndices[l + 2];
					v2.x = roomVertices[index * 6 + 0];
					v2.y = roomVertices[index * 6 + 1];
					v2.z = roomVertices[index * 6 + 2];



					// Transform triangle to world/listener space.
					jgs::Vector4D<float> v04D = jgs::Vector4D<float>(v0.x, v0.y, v0.z, 1.0f);
					v04D = v04D.transformed(v04D, modelRoom);
					v0 = juce::Vector3D<float>(v04D.x, v04D.y, v04D.z);

					jgs::Vector4D<float> v14D = jgs::Vector4D<float>(v1.x, v1.y, v1.z, 1.0f);
					v14D = v14D.transformed(v14D, modelRoom);
					v1 = juce::Vector3D<float>(v14D.x, v14D.y, v14D.z);

					jgs::Vector4D<float> v24D = jgs::Vector4D<float>(v2.x, v2.y, v2.z, 1.0f);
					v24D = v24D.transformed(v24D, modelRoom);
					v2 = juce::Vector3D<float>(v24D.x, v24D.y, v24D.z);

					Triangle triangle;
					triangle.v0 = v0;
					triangle.v1 = v1;
					triangle.v2 = v2;

						
					//Test to see if the ray intersects this triangle.
					test = intersectRayTriangle(ray, triangle, distance, intersect);
					if (test) {
						// Keep the result if it's closer than any intersection we've had so far.
						if (distance > 0.0001f) {
							result = distance;
							rayNormal = ((v1 - v0) ^ (v2 - v0)).normalised();
						}
					}
					//console() << test << ", " << resultP << ", " << distance << endl;

					//cSVFile << i << "," << j << "," << k << ",";
					//cSVFile << v0.x << "," << v0.y << "," << v0.z << ",";
					//cSVFile << v1.x << "," << v1.y << "," << v1.z << ",";
					//cSVFile << v2.x << "," << v2.y << "," << v2.z << ",";
					//cSVFile << rayNormal.x << "," << rayNormal.y << "," << rayNormal.z << "\n";
					//cSVFile << test << "," << distance << "\n";
				}
				//console() << endl;


				if (result < FLT_MAX) {
					rayReflect = reflect(ray.direction.normalised(), rayNormal);

					juce::Vector3D<float> pos = intersect;
					rayVectors[i][j][k][0] = pos;
					rayVectors[i][j][k][1] = rayReflect;

				}
				//console() << resultP << ", " << rayNormal << endl;

				cSVFile << i << "," << j << "," << k << ",";
				//cSVFile << ray.direction.x << "," << ray.direction.y << "," << ray.direction.z << ",";
				//cSVFile << ray.origin.x << "," << ray.origin.y << "," << ray.origin.z << ",";
				//cSVFile << rayNormal.x << "," << rayNormal.y << "," << rayNormal.z << ",";
				//cSVFile << test << "," << distance << "," << "\n";
				cSVFile << rayVectors[i][j][k][0].x << "," << rayVectors[i][j][k][0].y << "," << rayVectors[i][j][k][0].z << ",";
				cSVFile << rayVectors[i][j][k][1].x << "," << rayVectors[i][j][k][1].y << "," << rayVectors[i][j][k][1].z << ",";
				cSVFile << rayNormal.x << "," << rayNormal.y << "," << rayNormal.z << "\n";
			}
		}
	}
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


