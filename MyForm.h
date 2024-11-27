#pragma once
#include <Windows.h>
#include <stdio.h>
#include <chrono>
#include <cstdio>  
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <TlHelp32.h> 
#include <thread> 

namespace OSLab09 {


#pragma region hide

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ LaunchProcessesButton;
	protected:












	private: System::Windows::Forms::ToolStripMenuItem^ setAffinityToolStripMenuItem;
	private: System::Windows::Forms::ToolStripTextBox^ toolStripTextBox1;




	private: System::Windows::Forms::Label^ label1;








	private: System::Windows::Forms::TextBox^ textBox1;
	private: System::Windows::Forms::TextBox^ processNumberTextBox;



	private: System::Windows::Forms::Label^ label3;




	private: System::ComponentModel::IContainer^ components;



	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

#pragma endregion
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->LaunchProcessesButton = (gcnew System::Windows::Forms::Button());
			this->setAffinityToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox1 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->processNumberTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// LaunchProcessesButton
			// 
			this->LaunchProcessesButton->Location = System::Drawing::Point(86, 243);
			this->LaunchProcessesButton->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->LaunchProcessesButton->Name = L"LaunchProcessesButton";
			this->LaunchProcessesButton->Size = System::Drawing::Size(113, 29);
			this->LaunchProcessesButton->TabIndex = 1;
			this->LaunchProcessesButton->Text = L"Launch processes";
			this->LaunchProcessesButton->UseVisualStyleBackColor = true;
			this->LaunchProcessesButton->Click += gcnew System::EventHandler(this, &MyForm::LaunchProcessesButton_Click);
			// 
			// setAffinityToolStripMenuItem
			// 
			this->setAffinityToolStripMenuItem->Name = L"setAffinityToolStripMenuItem";
			this->setAffinityToolStripMenuItem->Size = System::Drawing::Size(32, 19);
			// 
			// toolStripTextBox1
			// 
			this->toolStripTextBox1->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9.07563F));
			this->toolStripTextBox1->Name = L"toolStripTextBox1";
			this->toolStripTextBox1->Size = System::Drawing::Size(100, 28);
			this->toolStripTextBox1->Text = L"12";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(-2, 297);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(0, 13);
			this->label1->TabIndex = 2;
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(55, 37);
			this->textBox1->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(420, 131);
			this->textBox1->TabIndex = 3;
			// 
			// processNumberTextBox
			// 
			this->processNumberTextBox->Location = System::Drawing::Point(166, 196);
			this->processNumberTextBox->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->processNumberTextBox->Name = L"processNumberTextBox";
			this->processNumberTextBox->Size = System::Drawing::Size(107, 20);
			this->processNumberTextBox->TabIndex = 4;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(52, 196);
			this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(110, 13);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Number of processes:";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(692, 337);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->processNumberTextBox);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->LaunchProcessesButton);
			this->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


	private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	
	private: int processNumber = 0;

private:
	void MapLinesToSharedMemory();
	
	HANDLE launchChildProcess();
	
	System::Void LaunchProcessesButton_Click(System::Object^ sender, System::EventArgs^ e);
	
	System::Void WaitForChildProcesses();

};
}


