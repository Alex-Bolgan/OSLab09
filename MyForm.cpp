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
std::vector<int> votes;

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
  
    }

    void MyForm::CountdownTimer_Tick(Object^ sender, EventArgs^ e) {
            
        if (remainingTime > 0) {
                remainingTime--;
                timeLabel->Text = FormatTime(remainingTime);
        } else {
                countdownTimer->Stop();
                timeLabel->Text = "00:00";
                
                std::string content(board);  
                textBox1->Text = gcnew System::String(content.c_str()); 
                       
                SignalChildProcesses();
                //Sleep(10000);

                votes.clear();

                WaitForChildProcesses();

                // Retrieve and display the exit code for each process
                for (int i = 0; i < processNumber; i++) {
                    DWORD exitCode;
                    if (GetExitCodeProcess(process_handles_arr[i], &exitCode)) {
                        /*// Display the exit code in some form (e.g., MessageBox, log, etc.)
                        System::String^ message = "Child process " + i + " exited with code: " + exitCode;
                        MessageBox::Show(message);*/

                        votes.push_back(static_cast<int>(exitCode));
                    }
                    else {
                        // Handle error in retrieving exit code
                        System::String^ error = "Failed to get exit code for process " + i +
                            ". Error: " + GetLastError();
                        MessageBox::Show(error);
                    }

                    // Close process handle
                    CloseHandle(process_handles_arr[i]);
                        //UnmapViewOfFile(board);
                        //CloseHandle(hMapFile);
                }

                FindTopThreeIdeas();
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

        if (sortIdeas[0].second == 0) {
            textBox1->Text = "*Child processes have not voted on existing ideas*" + Environment::NewLine;
            WriteToFile("*Child processes have not voted on existing ideas*");
            return;
        }

        if (sortIdeas.size() < 3) {

            for (int i = (sortIdeas.size() - 1); i >= 0; --i) {
                if (sortIdeas[i].second != 0) {
                    textBox1->Text = "Top " + (i+1) + " ideas: " + Environment::NewLine;
                    textBox1->Text += "*not enough ideas and votes for top 3*" + Environment::NewLine + Environment::NewLine;
                    break;
                }
            }
        }
        else if (sortIdeas.size() >= 3) {

            if (sortIdeas[2].second == 0) {
                for (int i = (sortIdeas.size() - 1); i >= 0; --i) {
                    if (sortIdeas[i].second != 0) {
                        textBox1->Text = "Top " + (i + 1) + " ideas: " + Environment::NewLine;
                        textBox1->Text += "*not enough ideas and votes for top 3*" + Environment::NewLine + Environment::NewLine;
                        break;
                    }
                }
            }
            else {
                textBox1->Text = "Top 3 ideas: " + Environment::NewLine + Environment::NewLine;
            }
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

    int MyForm::ShowLessThanThreeIdeas(std::vector<std::pair<std::string, int>>& sortedVotes, std::ostringstream& oss, int& lastPlace, int size)
    {
        for (int i = size; i >= 0; --i) {
            if (sortedVotes[i].second != 0) {
                lastPlace = i;
                break;
            }
        }
        if (lastPlace == -1) {
            return -1;
        }
        else {
            for (int i = 0; i <= lastPlace; ++i) {
                textBox1->Text += "Rank " + (i + 1) + ": " + gcnew System::String(sortedVotes[i].first.c_str()) + Environment::NewLine;
                oss << "Rank " << i + 1 << ": " << sortedVotes[i].first << "\n";
            }
        }
        return 0;
    }

    System::Void MyForm::backgroundWorker_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
    {
        return System::Void();
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

        // Create a vector of pairs (idea, votes), initializing votes to 0
        std::vector<std::pair<std::string, int>> sortedVotes;
        for (const auto& idea : ideas) {
            sortedVotes.emplace_back(idea, 0);
        }

        // Count votes for each idea by incrementing the vote count in the vector
        for (const auto& vote : votes) {
            if (vote >= 1 && vote <= static_cast<int>(sortedVotes.size())) {
                sortedVotes[vote - 1].second++; // Increment vote count for the corresponding idea
            }
            else {
                // Handle invalid votes (optional)
                // std::cerr << "Invalid vote received: " << vote << std::endl;
            }
        }

        std::sort(sortedVotes.begin(), sortedVotes.end(), CompareByCount);

        BoardHeader(sortedVotes);

        std::ostringstream oss;
        int lastPlace = -1; 


        if (sortedVotes.size() < 3) {
            ShowLessThanThreeIdeas(sortedVotes, oss, lastPlace, sortedVotes.size() - 1);
        }
        else {
            if (sortedVotes[2].second ==0) {
                ShowLessThanThreeIdeas(sortedVotes, oss, lastPlace, 1);
            }
            else {
                for (int i = 0; i < 3; ++i) {
                    textBox1->Text += "Rank " + (i + 1) + ": " + gcnew System::String(sortedVotes[i].first.c_str()) + Environment::NewLine;
                    oss << "Rank " << i + 1 << ": " << sortedVotes[i].first << "\n";
                }
            }
        }

        WriteToFile(oss.str());
    }

    void waitForSignal() {

        HANDLE hEvent;

        while(true){

            hEvent = OpenEvent(SYNCHRONIZE, FALSE, L"VotingEvent");

            if (hEvent != NULL) {
                break;
            }

            CloseHandle(hEvent);
        }
    
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
}
