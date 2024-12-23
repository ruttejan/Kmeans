
#pragma once
#include <vector>
#include <random>
#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <atomic>
#include <cstdlib>

using namespace std;

class PointKmeans { 

public:

	// Default constructor
	PointKmeans() {
		this->x = -1.0;
		this->y = -1.0;
	};

	PointKmeans(double x, double y);

	double getX() { return this->x; };

	void setX(double x) { this->x = x; };

	double getY() { return this->y; };

	void setY(double y) { this->y = y; };

	// Function to compare two Points
    bool equal(PointKmeans& p);


private:
	double x;
	double y;
};

// Generate random index in the 
int getRandomIndex(size_t n);

class Kmeans {

protected:
    vector<PointKmeans> points;
    size_t k;
    size_t maxIter;
    vector<PointKmeans> centroids;
    double sqDist = 0.0; // variable for multiple trials version for selecting the best trial

public:

	Kmeans(vector<PointKmeans> points, size_t k, size_t maxIter=1'000);

	virtual void initializeCentroids();

	// initialize multiple centroid as the number of trials
	vector<vector<PointKmeans>> initializeCentroidsForMultipleTrials(size_t nTrials);

	// Basic kmeans
	virtual pair<vector<PointKmeans>, vector<vector<PointKmeans>>> k_means();

	// Runs multiple trials of Basic Kmeans
    pair<vector<PointKmeans>, vector<vector<PointKmeans>>> k_meansMultipleTrials(size_t nTrials, vector<vector<PointKmeans>> initCentroids);

	// Helper function for parallel kmeans
	// Same as Basic kmeans, but returns the sqDist as well
    tuple<double, vector<PointKmeans>, vector<vector<PointKmeans>>> k_meansForParallel(vector<PointKmeans> c);

	// Runs in parallel multiple trials of Basic Kmeans
    pair<vector<PointKmeans>, vector<vector<PointKmeans>>> k_meansParallelMultipleTrials(size_t nTrials, vector<vector<PointKmeans>> initCentroids);

	vector<PointKmeans> getPoints() { return this->points; };

    vector<PointKmeans> getCentroids() { return this->centroids; };

    void setCentroids(vector<PointKmeans>& centroids) { this->centroids = centroids; };
};

class ParallelKmeans : public Kmeans{

public:

	// Parallel version also takes the centroids as argument to ensure it is computing everything the same way as single Threaded version
	ParallelKmeans(vector<PointKmeans> points, size_t k, vector<PointKmeans> centroids, size_t maxIter = 1'000);

	pair<vector<PointKmeans>, vector<vector<PointKmeans>>> k_means() override;

};

class KmeansPlusPlus : public Kmeans {

public:

	KmeansPlusPlus(vector<PointKmeans> points, size_t k, size_t maxIter = 1'000)
    : Kmeans(points, k, maxIter) {};

	// Kmeans++ centroids inicialization
	// Uses points selection that are selected randomly from a weighted probability distribution
	// Weight of each point is its squared distance to the nearest already generated centroid
    void initializeCentroids() override;

};



