
#include "dataGenerator.hpp"

ClusterGenerator::ClusterGenerator(int numPoints, int numCentroids, vector<PointKmeans> centroidPositions) {
    this->numPoints = numPoints;
    this->numCentroids = numCentroids;
    this->centroidPositions = centroidPositions;
    if (this->centroidPositions.empty()) {
        generateRandomCentroids();
    }
};


void ClusterGenerator::generateClusters() {
    srand(time(0));
    this->points.clear();

    for (auto& centroid : this->centroidPositions) {
        for (int i = 0; i < numPoints / numCentroids; ++i) {
            PointKmeans point = generatePointAroundCentroid(centroid);
            this->points.push_back(point);
        }
    }
}

void ClusterGenerator::generateRandomCentroids() {
    srand(time(0));
    centroidPositions.clear();
    for (int i = 0; i < numCentroids; ++i) {
        PointKmeans centroid = PointKmeans(double(rand() % 100), double(rand() % 100)); // modulo 100 -> width = height = 100
        centroidPositions.push_back(centroid);
    }
}

PointKmeans ClusterGenerator::generatePointAroundCentroid(PointKmeans& centroid) {
    double radius = (rand() % 100) / 20.0; // random radius from 0 to 5
    double angle = (rand() % 360) * (M_PI / 180.0); // random angle in radians

    double x = centroid.getX() + radius * cos(angle); // calculate x coord
    double y = centroid.getY() + radius * sin(angle); // calculate y coord

    PointKmeans point = PointKmeans(x, y);
    return point;
}