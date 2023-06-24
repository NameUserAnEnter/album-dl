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
	PrintLog("----------------------------   Start of session.   ----------------------------\n");

	RunBatch();
	bDone = true;
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
		unsigned char chBuf[BUFSIZE];

		{
			std::lock_guard<std::mutex> filePosLock(filePosMutex);
			if (SetFilePointer(hLogRead, totalRead, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ErrorWithCode("SetFilePointer", GetLastError());
		}
		ReadFile(hLogRead, &chBuf, BUFSIZE, &dwRead, NULL);
		{
			std::lock_guard<std::mutex> filePosLock(filePosMutex);
			if (SetFilePointer(hLogRead, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ErrorWithCode("SetFilePointer", GetLastError());
		}

		totalRead += dwRead;
		RemoveActiveHandle(hLogRead);

		std::string encodedBuf = "";
		for (int i = 0; i < dwRead; i++) encodedBuf += chBuf[i];

		std::wstring decodedBuf = DecodeFromUTF8(encodedBuf);
		PrintConsole(decodedBuf);

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


void Console::RunBatch()
{
	for (int i = 0; i < cmdLines.size(); i++)
	{
		RunProcess(cmdLines[i]);
		if (i + 1 < cmdLines.size()) PrintLog("\n\n");
	}
}

void Console::RunProcess(std::wstring path)
{
	PrintLog("Executing process:\n" + EncodeToUTF8(path) + "\n\n");

	STARTUPINFO startupInfo ={ };
	startupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	startupInfo.hStdOutput = hLogWrite;
	startupInfo.hStdError = hLogWrite;
	//startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION processInfo ={ };
	if (!CreateProcess(NULL, (wchar_t*)path.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) ErrorWithCode("CreateProcess", GetLastError());

	DWORD dwExitCode;
	do
	{
		GetExitCodeProcess(processInfo.hProcess, &dwExitCode);
	} while (dwExitCode == STILL_ACTIVE);
	PrintLog("\n\nProcess finished with exit code: " + HexToStr(dwExitCode) + "\n");
	PrintLog("----------------------------     End of process.   ----------------------------\n");



	CloseProperHandle(processInfo.hThread);
	CloseProperHandle(processInfo.hProcess);
}







void Console::PrintLog(std::wstring buf)
{
	std::lock_guard<std::mutex> filePosLock(filePosMutex);
	Write(hLogWrite, buf);
}

void Console::PrintConsole(std::wstring buf)
{
	//Write(GetStdHandle(STD_OUTPUT_HANDLE), buf);

	std::lock_guard<std::mutex> bufLock(outputBufMutex);
	*pOutputBuffer += (buf);
}

void Console::PrintLog(std::string buf)
{
	std::lock_guard<std::mutex> filePosLock(filePosMutex);
	Write(hLogWrite, toWide(buf));
}

void Console::PrintConsole(std::string buf)
{
	PrintConsole(toWide(buf));
}


void Console::Write(HANDLE hOut, std::wstring buf)
{
	std::string encoded = EncodeToUTF8(buf);

	DWORD dwWritten;
	if (!WriteFile(hOut, encoded.c_str(), encoded.size() * sizeof(encoded[0]), &dwWritten, NULL)) ErrorWithCode("WriteFile", GetLastError(), ERR_WRITE);
}

void Console::Read(HANDLE hIn, std::wstring& buf)
{
	DWORD dwRead;

	char* chBuf = (char*)calloc(buf.size(), sizeof(char));
	if (!ReadFile(hIn, chBuf, buf.size() * sizeof(buf[0]), &dwRead, NULL))
	{
		free(chBuf);
		ErrorWithCode("ReadFile", GetLastError(), ERR_READ);
	}
	
	std::string encoded = "";
	for (int i = 0; i < dwRead; i++) encoded[i] += chBuf[i];
	free(chBuf);

	buf = DecodeFromUTF8(encoded);
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

	PrintConsole("Session has finished. Exit code: " + NumToStr(exit_code) + " (" + HexToStr(exit_code) + ")\n");

	PrintConsole("----------------------------     End of session.   ----------------------------\n\n\n");
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

