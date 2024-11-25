#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>

const size_t FILE_SIZE = 1024 * 1024;
char* board;  

int main(int argc, char* argv[]) {
    
    /*if (argc < 4) {
        std::cerr << "Insufficient arguments\n";
        system("pause");
        return 1;
    }*/

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"IdeasSharedMemory");
    
    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object: " << GetLastError() << std::endl;
            std::cin.get();
        return 2;
    }

    board = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);
    
    if (board == NULL) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
            std::cin.get();

        CloseHandle(hMapFile);
        return 3;
    }

    //TODO: write ideas  to map
    
    //TODO: voting for best ideas

    UnmapViewOfFile(board);
    CloseHandle(hMapFile);
}