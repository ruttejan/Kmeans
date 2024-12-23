#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "kmeans.hpp"
#include "dataGenerator.hpp"
#include "tests.hpp"
#include <chrono>

using namespace std;

void printHelp(){
    cout << "Usage: ./kmeans [OPTIONS]" << endl;
    cout << "Options:" << endl;
    cout << "\t\t--help\t\t\tShow this help message" << endl;
    cout << "\tInput options (you can use only one of these arguments at a time):" << endl;
    cout << "\t\t--file\t\t\tDefault input option. Read points from a file" << endl;
    cout << "\t\t--random\t\tGenerate random points" << endl;
    cout << "\t\t--allFiles\t\tRun the algorithm for all test files" << endl;
    cout << "\tOutput options:" << endl;
    cout << "\t\t--plot\t\t\tSave plot of the output" << endl;
    cout << "\t\t--save\t\t\tSave the input points to a file" << endl;
    cout << "\tAlgorithm options:" << endl;
    cout << "\t\t--singleThread\t\tDefault option. Run the normal version of the algorithm" << endl;
    cout << "\t\t        \t\tMust be explicitly set if you want to run the singleThread version and parallel at the same time" << endl;
    cout << "\t\t--parallel\t\tRun the parallel version of the algorithm" << endl;
    cout << "\t\t--basic\t\tDefault option. Run the basic version of the algorithm" << endl;
    cout << "\t\t        \t\tMust be explicitly set if you want to run the basic version and other versions at the same time" << endl;
    cout << "\t\t--plusplus\t\tRun the kmeans++ version of the algorithm" << endl;
    cout << "\t\t--multiTrials\t\tRun the multiple trials version of the algorithm" << endl;
    cout << "\t\t--allVersions\t\tRun all versions of the algorithm (basic, ++, MT)" << endl;

}

enum class ARGUMENTS {
    RANDOM,
    FILE,
    ALLFILES,
    PLOT,
    SAVE,
    PARALLEL,
    SINGLETHREAD,
    BASIC,
    PLUSPLUS,
    MULTITRIALS,
    ALLVERSIONS,
    INVALID
};

ARGUMENTS getArgument(const string& arg){
    if(arg == "--random") return ARGUMENTS::RANDOM;
    if(arg == "--file") return ARGUMENTS::FILE;
    if(arg == "--allFiles") return ARGUMENTS::ALLFILES;
    if(arg == "--plot") return ARGUMENTS::PLOT;
    if(arg == "--save") return ARGUMENTS::SAVE;
    if(arg == "--singleThread") return ARGUMENTS::SINGLETHREAD;
    if(arg == "--parallel") return ARGUMENTS::PARALLEL;
    if(arg == "--basic") return ARGUMENTS::BASIC;
    if(arg == "--plusplus") return ARGUMENTS::PLUSPLUS;
    if(arg == "--multiTrials") return ARGUMENTS::MULTITRIALS;
    if(arg == "--allVersions") return ARGUMENTS::ALLVERSIONS;
    return ARGUMENTS::INVALID;
    
}


int main(int argc, char* argv[]) {

    if (argc == 2 && string(argv[1]) == "--help") {
        printHelp();
        return 0;
    }

    bool random = false;
    bool file = false;
    bool plot = false;
    bool save = false;
    bool singleThread = false;
    bool parallel = false;
    bool basic = false;
    bool plusplus = false;
    bool multi_trials = false;
    bool allFiles = false;

    string filename = "";
    string savefile = "";
    string plotfile = "";

    // load arguments
    for (int i = 1; i < argc; i++) {
        ARGUMENTS arg = getArgument(argv[i]);
        switch(arg){
            case ARGUMENTS::RANDOM:
                if(file || allFiles){
                    cout << "You can't use --random and --file or --allFiles at the same time" << endl;
                    return 1;
                }
                random = true;
                break;
            case ARGUMENTS::FILE:
                if(random || allFiles){
                    cout << "You can't use --file and --random or --allFiles at the same time" << endl;
                    return 1;
                }
                if(save){
                    cout << "You can't use --file and --save at the same time" << endl;
                    return 1;
                }
                file = true;
                break;
            case ARGUMENTS::ALLFILES:
                if(random || file){
                    cout << "You can't use --allFiles and --random or --file at the same time" << endl;
                    return 1;
                }
                allFiles = true;
                break;
            case ARGUMENTS::PLOT:
                plot = true;
                break;
            case ARGUMENTS::SAVE:
                if(file){
                    cout << "You can't use --save and --file at the same time" << endl;
                    return 1;
                }
                save = true;
                break;
            case ARGUMENTS::SINGLETHREAD:
                singleThread = true;
                break;
            case ARGUMENTS::PARALLEL:
                parallel = true;
                break;
            case ARGUMENTS::BASIC:
                basic = true;
                break;
            case ARGUMENTS::PLUSPLUS:
                plusplus = true;
                break;
            case ARGUMENTS::MULTITRIALS:
                multi_trials = true;
                break;
            case ARGUMENTS::ALLVERSIONS:
                basic = true;
                plusplus = true;
                multi_trials = true;
                break;
            default:
                cout << "Invalid option: " << argv[i] << endl;
                return 1;
        }

    }

    // set default version if not selected parallel or singleThread explicitly
    if(!parallel && !singleThread){
        singleThread = true;
    }

    // set default version if not selected basic, plusplus or multi_trials explicitly
    if(!basic && !plusplus && !multi_trials){
        basic = true;
    }

    // set default option for input
    if(!file && !random && !allFiles){
        file = true;
    }

    // load data
    int numberOfPoints = 0, numberOfClusters = 0;
    if (random){
        cout << "Enter the number of points: " << endl;
        cin >> numberOfPoints;
        if(numberOfPoints <= 0){
            cout << "Invalid number of points. Number of points must be greater than 0" << endl;
            return 1;
        }
        cout << "Enter the number of clusters: (max 10) ";
        cin >> numberOfClusters;
        if(numberOfClusters <= 0 || numberOfClusters > 10){
            cout << "Invalid number of clusters. Number of clusters must be greater than 0 and smaller or equal to 10" << endl;
            return 1;
        }
    } else if (file){
        cout << "Enter the file Id to read the points: " << endl;
        cout << "1 : input1-10c.txt" << endl;
        cout << "2 : input2-3c.txt" << endl;
        cout << "3 : input3-5c.txt" << endl;
        cout << "4 : input4-8c.txt" << endl;
        cout << "5 : input5-8c.txt" << endl;
        cout << "6 : input6-10c.txt" << endl;
        cout << "7 : input7-10c.txt" << endl;
        cout << "8 : letters_data-10c.txt" << endl;
        cout << "9 : numbers_data-10c.txt" << endl;
        cout << "10 : Other file" << endl;
        cout << "Enter the file Id: " << endl;

        size_t fileId;
        cin >> fileId;

        if (fileId < 1 || fileId > 10){
            cout << "Invalid file Id. File Id must be a number between 1 and 10" << endl;
            return 1;
        }

        if (fileId == 10){
            cout << "Enter the filename to read the points: " << endl;
            cin >> filename;
        } else {
            filename = getFilename(static_cast<FILES>(fileId));
        }
    }

    bool correct = false;
    if (save && !allFiles && !file){
        while(!correct){
            cout << "Enter the filename to save the points (without .txt): ";
            cin >> savefile;
            if(savefile == ""){
                cout << "Invalid filename. Filename must be non-empty" << endl;
            } else {
                correct = true;
            }
        }
        savefile = savefile + "-" + to_string(numberOfClusters) + "c.txt";
    }
    
    correct = false;
    if (plot && !allFiles && !file){
        while(!correct){
            cout << "Enter the filename to plot the points (without .svg): ";
            cout << "The plot will be saved in the images folder" << endl;
            cin >> plotfile;
            if(plotfile == ""){
                cout << "Invalid filename. Filename must be non-empty" << endl;
            } else {
                correct = true;
            }
        }
        plotfile = plotfile + "-" + to_string(numberOfClusters) + "c.svg";
    } else if(plot && allFiles){
        cout << "Plots will be saved in the images folder" << endl;
    }


    // Run the selected input
    if (file){
        run_test_file(filename, singleThread, parallel, basic, plusplus, multi_trials, plot);
    }
    if (random){
        run_test_random(numberOfPoints, numberOfClusters, singleThread, parallel, basic, plusplus, multi_trials, plot, save, savefile, plotfile);
    } 
    if (allFiles){
        run_tests_for_all_files(singleThread, parallel, basic, plusplus, multi_trials, plot);
    }

    return 0;
}
