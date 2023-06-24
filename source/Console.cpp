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

	ExitSafe(ERR_SUCCESS);
	bConsoleDone = true;
	return;

	std::thread th1(&Console::WriteLog, this);
	std::thread th2(&Console::ReadLog, this);

	th1.join();
	th2.join();
}



void Console::WriteLog()
{
	PrintLog("-------Start of log.-------\n\n\n");

	RunBatch();
	bDone = true;
}

void Console::ReadLog()
{
	bool bLastIter = false;
	DWORD dwRead;
	unsigned int totalRead = 0;
	do
	{
		GetFileHandle(L"log", OPEN_EXISTING, &hLogRead, true, FILE_SHARE_WRITE | FILE_SHARE_READ, GENERIC_READ);
		AddActiveHandle(hLogRead);

		const unsigned int BUFSIZE = 80;
		unsigned char chBuf[BUFSIZE];

		if (SetFilePointer(hLogRead, totalRead, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ErrorWithCode("SetFilePointer", GetLastError());
		ReadFile(hLogRead, &chBuf, BUFSIZE, &dwRead, NULL);
		if (SetFilePointer(hLogRead, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ErrorWithCode("SetFilePointer", GetLastError());

		totalRead += dwRead;
		RemoveActiveHandle(hLogRead);

		std::string strBuf = "";
		for (int i = 0; i < dwRead; i++) strBuf += chBuf[i];
		PrintConsole(strBuf);


		if (bDone && dwRead == 0) break;
	} while (true);
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
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdOutput = hLogWrite;
	startupInfo.hStdError = hLogWrite;
	//startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

	PROCESS_INFORMATION processInfo ={ };
	if (!CreateProcess(NULL, (wchar_t*)path.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) ErrorWithCode("CreateProcess", GetLastError());

	DWORD dwExitCode;
	do
	{
		GetExitCodeProcess(processInfo.hProcess, &dwExitCode);
	} while (dwExitCode == STILL_ACTIVE);
	PrintLog("\n\nProcess finished with exit code: " + HexToStr(dwExitCode) + "\n");
	PrintLog("----------------------------------------------------------------------\n");



	CloseProperHandle(processInfo.hThread);
	CloseProperHandle(processInfo.hProcess);
}







void Console::PrintLog(std::string buf)
{
	Write(hLogWrite, buf);
}

void Console::PrintConsole(std::string buf)
{
	//Write(GetStdHandle(STD_OUTPUT_HANDLE), buf);
	*pOutputBuffer += toWide(buf);
}


void Console::Write(HANDLE hOut, std::string buf)
{
	DWORD dwWritten;
	if (!WriteFile(hOut, buf.c_str(), buf.size() * sizeof(buf[0]), &dwWritten, NULL)) ErrorWithCode("WriteFile", GetLastError(), ERR_WRITE);
}

void Console::Read(HANDLE hIn, std::string& buf)
{
	DWORD dwRead;

	char* chBuf = (char*)calloc(buf.size(), sizeof(char));
	if (!ReadFile(hIn, chBuf, buf.size() * sizeof(buf[0]), &dwRead, NULL))
	{
		free(chBuf);
		ErrorWithCode("ReadFile", GetLastError(), ERR_READ);
	}
	buf.resize(dwRead);

	for (int i = 0; i < buf.size(); i++) buf[i] = chBuf[i];
	free(chBuf);
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

	PrintConsole("Session has finished. Exit code: " + NumToStr(exit_code) + " (" + HexToStr(exit_code) + ")");
	//ExitProcess(error_code);
}

void Console::CloseProperHandle(HANDLE hHandle)
{
	if (hHandle != NULL && hHandle != INVALID_HANDLE_VALUE)
	{
		if (!CloseHandle(hHandle)) ErrorWithCode("CloseHandle", GetLastError());
	}
}

