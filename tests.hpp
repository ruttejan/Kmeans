#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "kmeans.hpp"
#include "dataGenerator.hpp"
#include <chrono>

// Enum class for the test files
enum class FILES
{
    IN1 = 1,
    IN2 = 2,
    IN3 = 3,
    IN4 = 4,
    IN5 = 5,
    IN6 = 6,
    IN7 = 7,
    LETTERS = 8,
    NUMBERS = 9
};

// Function to get the filename for the test files
string getFilename(FILES file);

// Function for extracting the number of clusters from the filename
int getNumberOfClusters(const string& filename);

// Function to save the points to a file
void savePointsToFile(const string& filename, vector<PointKmeans>& points, int numberOfClusters);

// Function to read the points from a file
vector<PointKmeans> readPointsFromFile(const string& filename);

// Function to run an arbitrary test
void run_test(int numberOfClusters, 
                vector<PointKmeans> points,
                bool singleThread, 
                bool parallel,
                bool basic,
                bool plusplus,
                bool multi_trials,
                bool plot,
                string plotfile);

// Function to run a test with random points 
//  - generates points and runs the test
void run_test_random(int numberOfPoints,
                    int numberOfClusters,
                    bool singleThread,
                    bool parallel,
                    bool basic,
                    bool plusplus,
                    bool multi_trials,
                    bool plot,
                    bool save,
                    string savefile,
                    string plotfile);

// Function to run a test with points from a file 
//  - reads points from a file and runs the test
void run_test_file(const string& filename,
                    bool singleThread,
                    bool parallel,
                    bool basic,
                    bool plusplus,
                    bool multi_trials,
                    bool plot);

// Function to run tests for all test files defined in FILES enum
void run_tests_for_all_files(
                    bool singleThread,
                    bool parallel,
                    bool basic,
                    bool plusplus,
                    bool multi_trials,
                    bool plot);

