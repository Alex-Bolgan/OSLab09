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
#include <map>
#include <algorithm>


const size_t FILE_SIZE = 1024 * 1024;

extern std::array<HANDLE, 8> process_handles_arr;
extern HANDLE hMapFile;
char* board;
HANDLE hResultFile;

namespace OSLab09 {

    void MyForm::MapLinesToSharedMemory() {

        hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, FILE_SIZE, L"IdeasSharedMemory");

        if (hMapFile == NULL) {
            MessageBox::Show("Could not create file mapping object: " + GetLastError());
            return;
        }

        board = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);

        if (board == NULL) {
            MessageBox::Show("Could not map view of file: " + GetLastError());
        }

        HANDLE hMutex = CreateMutex(NULL, FALSE, L"IdeasMutex");
        if (hMutex == NULL) {
            MessageBox::Show("Could not create mutex object: " + GetLastError());
            UnmapViewOfFile(board);
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

    void MyForm::SignalChildProcesses() {
       
        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, L"ParentToChildEvent");

        if (hEvent == NULL) {
            MessageBox::Show("Failed to create event: " + GetLastError());
            return;
        }

        Sleep(5000); 
        if (!SetEvent(hEvent)) {
            MessageBox::Show("Failed to signal event: " + GetLastError());
        }
        else {
            MessageBox::Show("Signal sent to child processes!");
        }

        CloseHandle(hEvent); 
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

        remainingTime = 30;
        timeLabel->Text = FormatTime(remainingTime);
        countdownTimer->Start();

        SignalChildProcesses();

        WaitForChildProcesses();
        
        //Для перевірки пам'яті (можна викинути)
        std::string content(board);  
        MessageBox::Show(gcnew System::String(content.c_str())); 

        WaitForMultipleObjects(processNumber, process_handles_arr.data(), TRUE, INFINITE);

        // Retrieve and display the exit code for each process
        for (int i = 0; i < processNumber; i++) {
            DWORD exitCode;
            if (GetExitCodeProcess(process_handles_arr[i], &exitCode)) {
                /*// Display the exit code in some form (e.g., MessageBox, log, etc.)
                System::String^ message = "Child process " + i + " exited with code: " + exitCode;
                MessageBox::Show(message);*/
            }
            else {
                // Handle error in retrieving exit code
                System::String^ error = "Failed to get exit code for process " + i +
                    ". Error: " + GetLastError();
                MessageBox::Show(error);
            }

            // Close process handle
            CloseHandle(process_handles_arr[i]);
        }

    }

    void MyForm::CountdownTimer_Tick(Object^ sender, EventArgs^ e) {
            
        if (remainingTime > 0) {
                remainingTime--;
                timeLabel->Text = FormatTime(remainingTime);
        } else {
                countdownTimer->Stop();
                timeLabel->Text = "00:00";
                FindTopThreeIdeas();
                MessageBox::Show("Time out!", "Timer");
                        
                UnmapViewOfFile(board);
                CloseHandle(hMapFile);
        }
    }
      
    String^ MyForm::FormatTime(int seconds) {

            int minutes = seconds / 60;
            int secs = seconds % 60;

            return String::Format("{0:D2}:{1:D2}", minutes, secs);
    }


    inline System::Void MyForm::WaitForChildProcesses() {
        WaitForMultipleObjects(processNumber, process_handles_arr.data(), TRUE, INFINITE);
    }

    // Compare function
    bool CompareByCount(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second > b.second;  // sort in descending order
    }

    void MyForm::BoardHeader(std::vector<std::pair<std::string, int>> sortIdeas)
    {
        if (sortIdeas.size() < 3) {
            textBox1->Text = "Top " + sortIdeas.size() + " ideas: " + Environment::NewLine;
            textBox1->Text += "*not enough ideas for top 3*" + Environment::NewLine + Environment::NewLine;
        }
        else {
            textBox1->Text = "Top 3 ideas: " + Environment::NewLine + Environment::NewLine;
        }
    }

    void MyForm::WriteToFile(const std::string& data)
    {
        const char* filePath = "output.txt";

        HANDLE hFile = CreateFileA(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create or open file. Error: " << GetLastError() << std::endl;
            return;
        }

        DWORD written;
        BOOL writeResult = WriteFile(hFile, data.c_str(), data.size(), &written, NULL);

        if (!writeResult) {
            std::cerr << "Failed to write to file. Error: " << GetLastError() << std::endl;
        }

        CloseHandle(hFile);
    }

    void MyForm::FindTopThreeIdeas()
    {
        //Search and show top 3 most popular ideas 

        // Reading all board ideas
        std::vector<std::string> ideas;
        std::string content(board);
        std::istringstream iss(content);
        std::string idea;

        while (std::getline(iss, idea)) {
            ideas.push_back(idea);
        }

        // Counting repetitions of ideas 
        std::map<std::string, int> ideaCount;
        for (const auto& i : ideas) {
            ideaCount[i]++;
        }

        // Convert map into a vector of pairs for sorting
        std::vector<std::pair<std::string, int>> sortedIdeas(ideaCount.begin(), ideaCount.end());

        std::sort(sortedIdeas.begin(), sortedIdeas.end(), CompareByCount);

        BoardHeader(sortedIdeas);

        std::ostringstream oss;
        for (int i = 0; i < 3 && i < sortedIdeas.size(); ++i) {
            textBox1->Text += "Rank " + (i + 1) + ": " + gcnew System::String(sortedIdeas[i].first.c_str()) + Environment::NewLine;
            oss << "Rank " << i + 1 << ": " << sortedIdeas[i].first << "\n";
        }

        WriteToFile(oss.str());
    }

}
