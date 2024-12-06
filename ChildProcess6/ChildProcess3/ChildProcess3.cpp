#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#define NUMBER_OF_IDEAS 1

const size_t FILE_SIZE = 1024 * 1024;
char* board;

//Returns vector of ideas, read from file. Takes file path as parameter
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

//Selects random ideas from file and stores in a seperated vector of strings 
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

//Waits for signal from parent object before continuing operating
void waitForSignal() {

    HANDLE hEvent;

    while(true){

        hEvent = OpenEvent(SYNCHRONIZE, FALSE, L"ParentToChildEvent");

        if (hEvent != NULL) {
            break;
        }

        CloseHandle(hEvent);
    }
    //Notify user about waiting signal
    std::cout << "Child process waiting for signal..." << std::endl;

    DWORD waitResult = WaitForSingleObject(hEvent, INFINITE);
    if (waitResult == WAIT_OBJECT_0) {
        std::cout << "Signal received! Processing event..." << std::endl;        
    }
    else {
        std::cerr << "Wait failed: " << GetLastError() << std::endl;
    }
    CloseHandle(hEvent);
}
//Output all chosen ideas to a console (should be used after signal from parent object)
void outputIdeasWithNumbers(const std::string& ideas) {
    std::istringstream stream(ideas);
    std::string idea;
    int lineNumber = 1;

    while (std::getline(stream, idea)) {
        if (!idea.empty()) { // Skip empty lines
            std::cout << lineNumber++ << ". " << idea << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
  
    if (argc < 1) {
        std::cerr << "Insufficient arguments\n";
        system("pause");
        //return error code
        return -1;

    }
    //Read ideas
    std::string fileName = "Ideas.txt";
    std::vector<std::string> ideas = readIdeasFromFile(fileName);

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"IdeasSharedMemory");

    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object: " << GetLastError() << std::endl;
        std::cin.get();
        //return error code
        return -2;
    }
    //Synchoronization
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"IdeasMutex");
    if (hMutex == NULL) {
        std::cerr << "Could not open mutex object: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        std::cin.get();
        return 3;
    }

    board = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);

    if (board == NULL) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        std::cin.get();
    }

    //Select ideas
    std::vector<std::string> randomIdeas = selectRandomIdeas(ideas, NUMBER_OF_IDEAS);

    std::ostringstream oss;
    DWORD processID = GetCurrentProcessId();
    oss << "(" << processID << ")";
    for (const auto& idea : randomIdeas) {
        oss << idea << "\n";
    }
    std::string ideasToWrite = oss.str();

    WaitForSingleObject(hMutex, INFINITE);

    size_t currentSize = strlen(board);
    //Write idea to shared memory
    if (currentSize + ideasToWrite.size() + 1 > FILE_SIZE) {
        std::cerr << "Not enough space to write data." << std::endl;
        ReleaseMutex(hMutex);
        CloseHandle(hMapFile);
        //return error code
        return -3;
    }

    memcpy(board + currentSize, ideasToWrite.c_str(), ideasToWrite.size() + 1);
    ReleaseMutex(hMutex);

    std::cout << ideasToWrite;

    CloseHandle(hMapFile);
    //wait for signal from parent object
    waitForSignal();

    std::string content(board);

    outputIdeasWithNumbers(content);

    UnmapViewOfFile(board);

    //Let user choose best idea
    int vote;
    std::cin>>vote;
    return vote;
}