#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

const size_t FILE_SIZE = 1024 * 1024;
char* board;  

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

int main(int argc, char* argv[]) {
    
    if (argc < 1) {
        std::cerr << "Insufficient arguments\n";
        system("pause");
        return 1;
    }

    std::string fileName = "Ideas.txt";
    std::vector<std::string> ideas = readIdeasFromFile(fileName);

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"IdeasSharedMemory");
    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object: " << GetLastError() << std::endl;
            std::cin.get();
        return 2;
    }

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
        CloseHandle(hMapFile);
        return 5;
    }

    std::vector<std::string> randomIdeas = selectRandomIdeas(ideas, 2);

    std::ostringstream oss;
    for (const auto& idea : randomIdeas) {
        oss << idea << "\n";
    }
    std::string ideasToWrite = oss.str();

    WaitForSingleObject(hMutex, INFINITE);

    size_t currentSize = strlen(board);

    if (currentSize + ideasToWrite.size() + 1 > FILE_SIZE) {
        std::cerr << "Not enough space to write data." << std::endl;
        ReleaseMutex(hMutex);
        CloseHandle(hMapFile);
        CloseHandle(hMutex);
        return 5;
    }

    memcpy(board + currentSize, ideasToWrite.c_str(), ideasToWrite.size() + 1);
    ReleaseMutex(hMutex);
    //TODO: voting for best ideas

    UnmapViewOfFile(board);
    CloseHandle(hMapFile);
    CloseHandle(hMutex);

    return 0;
}