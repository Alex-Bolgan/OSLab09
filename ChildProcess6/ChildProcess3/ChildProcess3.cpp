#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#define NUMBER_OF_IDEAS 2

const size_t FILE_SIZE = 1024 * 1024;
char* board;

//returns vector of ideas, read from file. Takes file path as parameter
std::vector<std::string> readIdeasFromFile(const std::string& fileName) {
    std::vector<std::string> ideas;
    std::ifstream inFile(fileName);
    if (!inFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return ideas;
    }
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            ideas.push_back(line);
        }
    }
    inFile.close();
    return ideas;
}

//returns vector of ideas, chosen from all read ideas. Takes refernce to vector with ideas, and number of ideas to return as parameters
std::vector<std::string> selectRandomIdeas(const std::vector<std::string>& ideas, size_t count) {
    std::vector<std::string> selectedIdeas;
    if (ideas.size() < count) {
        std::cerr << "Not enough ideas to select " << count << " items." << std::endl;
        return selectedIdeas;
    }

    std::vector<int> indices;
    for (int i = 0; i < static_cast<int>(ideas.size()); ++i) {
        indices.push_back(i);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    for (size_t i = 0; i < count; ++i) {
        selectedIdeas.push_back(ideas[indices[i]]);
    }

    return selectedIdeas;
}

//Simulates choosing the best idea randomly from a set of ideas.
int bestChosenIdea(int numberOfProcesses) {
    // Create a random device
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, numberOfProcesses * NUMBER_OF_IDEAS); // Random integers between 1 and all ideas

    // Generate and print a random integer
    int random_number = dist(gen);
    std::cout << "Random Number: " << random_number << std::endl;

    return random_number;
}

int main(int argc, char* argv[]) {

    if (argc < 1) {
        std::cerr << "Insufficient arguments\n";
        system("pause");
        //return error code
        return -1;
    }

    std::string fileName = "Ideas.txt";
    std::vector<std::string> ideas = readIdeasFromFile(fileName);

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"IdeasSharedMemory");

    std::vector<std::string> randomIdeas = selectRandomIdeas(ideas, NUMBER_OF_IDEAS);

    for (const auto& idea : randomIdeas) {
        std::cout << idea << std::endl;
    }


    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object: " << GetLastError() << std::endl;
        std::cin.get();
        //return error code
        return -2;
    }

    board = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);

    if (board == NULL) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        std::cin.get();

        CloseHandle(hMapFile);
        //return error code
        return -3;
    }

    //TODO: write ideas  to map



    std::cin.get();
    UnmapViewOfFile(board);
    CloseHandle(hMapFile);
    //TODO: voting for best ideas
    int numberOfProcesses = std::stoi(argv[1]);
    return bestChosenIdea(numberOfProcesses);
}