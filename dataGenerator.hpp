
#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <math.h>

#ifndef M_PI 
#define M_PI 3.14159265358979323846
#endif // !M_PI 3.14159265358979323846


#include "kmeans.hpp"
using namespace std;

// Class for generating test data
// Generates clusters as points inside a circle around a centroid
// Works only with 2D space
class ClusterGenerator {
public:
    
    // Generator is initialized with:
    // numPoints: number of Points to generate
    // numCentroids: number of Centroids to generate
    // Optional - centroidPositions: pre-defined centroid positions
    ClusterGenerator(int numPoints, int numCentroids, vector<PointKmeans> centroidPositions = {});

    // generates points around centroids
    // points are split uniformly between centroids
    void generateClusters();

    vector<PointKmeans>& getCentroids() { return this->centroidPositions; };

    vector<PointKmeans>& getPoints(){ return this->points; };


private:
    int numPoints;
    int numCentroids;
    vector<PointKmeans> centroidPositions;
    vector<PointKmeans> points;

    // generates random points (centroids) in a space x = [0, 100], y = [0, 100]
    void generateRandomCentroids();

    // generates a point around a certain centroid
    PointKmeans generatePointAroundCentroid(PointKmeans& centroid);
};

