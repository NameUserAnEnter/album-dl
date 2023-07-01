#include "Console.h"


Console::Console()
{
	bInit = false;
	bLogOpen = false;

	logFilepath = L"";
	pOutputBuffer = nullptr;
	pPrintMutex = nullptr;

	hLogWrite = NULL;

	hSubOutWr = NULL;
	hSubOutRd = NULL;

	ActiveHandles.clear();
	cmdLines.clear();

	currentCmdIndex = 0;
}

void Console::InitConsole(std::wstring _logFilepath, std::wstring* _pOutputBuffer, std::mutex* _pPrintMutex)
{
	logFilepath = _logFilepath;
	pOutputBuffer = _pOutputBuffer;
	pPrintMutex = _pPrintMutex;

	hLogWrite = NULL;

	hSubOutWr = NULL;
	hSubOutRd = NULL;

	ActiveHandles.clear();
	cmdLines.clear();

	currentCmdIndex = 0;

	InitSubOutputPipe();

	bLogOpen = false;
	bInit = true;
}

void Console::AddCmd(std::wstring cmdLine)
{
	cmdLines.push_back(cmdLine);
}

void Console::AddCmd(std::vector<std::wstring> newCmdLines)
{
	for (auto cmd : newCmdLines) cmdLines.push_back(cmd);
}

void Console::TrashCmds()
{
	cmdLines.clear();
}

Console::~Console()
{
	for (int i = 0; i < ActiveHandles.size(); i++)
	{
		CloseProperHandle(ActiveHandles[ActiveHandles.size() - 1 - i]);
	}

	ActiveHandles.clear();
	cmdLines.clear();
}

void Console::RunSession()
{
	if (!bInit) return;
	PrintLogAndConsoleNarrow("----------------------------   Start of session.   ----------------------------\n");

	RunBatch();

	PrintLogAndConsoleNarrow("Session has finished. Exit code: " + (NumToStr(ERR_SUCCESS)) + " (" + (HexToStr(ERR_SUCCESS)) + ")\n");
	PrintLogAndConsoleNarrow("----------------------------   End of session.     ----------------------------\n");
}

void Console::RunBatch()
{
	currentCmdIndex = 0;
	while (currentCmdIndex < cmdLines.size())
	{
		RunProcess(cmdLines[currentCmdIndex]);
		currentCmdIndex++;

		if (currentCmdIndex < cmdLines.size()) PrintLogAndConsoleNarrow("\n\n");
	};
}

// CreateProc, CreateProcWrapper

void Console::RunProcess(std::wstring wPath)
{
	if (wPath.empty()) return;

	std::wstring path = (wPath);
	

	auto typeVar = path[0];
	decltype(typeVar)* szPath = (decltype(typeVar)*)calloc(path.size() + 1, sizeof(decltype(typeVar)));
	if (szPath == nullptr) Error("calloc");
	
	for (int i = 0; i < path.size(); i++) szPath[i] = path[i];
	szPath[path.size()] = '\0';

	PrintLogAndConsoleNarrow( "----------------------------   Time: " + GetDateAndTimeStr() + "\n");
	PrintLogAndConsole		(L"----------------------------   Executing process:\n" + (wPath) + L"\n");
	PrintLogAndConsoleNarrow( "----------------------------   Start of process.   ----------------------------\n\n");
	
	
	STARTUPINFO startupInfo = { };
	startupInfo.hStdOutput = hSubOutWr;
	startupInfo.hStdError = hSubOutWr;
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	
	DWORD dwCreationFlags = 0;
	dwCreationFlags |= CREATE_NO_WINDOW;

	PROCESS_INFORMATION processInfo = { };
	BOOL bResult = CreateProcessW(
		NULL, szPath, NULL, NULL, TRUE, dwCreationFlags,
		NULL, NULL, &startupInfo, &processInfo);
	free(szPath);
	if (!bResult) ErrorWithCode("CreateProcess", GetLastError());




	DWORD dwExitCode;
	do
	{
		if (!GetExitCodeProcess(processInfo.hProcess, &dwExitCode)) ErrorWithCode("GetExitCodeProcess", GetLastError());
		GetSubOutput();
	} while (dwExitCode == STILL_ACTIVE);

	PrintLogAndConsoleNarrow("\n\n");
	PrintLogAndConsoleNarrow("----------------------------   Time: " + GetDateAndTimeStr() + "\n");
	PrintLogAndConsoleNarrow("Process finished with exit code: " + (NumToStr(dwExitCode)) + " (" + (HexToStr(dwExitCode)) + ")\n");
	PrintLogAndConsoleNarrow("----------------------------   End of process.     ----------------------------\n");

	CloseProperHandle(processInfo.hThread);
	CloseProperHandle(processInfo.hProcess);
}







void Console::PrintLogAndConsole(std::wstring buf)
{
	if (!bInit) return;

	PrintConsole(buf);

	if (!bLogOpen) return;
	Write(hLogWrite, EncodeToUTF8(buf));
}

void Console::PrintLogAndConsoleNarrow(std::string buf)
{
	PrintLogAndConsole(toWide(buf));
}

void Console::PrintConsole(std::wstring buf)
{
	if (!bInit) return;

	std::lock_guard<std::mutex> bufLock(*pPrintMutex);
	*pOutputBuffer += (buf);
}


void Console::Write(HANDLE hOut, std::string buf)
{
	std::string encoded = buf;

	DWORD dwWritten;
	if (!WriteFile(hOut, encoded.c_str(), encoded.size() * sizeof(encoded[0]), &dwWritten, NULL)) ErrorWithCode("WriteFile", GetLastError(), ERR_WRITE);
}

void Console::Read(HANDLE hIn, std::string& buf)
{
	DWORD dwRead;

	unsigned char* chBuf = (unsigned char*)calloc(buf.size(), sizeof(unsigned char));
	if (chBuf == nullptr) Error("calloc");

	BOOL bResult = ReadFile(hIn, chBuf, buf.size() * sizeof(chBuf[0]), &dwRead, NULL);
	if (bResult)
	{
		std::string encoded = "";
		for (int i = 0; i < dwRead; i++) encoded += chBuf[i];

		buf = encoded;
	}

	free(chBuf);
	if (!bResult) ErrorWithCode("ReadFile", GetLastError(), ERR_READ);
}

unsigned long Console::GetPipeBufSize()
{
	unsigned long return_value = 0;
	if (!PeekNamedPipe(hSubOutRd, NULL, NULL, NULL, &return_value, NULL)) ErrorWithCode("PeekNamedPipe", GetLastError());
	return return_value;
}



std::wstring Console::GetWideFromRawCodePoints(const char* raw)
{
	std::wstring raw_input = L"";
	for (int i = 0; raw[i] != '\0'; i += 2)
	{
		raw_input.push_back(0x00);
		raw_input.back() += (unsigned char)raw[i];
		raw_input.back() += (unsigned short)(raw[i + 1] << 8);
	}
	return raw_input;
}

void Console::GetSubOutput()
{
	for (;;)
	{
		std::string buf;
		buf.resize(GetPipeBufSize());

		if (!buf.empty())
		{
			Read(hSubOutRd, buf);

			if (beginWith(cmdLines[currentCmdIndex], L"cmd /u") || beginWith(cmdLines[currentCmdIndex], L"CMD /U"))
			{
				PrintLogAndConsole(GetWideFromRawCodePoints(buf.c_str()));
			}
			else PrintLogAndConsole(DecodeFromUTF8(buf));
		}
		

		if (!GetPipeBufSize()) break;
	}
}



void Console::InitSubOutputPipe()
{
	SECURITY_ATTRIBUTES secAttr = { };
	secAttr.bInheritHandle = TRUE;
	secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAttr.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&hSubOutRd, &hSubOutWr, &secAttr, 0)) ErrorWithCode("CreatePipe", GetLastError(), ERR_PIPE);
	AddActiveHandle(hSubOutRd);
	AddActiveHandle(hSubOutWr);
}

void Console::OpenLog()
{
	if (!bInit) return;

	GetFileHandle(logFilepath.c_str(), CREATE_ALWAYS, &hLogWrite, true, FILE_SHARE_READ, GENERIC_WRITE);
	AddActiveHandle(hLogWrite);

	bLogOpen = true;
}

void Console::CloseLog()
{
	if (!bInit) return;
	if (!bLogOpen) return;

	RemoveActiveHandle(hLogWrite);
	bLogOpen = false;
}

void Console::GetFileHandle(std::wstring wPath, DWORD dwCreationDisposition, HANDLE* hDest, bool bInheritable,
							DWORD dwShareMode, DWORD dwDesiredAccess)
{
	std::wstring path = (wPath);

	if (!bInheritable) *hDest = CreateFileW(path.c_str(), dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	else
	{
		SECURITY_ATTRIBUTES secAttr ={ };
		secAttr.nLength = sizeof(secAttr);
		secAttr.lpSecurityDescriptor = NULL;
		secAttr.bInheritHandle = true;

		*hDest = CreateFileW(path.c_str(), dwDesiredAccess, dwShareMode, &secAttr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (*hDest == INVALID_HANDLE_VALUE) ErrorWithCode("CreateFile", GetLastError(), ERR_FILE);
}






void Console::AddActiveHandle(HANDLE hActive)
{
	for (int i = 0; i < ActiveHandles.size(); i++)
	{
		if (ActiveHandles[i] == hActive) return;
	}
	ActiveHandles.push_back(hActive);
}

void Console::RemoveActiveHandle(HANDLE hActive)
{
	for (int i = 0; i < ActiveHandles.size(); i++)
	{
		if (ActiveHandles[i] == hActive)
		{
			CloseProperHandle(ActiveHandles[i]);
			for (int j = i; j + 1 < ActiveHandles.size(); j++)
			{
				ActiveHandles[j] = ActiveHandles[j + 1];
			}

			ActiveHandles.pop_back();
			break;
		}
	}
}





void Console::Error(std::string function, unsigned long exit_code)
{
	std::string output = FuncErrOutput(function);

	if (exit_code != ERR_SUCCESS) MessageDialog(output);
	ExitProcess(exit_code);	// the destructor contains the kill-handles loop, so end the lifetime by ending process
}

void Console::ErrorWithCode(std::string function, unsigned long external_code, unsigned long exit_code)
{
	std::string output = FuncErrOutput(function, external_code);

	if (exit_code != ERR_SUCCESS) MessageDialog(output);
	ExitProcess(exit_code);
}




void Console::CloseProperHandle(HANDLE hHandle)
{
	if (hHandle != NULL && hHandle != INVALID_HANDLE_VALUE)
	{
		if (!CloseHandle(hHandle)) ErrorWithCode("CloseHandle", GetLastError());
	}
}

