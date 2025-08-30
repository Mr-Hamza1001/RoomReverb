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
#include <cmath>

using namespace std;

/***************************************************************/
// Helper to convert between Cartesian and Spherical coordinates
/***************************************************************/

class Spherical;

class Cartesian
{
private:
	float mx, my, mz;
public:
	Cartesian() {}
	Cartesian(float x, float y, float z) {
		mx = x;
		my = y;
		mz = z;
	}
	float& get_x() {
		return mx;
	}
	float& get_y() {
		return my;
	}
	float& get_z() {
		return mz;
	}
	Spherical car_to_sph();
};

class Spherical
{
private:
	float mr, mtheta, mphi;
public:
	Spherical() {}
	Spherical(float r, float theta, float phi) {
		mr = r;
		mtheta = theta;
		mphi = phi;
	}
	float& get_r() {
		return mr;
	}
	float& get_theta() {
		return mtheta;
	}
	float& get_phi() {
		return mphi;
	}
	Cartesian sph_to_car();
};

Spherical Cartesian::car_to_sph() {
	Spherical temp;
	float r, theta, phi;

	temp.get_r() = (float)sqrt(pow(mx, 2) + pow(my, 2) + pow(mz, 2));
	theta = atan2(my, mx) + juce::MathConstants<float>::pi;
	phi = acos(mz / temp.get_r());

	temp.get_theta() = fmodf(theta, 2 * juce::MathConstants<float>::pi);
	temp.get_phi() = fmodf(phi, juce::MathConstants<float>::pi);

	return temp;
}

Cartesian Spherical::sph_to_car() {
	Cartesian temp;

	mtheta -= juce::MathConstants<float>::pi;
	mtheta = fmodf(mtheta, juce::MathConstants<float>::pi);
	mphi = fmodf(mphi, juce::MathConstants<float>::pi);

	temp.get_x() = mr * cos(mtheta) * sin(mphi);
	temp.get_y() = mr * sin(mtheta) * sin(mphi);
	temp.get_z() = mr * cos(mphi);

	return temp;
}

