#include "tests.hpp"

// Colors used for the terminal output
const string yellow = "\033[1;93m";
const string green = "\033[1;92m";
const string red = "\033[1;91m";
const string magenta = "\033[1;95m";
const string reset = "\033[0m";

string getFilename(FILES file){
    switch (file)
    {
    case FILES::IN1:
        return "input/input1-10c.txt";
    case FILES::IN2:
        return "input/input2-3c.txt";
    case FILES::IN3:
        return "input/input3-5c.txt";
    case FILES::IN4:
        return "input/input4-8c.txt";
    case FILES::IN5:
        return "input/input5-8c.txt";
    case FILES::IN6:
        return "input/input6-10c.txt";
    case FILES::IN7:
        return "input/input7-10c.txt";
    case FILES::LETTERS:
        return "input/letters_data-10c.txt";
    case FILES::NUMBERS:
        return "input/numbers_data-10c.txt";
    default:
        return "";
    }
}

int getNumberOfClusters(const string& filename){
    string num = "";
    int numberOfClusters = 0;

    // parse the filename to get the number of clusters
    // the filename has to be filename-<number>c.txt
    bool foundDash = false;
    for (int i = 0; i < filename.size(); i++){
        if (filename[i] == '-'){
            foundDash = true;
        } else if (filename[i] == 'c' && foundDash){
            break;
        } else if (foundDash){
            num += filename[i];
        }
    }
    numberOfClusters = stoi(num);
    return numberOfClusters;
}

vector<PointKmeans> readPointsFromFile(const string& filename) {
    vector<PointKmeans> points;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        double x, y;
        if (ss >> x >> y) {
            PointKmeans point = PointKmeans(x, y);
            points.push_back(point);
        }
    }

    return points;
}



void writeSVGFile(vector<vector<PointKmeans>>& clusters, const string& filename, vector<PointKmeans>& centroids ,const string& info = "") {
    
    vector<string> colors = { "blue", "red", "green", "purple", "brown", "orange", "yellow", "gray" , "pink", "aqua"};

    cout << "Saving plot: " << endl;

    int width = 140;
    int height = 140;
    int startx = 0;
    int starty = 0;
    int offsetx = 20;
    int offsety = 20;
    double pointSize = 0.5;
    double fraction = 1.0;

    if (filename.substr(0, 7) == "letters"){
        width = 1000;
        height = 1000;
        startx = 0;
        starty = 0;
        offsetx = 600;
        offsety = 400;
        pointSize = 5;
        fraction = 10.0;
    } else if (filename.substr(0, 7) == "numbers"){
        width = 50;
        height = 50;
        offsetx = 25;
        offsety = 35;
    }
    string file = "images/" + filename + info + ".svg";
    ofstream svgFile(file);
    svgFile << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"" << startx << " " << starty << " " << width << " " << height << "\">\n";
    // add white background with black border
    svgFile << "<rect width=\"" << width << "\" height=\"" << height << "\" fill=\"white\" stroke=\"black\" stroke-width=\"" << pointSize << "\" />\n";

    // Draw all points and centroids as clusters
    for (int i = 0; i < centroids.size(); i++) {

        for (auto& point : clusters[i]) {
            svgFile << "<circle cx=\"" << point.getX() / fraction + offsetx << "\" cy=\"" << height - ( point.getY() / fraction + offsety ) << "\" r=\"" << pointSize << "\" fill=\"" << colors[i] << "\" />\n";
        }
        svgFile << "<circle cx=\"" << centroids[i].getX() / fraction + offsetx << "\" cy=\"" << height - ( centroids[i].getY() / fraction + offsety ) << "\" r=\"" << pointSize*2 << "\" fill=\"black\" />\n";
        
    }

    svgFile << "</svg>\n";
    svgFile.close();
    cout << "Plot saved to: " << file << endl;
}

void run_test(int numberOfClusters, 
                vector<PointKmeans> points,
                bool singleThread, 
                bool parallel,
                bool basic,
                bool plusplus,
                bool multiTrials,
                bool plot,
                string plotfile
){


    cout << "\tNumber of points: " << points.size() << endl;
    cout << "\tNumber of clusters: " << numberOfClusters << endl;
    cout << "-----------------------------------" << endl;


    Kmeans kmeans = Kmeans(points, numberOfClusters, 10000);
    // Initialize centroids for basic kmeans
    kmeans.initializeCentroids();
    auto initCentroids = kmeans.getCentroids();
    // Initialize output variables od basic kmeans
    vector<PointKmeans> normalCentroids;
    vector<PointKmeans> parallelCentroids;
    vector<vector<PointKmeans>> normalClusters;
    vector<vector<PointKmeans>> parallelClusters;

    if(basic) cout << "Kmeans random initialization:" << endl;

    // Single thread basic kmeans
    if(basic && singleThread){
        auto start = chrono::high_resolution_clock::now();

        pair<vector<PointKmeans>, vector<vector<PointKmeans>>> res = kmeans.k_means();
        normalCentroids = res.first;
        normalClusters = res.second;

        auto end = chrono::high_resolution_clock::now();
        cout << "\tKmeans time: " << yellow << chrono::duration<double>(end - start).count() << reset << endl; 
    }

    // Parallel basic kmeans
    if (basic && parallel){
        ParallelKmeans parallelkmeans = ParallelKmeans(points, numberOfClusters, initCentroids, 10000);
        auto start = chrono::high_resolution_clock::now();
        pair<vector<PointKmeans>, vector<vector<PointKmeans>>> res = parallelkmeans.k_means();
        parallelCentroids = res.first;
        parallelClusters = res.second;
        auto end = chrono::high_resolution_clock::now();
        cout << "\tKmeans parallel time: " << yellow << chrono::duration<double>(end - start).count() << reset << endl;
    }

    
    // check if the centroids of singleThread and parallel are equal
    if (basic && singleThread && parallel){
        bool equal = true;
        for (int i = 0; i < normalCentroids.size(); i++) {
            if (!normalCentroids[i].equal(parallelCentroids[i])) {
                equal = false;
                break;
            }
        }
        if (equal) cout << "\tCentroids are " << green << "equal" << reset << endl;
        else cout << "\tCentroids are " << red << "not equal" << reset << endl;
    }

    // Plot output of basic Kmeans
    if(basic && singleThread && plot){
        writeSVGFile(normalClusters, plotfile, normalCentroids, "Kmeans");
        
    }
    if (basic && parallel && plot){
        writeSVGFile(parallelClusters, plotfile, parallelCentroids, "ParallelKmeans");
    }

    if (basic) cout << "-----------------------------------" << endl;

    KmeansPlusPlus kmeansplusplus = KmeansPlusPlus(points, numberOfClusters);
    // Initialize centroids for Kmeans++
    kmeansplusplus.initializeCentroids();
    auto initCentroidsPlusPlus = kmeansplusplus.getCentroids();
    // Initialize output variables of Kmeans++
    vector<PointKmeans> normalCentroidsPlusPLus;
    vector<PointKmeans> parallelCentroidsPlusPLus;
    vector<vector<PointKmeans>> normalClustersPlusPLus;
    vector<vector<PointKmeans>> parallelClustersPlusPLus;
    
    if(plusplus) cout << "Kmeans++ initialization:" << endl;

    // Single thread Kmeans++
    if (plusplus && singleThread){

        auto start = chrono::high_resolution_clock::now();
        pair<vector<PointKmeans>, vector<vector<PointKmeans>>> res = kmeansplusplus.k_means();
        normalCentroidsPlusPLus = res.first;
        normalClustersPlusPLus = res.second;
        auto end = chrono::high_resolution_clock::now();
        cout << "\tKmeans++ time: " << yellow << chrono::duration<double>(end - start).count() << reset << endl;
    }

    // Parallel Kmeans++
    if (plusplus && parallel){
        ParallelKmeans parallelkmeansplusplus = ParallelKmeans(points, numberOfClusters, initCentroidsPlusPlus, 10000);
        auto start = chrono::high_resolution_clock::now();
        pair<vector<PointKmeans>, vector<vector<PointKmeans>>> res = parallelkmeansplusplus.k_means();
        parallelCentroidsPlusPLus = res.first;
        parallelClustersPlusPLus = res.second;
        auto end = chrono::high_resolution_clock::now();
        cout << "\tKmeans++ parallel time: " << yellow << chrono::duration<double>(end - start).count() << reset << endl;
    }
    

    // check if the centroids of singleThread and parallel are equal
    if (plusplus && singleThread && parallel){
        bool equal = true;
        for (int i = 0; i < normalCentroidsPlusPLus.size(); i++) {
            if (!normalCentroidsPlusPLus[i].equal(parallelCentroidsPlusPLus[i])) {
                equal = false;
                break;
            }
        }
        if (equal) cout << "\tCentroids are " << green << "equal" << reset << endl;
        else cout << "\tCentroids are " << red << "not equal" << reset << endl;
    }

    // Plot output of Kmeans++
    if(plusplus && singleThread && plot){
        writeSVGFile(normalClustersPlusPLus, plotfile, normalCentroidsPlusPLus, "Kmeans++");
    }
    if (plusplus && parallel && plot){
        writeSVGFile(parallelClustersPlusPLus, plotfile, parallelCentroidsPlusPLus, "ParallelKmeans++");
    }
    

    if(plusplus) cout << "-----------------------------------" << endl;

    if(multiTrials) cout << "Multiple trials: " << endl;

    size_t numTrials = 20;
    Kmeans kmeansMT = Kmeans(points, numberOfClusters, 10000);
    // Initialize centroids for multiple trials
    vector<vector<PointKmeans>> initCentroidsMT = kmeans.initializeCentroidsForMultipleTrials(numTrials);
    
    // Initialize output variables of multiple trials
    vector<PointKmeans> normalCentroidsMT;
    vector<PointKmeans> parallelCentroidsMT;
    vector<vector<PointKmeans>> normalClustersMT;
    vector<vector<PointKmeans>> parallelClustersMT;

    // Single thread multiple trials
    if (multiTrials && singleThread){
        auto start = chrono::high_resolution_clock::now();
        pair<vector<PointKmeans>, vector<vector<PointKmeans>>> res = kmeansMT.k_meansMultipleTrials(numTrials, initCentroidsMT);
        normalCentroidsMT = res.first;
        normalClustersMT = res.second;
        auto end = chrono::high_resolution_clock::now();
        cout << "\tKmeans multiple trials time: " << yellow << chrono::duration<double>(end - start).count() << reset << endl;
    }

    // Parallel multiple trials
    if (multiTrials && parallel){
        Kmeans parallelkmeansMT = Kmeans(points, numberOfClusters, 10000);
        auto start = chrono::high_resolution_clock::now();
        pair<vector<PointKmeans>, vector<vector<PointKmeans>>> res = parallelkmeansMT.k_meansParallelMultipleTrials(numTrials, initCentroidsMT);
        parallelCentroidsMT = res.first;
        parallelClustersMT = res.second;
        auto end = chrono::high_resolution_clock::now();
        cout << "\tKmeans parallel multiple trials time: " << yellow << chrono::duration<double>(end - start).count() << reset << endl;
    }


    // check if the centroids of singleThread and parallel are equal
    if (multiTrials && singleThread && parallel){
        bool equal = true;
        for (int i = 0; i < normalCentroidsMT.size(); i++) {
            if (!normalCentroidsMT[i].equal(parallelCentroidsMT[i])) {
                equal = false;
                break;
            }
        }
        if (equal) cout << "\tCentroids are " << green << "equal" << reset << endl;
        else cout << "\tCentroids are " << red << "not equal" << reset << endl;
    }

    // Plot output of multiple trials
    if(multiTrials && singleThread && plot){
        writeSVGFile(normalClustersMT, plotfile, normalCentroidsMT, "KmeansMT");
    }
    if (multiTrials && parallel && plot){
        writeSVGFile(parallelClustersMT, plotfile, parallelCentroidsMT, "KmeansParallelMT");
    }
    
    cout << magenta << "-----------------------------------" << reset << endl;

}

void savePointsToFile(const string& filename, vector<PointKmeans>& points, int numberOfClusters) {

    // Change the filename so it has correct format
    string output = "input/" + filename + "-" + to_string(numberOfClusters) + "c.txt";
    cout << "Saving points to: " << filename << endl;
    ofstream file;
    file.open(filename);
    
    for (PointKmeans& point : points) {
        double x = point.getX();
        double y = point.getY();

        file << x << " " << y << endl;
    }

    file.close();
    cout << "Points saved to: " << output << endl;
}


void run_test_random(int numberOfPoints,
                    int numberOfClusters,
                    bool singleThread,
                    bool parallel,
                    bool basic,
                    bool plusplus,
                    bool multiTrials,
                    bool plot,
                    bool save,
                    string savefile,
                    string plotfile
                    ){
    
    // Generate random points
    ClusterGenerator gen1 = ClusterGenerator(numberOfPoints, numberOfClusters);
    gen1.generateClusters();
    vector<PointKmeans> points = gen1.getPoints();

    // Save points to file
    if (save)
        savePointsToFile(savefile, points, numberOfClusters);

    cout << magenta << "-----------------------------------" << reset << endl;
    cout << "Running test for random data" << endl;
    // Run the test
    run_test(numberOfClusters,
            points,
            singleThread,
            parallel,
            basic,
            plusplus,
            multiTrials,
            plot,
            plotfile);
}

void run_test_file(const string& filename,
                    bool singleThread,
                    bool parallel,
                    bool basic,
                    bool plusplus,
                    bool multiTrials,
                    bool plot
                    ){
    
    // Read the info and points from the file
    int numberOfClusters = getNumberOfClusters(filename);
    vector<PointKmeans> points = readPointsFromFile(filename);

    // Extract the filename for the output
    string fileInfo = filename.substr(6); // remove "input/"
    fileInfo = fileInfo.substr(0, fileInfo.size() - 4); // remove ".txt"

    cout << magenta << "-----------------------------------" << reset << endl;
    cout << "Running test for file: " << fileInfo << endl;

    // Run the test
    run_test(numberOfClusters,
            points,
            singleThread,
            parallel,
            basic,
            plusplus,
            multiTrials,
            plot,
            fileInfo);
}

void run_tests_for_all_files(
                    bool singleThread,
                    bool parallel,
                    bool basic,
                    bool plusplus,
                    bool multiTrials,
                    bool plot
){

    // Run tests for all files
    for (int i = 1; i <= 9; i++){
        FILES file = static_cast<FILES>(i);
        string filename = getFilename(file);
        cout << i << " ";
        run_test_file(filename,
                    singleThread,
                    parallel,
                    basic,
                    plusplus,
                    multiTrials,
                    plot);
        cout << endl;
    }

}
