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
			
			 countdownTimer->Interval = 1000;
			 countdownTimer->Tick += gcnew EventHandler(this, &MyForm::CountdownTimer_Tick);

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
	private: System::Windows::Forms::Timer^ countdownTimer;
	private: System::Windows::Forms::Label^ timeLabel;





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
			this->components = (gcnew System::ComponentModel::Container());
			this->LaunchProcessesButton = (gcnew System::Windows::Forms::Button());
			this->setAffinityToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripTextBox1 = (gcnew System::Windows::Forms::ToolStripTextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->processNumberTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->countdownTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->timeLabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// LaunchProcessesButton
			// 
			this->LaunchProcessesButton->Location = System::Drawing::Point(115, 299);
			this->LaunchProcessesButton->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->LaunchProcessesButton->Name = L"LaunchProcessesButton";
			this->LaunchProcessesButton->Size = System::Drawing::Size(151, 36);
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
			this->label1->Location = System::Drawing::Point(-3, 366);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(0, 16);
			this->label1->TabIndex = 2;
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(73, 46);
			this->textBox1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(559, 160);
			this->textBox1->TabIndex = 3;
			// 
			// processNumberTextBox
			// 
			this->processNumberTextBox->Location = System::Drawing::Point(221, 241);
			this->processNumberTextBox->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->processNumberTextBox->Name = L"processNumberTextBox";
			this->processNumberTextBox->Size = System::Drawing::Size(141, 22);
			this->processNumberTextBox->TabIndex = 4;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(69, 241);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(139, 16);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Number of processes:";
			// 
			// timeLabel
			// 
			this->timeLabel->AutoSize = true;
			this->timeLabel->Location = System::Drawing::Point(698, 96);
			this->timeLabel->Name = L"timeLabel";
			this->timeLabel->Size = System::Drawing::Size(14, 16);
			this->timeLabel->TabIndex = 7;
			this->timeLabel->Text = L"0";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(923, 415);
			this->Controls->Add(this->timeLabel);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->processNumberTextBox);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->LaunchProcessesButton);
			this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
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
	private: int remainingTime;


private:
	void MapLinesToSharedMemory();
	
	HANDLE launchChildProcess();
	
	System::Void LaunchProcessesButton_Click(System::Object^ sender, System::EventArgs^ e);

	void CountdownTimer_Tick(Object^ sender, EventArgs^ e);

	String^ FormatTime(int seconds);
	
	System::Void WaitForChildProcesses();

};
}


