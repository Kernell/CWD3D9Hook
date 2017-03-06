#pragma once

namespace loader
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для Loader
	/// </summary>
	public ref class Loader : public System::Windows::Forms::Form
	{
	public:
		Loader(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~Loader()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  ButtonRefresh;
	private: System::Windows::Forms::GroupBox^  ProcessListGroup;
	protected:

	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatus;
	private: System::Windows::Forms::Button^  ButtonInject;
	private: System::Windows::Forms::DataGridView^  ProcessList;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  ColumnPID;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  ColumnName;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  ColumnWindowName;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog;





















	protected:

	protected:

	private:
		/// <summary>
		/// Требуется переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Обязательный метод для поддержки конструктора - не изменяйте
		/// содержимое данного метода при помощи редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->ButtonRefresh = ( gcnew System::Windows::Forms::Button() );
			this->ProcessListGroup = ( gcnew System::Windows::Forms::GroupBox() );
			this->ButtonInject = ( gcnew System::Windows::Forms::Button() );
			this->ProcessList = ( gcnew System::Windows::Forms::DataGridView() );
			this->ColumnPID = ( gcnew System::Windows::Forms::DataGridViewTextBoxColumn() );
			this->ColumnName = ( gcnew System::Windows::Forms::DataGridViewTextBoxColumn() );
			this->ColumnWindowName = ( gcnew System::Windows::Forms::DataGridViewTextBoxColumn() );
			this->statusStrip1 = ( gcnew System::Windows::Forms::StatusStrip() );
			this->toolStripStatus = ( gcnew System::Windows::Forms::ToolStripStatusLabel() );
			this->openFileDialog = ( gcnew System::Windows::Forms::OpenFileDialog() );
			this->ProcessListGroup->SuspendLayout();
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->ProcessList ) )->BeginInit();
			this->statusStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// ButtonRefresh
			// 
			this->ButtonRefresh->Location = System::Drawing::Point( 6, 359 );
			this->ButtonRefresh->Name = L"ButtonRefresh";
			this->ButtonRefresh->Size = System::Drawing::Size( 75, 23 );
			this->ButtonRefresh->TabIndex = 0;
			this->ButtonRefresh->Text = L"Refresh";
			this->ButtonRefresh->UseVisualStyleBackColor = true;
			this->ButtonRefresh->Click += gcnew System::EventHandler( this, &Loader::ButtonRefresh_Click );
			// 
			// ProcessListGroup
			// 
			this->ProcessListGroup->Controls->Add( this->ButtonInject );
			this->ProcessListGroup->Controls->Add( this->ProcessList );
			this->ProcessListGroup->Controls->Add( this->ButtonRefresh );
			this->ProcessListGroup->Location = System::Drawing::Point( 12, 12 );
			this->ProcessListGroup->Name = L"ProcessListGroup";
			this->ProcessListGroup->Size = System::Drawing::Size( 395, 388 );
			this->ProcessListGroup->TabIndex = 1;
			this->ProcessListGroup->TabStop = false;
			this->ProcessListGroup->Text = L"Process list";
			// 
			// ButtonInject
			// 
			this->ButtonInject->Location = System::Drawing::Point( 314, 359 );
			this->ButtonInject->Name = L"ButtonInject";
			this->ButtonInject->Size = System::Drawing::Size( 75, 23 );
			this->ButtonInject->TabIndex = 2;
			this->ButtonInject->Text = L"Inject";
			this->ButtonInject->UseVisualStyleBackColor = true;
			this->ButtonInject->Click += gcnew System::EventHandler( this, &Loader::ButtonInject_Click );
			// 
			// ProcessList
			// 
			this->ProcessList->AllowUserToAddRows = false;
			this->ProcessList->AllowUserToDeleteRows = false;
			this->ProcessList->AllowUserToOrderColumns = true;
			this->ProcessList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->ProcessList->Columns->AddRange( gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >( 3 )
			{
				this->ColumnPID,
					this->ColumnName, this->ColumnWindowName
			} );
			this->ProcessList->Location = System::Drawing::Point( 6, 19 );
			this->ProcessList->MultiSelect = false;
			this->ProcessList->Name = L"ProcessList";
			this->ProcessList->ReadOnly = true;
			this->ProcessList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->ProcessList->Size = System::Drawing::Size( 383, 334 );
			this->ProcessList->TabIndex = 1;
			// 
			// ColumnPID
			// 
			this->ColumnPID->HeaderText = L"PID";
			this->ColumnPID->Name = L"ColumnPID";
			this->ColumnPID->ReadOnly = true;
			this->ColumnPID->Width = 40;
			// 
			// ColumnName
			// 
			this->ColumnName->HeaderText = L"Name";
			this->ColumnName->Name = L"ColumnName";
			this->ColumnName->ReadOnly = true;
			this->ColumnName->Width = 140;
			// 
			// ColumnWindowName
			// 
			this->ColumnWindowName->HeaderText = L"Window Name";
			this->ColumnWindowName->Name = L"ColumnWindowName";
			this->ColumnWindowName->ReadOnly = true;
			this->ColumnWindowName->Width = 140;
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^  >( 1 )
			{
				this->toolStripStatus
			} );
			this->statusStrip1->Location = System::Drawing::Point( 0, 403 );
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size( 419, 22 );
			this->statusStrip1->SizingGrip = false;
			this->statusStrip1->TabIndex = 2;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// toolStripStatus
			// 
			this->toolStripStatus->Name = L"toolStripStatus";
			this->toolStripStatus->Size = System::Drawing::Size( 16, 17 );
			this->toolStripStatus->Text = L"...";
			// 
			// openFileDialog
			// 
			this->openFileDialog->Filter = L"Расширение приложения|*.dll";
			// 
			// Loader
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF( 6, 13 );
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size( 419, 425 );
			this->Controls->Add( this->statusStrip1 );
			this->Controls->Add( this->ProcessListGroup );
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"Loader";
			this->Text = L"Loader";
			this->Load += gcnew System::EventHandler( this, &Loader::Loader_Load );
			this->ProcessListGroup->ResumeLayout( false );
			( cli::safe_cast<System::ComponentModel::ISupportInitialize^>( this->ProcessList ) )->EndInit();
			this->statusStrip1->ResumeLayout( false );
			this->statusStrip1->PerformLayout();
			this->ResumeLayout( false );
			this->PerformLayout();

		}
#pragma endregion
private:
	Void Loader_Load( System::Object^  sender, System::EventArgs^  e );
	Void ButtonRefresh_Click( System::Object^  sender, System::EventArgs^  e );

	Void LoadProcesses( Void );
	Void ButtonInject_Click( System::Object^  sender, System::EventArgs^  e );

public:
	Void Inject( unsigned long ulPID, System::String^ strFilePath );

};
}
