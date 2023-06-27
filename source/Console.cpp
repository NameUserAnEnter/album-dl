#include "Console.h"


Console::Console(std::wstring _logFilepath, std::wstring* _pOutputBuffer) : logFilepath(_logFilepath), pOutputBuffer(_pOutputBuffer)
{
	hLogWrite = NULL;
	hLogRead = NULL;

	ActiveHandles.clear();
	cmdLines.clear();


	bDone = false;
	bConsoleDone = false;
}

void Console::AddCmdLine(std::wstring cmdLine)
{
	cmdLines.push_back(cmdLine);
}

Console::~Console()
{
	ActiveHandles.clear();
	cmdLines.clear();
}

void Console::RunConsole()
{
	GetFileHandle(logFilepath.c_str(), CREATE_ALWAYS, &hLogWrite, true, FILE_SHARE_READ, GENERIC_WRITE);
	AddActiveHandle(hLogWrite);

	thWrite = std::move(std::thread(&Console::WriteLog, this));
	thRead = std::move(std::thread(&Console::ReadLog, this));

	thWrite.join();
	thRead.join();

	ExitSafe(ERR_SUCCESS);
	bConsoleDone = true;
	return;
}



void Console::WriteLog()
{
	/*
	PrintLog("----------------------------   Start of session.   ----------------------------\n");
	
	PrintLog("----------------------------   Time: [");
	std::tm current_time = GetCurrentDateAndTime();
	unsigned int mt = current_time.tm_mon;
	unsigned int dd = current_time.tm_mday;
	unsigned int yyyy = current_time.tm_year;
	unsigned int hh = current_time.tm_hour;
	unsigned int mn = current_time.tm_min;
	unsigned int ss = current_time.tm_sec;
	PrintLog(NumToStr(mt, 10, 2) + "-");
	PrintLog(NumToStr(dd, 10, 2) + "-");
	PrintLog(NumToStr(yyyy, 10, 4) + ": ");

	PrintLog(NumToStr(hh, 10, 2) + ":");
	PrintLog(NumToStr(mn, 10, 2) + ":");
	PrintLog(NumToStr(ss, 10, 2));

	PrintLog("]\n");
	PrintLog("-------------------------------------------------------------------------------\n");
	*/
	PrintLog(L"����朳\n");
	//PrintLog("\n");


	RunBatch();
	bDone = true;
}


void Console::RunBatch()
{
	for (int i = 0; i < cmdLines.size(); i++)
	{
		RunProcess(cmdLines[i]);
		if (i + 1 < cmdLines.size()) PrintLog(L"\n\n");
	}
}

void Console::RunProcess(std::wstring wPath)
{
	if (wPath.empty()) return;

	// arg matches CreateProcessW
	std::wstring path = wPath;

	
	//setlocale(LC_ALL, ".UTF8");
	
	bool bTest1 = false;
	if (bTest1)
	{
		///*
		AllocConsole();

#define test_str "Ch곳� W�r�d.\n"
#define utest_str u8"Ch곳� W�r�d.\n"
#define wtest_str L"Ch곳� W�r�d.\n"
		std::wstring str = wtest_str;
		std::string encoded_str = EncodeToUTF8(str);

		UINT old_CP = 852;
		UINT new_CP = 65001;

		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		Write(hOut, "OUTPUT CODE-PAGE: " + NumToStr(GetConsoleOutputCP()) + "\n");
		Write(hOut, "CODE-PAGE: " + NumToStr(GetConsoleCP()) + "\n");
		Write(hOut, EncodeToUTF8(wtest_str));
		Write(hOut, utest_str);
		Write(hOut, test_str);

		if (!SetConsoleCP(new_CP)) ErrorWithCode("SetConsoleOutputCP", GetLastError());
		if (!SetConsoleOutputCP(new_CP)) ErrorWithCode("SetConsoleOutputCP", GetLastError());

		Write(hOut, "\nOUTPUT CODE-PAGE: " + NumToStr(GetConsoleOutputCP()) + "\n");
		Write(hOut, "CODE-PAGE: " + NumToStr(GetConsoleCP()) + "\n");
		Write(hOut, EncodeToUTF8(wtest_str));
		Write(hOut, utest_str);
		Write(hOut, test_str);

		if (!SetConsoleCP(old_CP)) ErrorWithCode("SetConsoleOutputCP", GetLastError());
		if (!SetConsoleOutputCP(old_CP)) ErrorWithCode("SetConsoleOutputCP", GetLastError());

		Write(hOut, "\n");
		if (!WriteConsoleW(hOut, str.c_str(), str.size(), NULL, NULL)) ErrorWithCode("WriteFile", GetLastError());
		if (!WriteConsoleA(hOut, encoded_str.c_str(), encoded_str.size(), NULL, NULL)) ErrorWithCode("WriteFile", GetLastError());

		Write(hOut, "\n");
		if (!WriteFile(hOut, str.c_str(), str.size() * sizeof(str[0]), NULL, NULL)) ErrorWithCode("WriteFile", GetLastError());
		if (!WriteFile(hOut, encoded_str.c_str(), encoded_str.size() * sizeof(encoded_str[0]), NULL, NULL)) ErrorWithCode("WriteFile", GetLastError());
		return;
		//*/
	}

	auto typeVar = path[0];
	// CreateProcess' lpCommandLine arg has to be non-const
	decltype(typeVar)* szPath = (decltype(typeVar)*)calloc(path.size() + 1, sizeof(decltype(typeVar)));
	if (szPath == nullptr) Error("calloc");		// exits the program if nullptr
	
	// TEST TYPE DEDUCTION:
	/*
	MessageDialog("typeVar's type: " + std::string(typeid(decltype(typeVar)).name()) + "\nszPath's type: " + std::string(typeid(decltype(szPath)).name())
					+ "\npath.c_str()'s type: " + std::string(typeid(decltype(path.c_str())).name()));
	free(szPath);
	return;
	*/
	//----------------------------------------------------------

	
	
	
	for (int i = 0; i < path.size(); i++) szPath[i] = path[i];
	szPath[path.size()] = '\0';


	
	PrintLog(L"Executing process:\n" + (wPath) + L"\n\n");

	bool bUseNativeConsole = false;
	STARTUPINFO startupInfo = { };
	
	if (bUseNativeConsole) AllocConsole();
	else startupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	startupInfo.hStdOutput = hLogWrite;
	startupInfo.hStdError = hLogWrite;

	DWORD dwCreationFlags = 0;
	//dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
	//dwCreationFlags |= CREATE_NO_WINDOW;

	PROCESS_INFORMATION processInfo = { };
	// switch to CreateProcessA for utf-8?
	BOOL bResult = CreateProcess(NULL, szPath, NULL, NULL, TRUE, dwCreationFlags,
								  NULL, NULL, &startupInfo, &processInfo);
	free(szPath);	// after CreateProcess() is done szPath has to be deallocated regardless of the result
	if (!bResult) ErrorWithCode("CreateProcess", GetLastError());



	DWORD dwExitCode;
	do
	{
		GetExitCodeProcess(processInfo.hProcess, &dwExitCode);
	} while (dwExitCode == STILL_ACTIVE);
	PrintLog(L"\n\nProcess finished with exit code: " + toWide(HexToStr(dwExitCode)) + L"\n");
	PrintLog(L"----------------------------     End of process.   ----------------------------\n");


	
	CloseProperHandle(processInfo.hThread);
	CloseProperHandle(processInfo.hProcess);
}







void Console::PrintLog(std::wstring buf)
{
	std::lock_guard<std::mutex> filePosLock(filePosMutex);
	Write(hLogWrite, EncodeToUTF8(buf));
}

void Console::PrintConsole(std::wstring buf)
{
	//Write(GetStdHandle(STD_OUTPUT_HANDLE), buf);

	std::lock_guard<std::mutex> bufLock(outputBufMutex);
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

	if (!ReadFile(hIn, chBuf, buf.size() * sizeof(chBuf[0]), &dwRead, NULL))
	{
		free(chBuf);
		ErrorWithCode("ReadFile", GetLastError(), ERR_READ);
	}
	
	std::string encoded = "";
	for (int i = 0; i < dwRead; i++) encoded += chBuf[i];
	free(chBuf);

	buf = encoded;
}



void Console::ReadLog()
{
	bool bLastIter = false;
	DWORD dwRead;
	unsigned int totalRead = 0;

	for (;;)
	{
		GetFileHandle(logFilepath.c_str(), OPEN_EXISTING, &hLogRead, true, FILE_SHARE_WRITE | FILE_SHARE_READ, GENERIC_READ);
		AddActiveHandle(hLogRead);

		const unsigned int BUFSIZE = 4096;
		//unsigned char chBuf[BUFSIZE];
		std::string buf;
		buf.resize(BUFSIZE);

		{
			std::lock_guard<std::mutex> filePosLock(filePosMutex);
			if (SetFilePointer(hLogRead, totalRead, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ErrorWithCode("SetFilePointer", GetLastError());
		}
		//ReadFile(hLogRead, &chBuf, BUFSIZE, &dwRead, NULL);
		Read(hLogRead, buf);
		dwRead = buf.size();
		{
			std::lock_guard<std::mutex> filePosLock(filePosMutex);
			if (SetFilePointer(hLogRead, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ErrorWithCode("SetFilePointer", GetLastError());
		}

		totalRead += dwRead;
		RemoveActiveHandle(hLogRead);

		std::string encoded = buf;

		std::wstring decoded = DecodeFromUTF8(encoded);
		PrintConsole(decoded);
		//PrintConsole(toWide(encoded));

		if (bLastIter)
		{
			if (dwRead == 0) break;
			else continue;
		}
		if (bDone && dwRead == 0)
		{
			bLastIter = true;
			continue;
		}
	}
}




void Console::GetFileHandle(const wchar_t* path, DWORD dwCreationDisposition, HANDLE* hDest, bool bInheritable,
							DWORD dwShareMode, DWORD dwDesiredAccess)
{
	if (!bInheritable) *hDest = CreateFile(path, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	else
	{
		SECURITY_ATTRIBUTES secAttr ={ };
		secAttr.nLength = sizeof(secAttr);
		secAttr.lpSecurityDescriptor = NULL;
		secAttr.bInheritHandle = true;

		*hDest = CreateFile(path, dwDesiredAccess, dwShareMode, &secAttr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
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
	std::string output = ErrorOutput(function);

	if (exit_code != ERR_SUCCESS) MessageDialog(output);
	ExitSafe(exit_code);
}

void Console::ErrorWithCode(std::string function, unsigned long external_code, unsigned long exit_code)
{
	std::string output = ErrorOutput(function, external_code);

	if (exit_code != ERR_SUCCESS) MessageDialog(output);
	ExitSafe(exit_code);
}



std::string Console::ErrorOutput(std::string function)
{
	std::string output = "Failed to invoke " + function + "()";
	return output;
}

std::string Console::ErrorOutput(std::string function, unsigned long external_code)
{
	std::string output = ErrorOutput(function);
	output += ": " + NumToStr(external_code) + " (" + HexToStr(external_code) + ")";

	return output;
}

void Console::ExitSafe(unsigned long exit_code)
{
	for (int i = 0; i < ActiveHandles.size(); i++)
	{
		CloseProperHandle(ActiveHandles[ActiveHandles.size() - 1 - i]);
	}

	PrintConsole(L"Session has finished. Exit code: " + toWide(NumToStr(exit_code)) + L" (" + toWide(HexToStr(exit_code)) + L")\n");

	PrintConsole(L"----------------------------     End of session.   ----------------------------\n\n\n");
	if (exit_code != ERR_SUCCESS) ExitProcess(exit_code);
}

void Console::CloseProperHandle(HANDLE hHandle)
{
	if (hHandle != NULL && hHandle != INVALID_HANDLE_VALUE)
	{
		std::lock_guard<std::mutex> handleLock(handleMutex);
		if (!CloseHandle(hHandle)) ErrorWithCode("CloseHandle", GetLastError());
	}
}

