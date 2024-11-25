#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;


std::array<HANDLE, 8> process_handles_arr;
HANDLE hMapFile;

int main(array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    OSLab09::MyForm form;
    Application::Run(% form);

    return 0;
}