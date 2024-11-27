#include "MyForm.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <msclr/marshal_cppstd.h>


const size_t FILE_SIZE = 1024 * 1024;

extern std::array<HANDLE, 8> process_handles_arr;
extern HANDLE hMapFile;
char* board;
HANDLE hResultFile;

namespace OSLab09{

void MyForm::MapLinesToSharedMemory() {

    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, FILE_SIZE, L"IdeasSharedMemory");

    if (hMapFile == NULL) {
        MessageBox::Show("Could not create file mapping object: " + GetLastError());
        return;
    }

    board = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);
    
    if (board == NULL) {
        MessageBox::Show("Could not map view of file: "+ GetLastError());
        CloseHandle(hMapFile);
        return;
    }
}

HANDLE MyForm::launchChildProcess() {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::ostringstream oss;

    oss << processNumber;

    std::string filePath = "x64\\Debug\\ChildProcess3.exe";

    std::string command = filePath + " " + oss.str();  
    LPSTR cmdLine = const_cast<char*>(command.c_str());

    if (!CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        MessageBox::Show("CreateProcess failed: " + GetLastError());
        return NULL;
    }

    CloseHandle(pi.hThread);
    return pi.hProcess;
}

    System::Void MyForm::LaunchProcessesButton_Click(System::Object^ sender, System::EventArgs^ e) {

        processNumber = Convert::ToInt32(processNumberTextBox->Text);

        if (processNumber < 1 || processNumber > 8) {
           MessageBox::Show("Process number must be between 1 and 8");
            return;
        }

        MapLinesToSharedMemory();

        for (int i = 0; i < processNumber; i++)
        {
             process_handles_arr[i] = launchChildProcess();
        }


        WaitForChildProcesses();
        UnmapViewOfFile(board);
        CloseHandle(hMapFile);
    } 

    inline System::Void MyForm::WaitForChildProcesses() {

        WaitForMultipleObjects(processNumber, process_handles_arr.data(), TRUE, INFINITE);
    }

}
