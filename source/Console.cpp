#include "Console.h"


Console::~Console()
{
	for (int i = 0; i < ActiveHandles.size(); i++)
	{
		CloseProperHandle(ActiveHandles[ActiveHandles.size() - 1 - i]);
	}

	ActiveHandles.clear();
	cmdLines.clear();
}



void Console::InitValues()
{
	bInit = false;
	bLogOpen = false;
	bDumpBytes = false;
	bWrap = false;

	logFilepath = L"";
	pOutputBuffer = nullptr;

	hLogWrite = NULL;

	hSubOutWr = NULL;
	hSubOutRd = NULL;

	ActiveHandles.clear();
	cmdLines.clear();

	currentCmdIndex = 0;

	uCurrentColumn = 0;
	uBufferWidth = 80;

	dumpName = "dump_console";
}

Console::Console()
{
	InitValues();
}

void Console::InitConsole(std::wstring _logFilepath, std::wstring* _pOutputBuffer,
						  bool _bWrap, unsigned int _uBufferWidth)
{
	if (bInit) ErrMsgExit("Tried to re-initialize a Console object.");
	InitValues();

	logFilepath = _logFilepath;
	pOutputBuffer = _pOutputBuffer;

	bWrap = _bWrap;
	uBufferWidth = _uBufferWidth;

	InitSubOutputPipe();

	bLogOpen = false;
	bInit = true;
}


std::mutex* Console::GetPrintMutex()
{
	return &printMutex;
}



void Console::AddCmd(std::wstring cmdLine, OUTPUT_MODE mode)
{
	if (mode == OUTPUT_MODE::DEFAULT)
	{
		if (beginWith(cmdLine, L"cmd /u") || beginWith(cmdLine, L"CMD /U")) mode = FIXED_UNICODE16;
		else mode = UTF8;
	}
	cmdLines.push_back({ cmdLine, mode });
}

void Console::AddCmd(std::vector<std::wstring> newCmdLines, OUTPUT_MODE mode)
{
	for (auto cmdLine : newCmdLines)
	{
		if (mode == OUTPUT_MODE::DEFAULT)
		{
			if (beginWith(cmdLine, L"cmd /u") || beginWith(cmdLine, L"CMD /U")) mode = FIXED_UNICODE16;
			else mode = UTF8;
		}

		cmdLines.push_back({ cmdLine, mode });
	}
}

void Console::TrashCmds()
{
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
	if (bDumpBytes)
	{
		ClearBytesDump(dumpName);
	}


	currentCmdIndex = 0;
	while (currentCmdIndex < cmdLines.size())
	{
		RunProcess(cmdLines[currentCmdIndex].line);
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
	PrintLogAndConsoleNarrow( "----------------------------   Output mode: " + GetModeStr() + "\n");
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

	if (bWrap)
	{
		std::wstring copy = L"";
		for (int i = 0; i < buf.size(); i++)
		{
			copy += buf[i];
			uCurrentColumn++;

			if (uCurrentColumn >= uBufferWidth)
			{
				copy += L'\n';
			}
			if (copy.back() == L'\n') uCurrentColumn = 0;
		}
		buf = copy;
	}

	PrintConsole(buf);

	if (!bLogOpen) return;
	AppendDataToFile(EncodeToUTF8(buf), logFilepath);
}

void Console::PrintLogAndConsoleNarrow(std::string buf)
{
	PrintLogAndConsole(toWide(buf));
}

void Console::PrintConsole(std::wstring buf)
{
	if (!bInit) return;

	//MessageDialog("test");
	std::lock_guard<std::mutex> bufLock(printMutex);
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



void Console::GetSubOutput()
{
	for (;;)
	{
		std::string buf;
		buf.resize(GetPipeBufSize());

		if (!buf.empty())
		{
			Read(hSubOutRd, buf);
			if (bDumpBytes)
			{
				static unsigned int pos = 0;
				AppendBytesDump(buf, dumpName, pos);
			}


			switch (cmdLines[currentCmdIndex].mode)
			{
				case FIXED_UNICODE16:
					PrintLogAndConsole(GetWideFromFixedUnicode16(buf.c_str()));
					break;
				case WINDOWS1250:
					PrintLogAndConsole(GetWideFromCodePage(buf.c_str(), codepage::table_CP1250));
					break;
				
				case UTF8:
				default:
					PrintLogAndConsole(DecodeFromUTF8(buf));
					break;
			}
		}
		

		if (!GetPipeBufSize()) break;
	}
}




std::string Console::GetModeStr()
{
	std::string modeStr = "";
	if (cmdLines[currentCmdIndex].mode == FIXED_UNICODE16) modeStr = "FIXED_UNICODE16";
	if (cmdLines[currentCmdIndex].mode == WINDOWS1250) modeStr = "WINDOWS1250";
	if (cmdLines[currentCmdIndex].mode == UTF8) modeStr = "UTF8";
	return modeStr;
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

	ClearFileData(logFilepath);
	bLogOpen = true;
}

void Console::CloseLog()
{
	if (!bInit) return;
	if (!bLogOpen) return;


	bLogOpen = false;
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

