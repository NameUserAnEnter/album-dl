#include "Console.h"


Console::Console(std::wstring _logFilepath, std::wstring* _pOutputBuffer) : logFilepath(_logFilepath), pOutputBuffer(_pOutputBuffer)
{
	hLogWrite = NULL;
	hLogRead = NULL;

	ActiveHandles.clear();

	GetFileHandle(logFilepath.c_str(), CREATE_ALWAYS, &hLogWrite, true, FILE_SHARE_READ, GENERIC_WRITE);
	AddActiveHandle(hLogWrite);

	bDone = false;
	bConsoleDone = false;
}

Console::~Console()
{
	ExitSafe(ERR_SUCCESS);
}

void Console::RunConsole()
{
	GetFileHandle(L"log", CREATE_ALWAYS, &hLogWrite, true, FILE_SHARE_READ, GENERIC_WRITE);
	AddActiveHandle(hLogWrite);

	std::thread th1(&WriteLog);
	std::thread th2(&ReadLog);

	th1.join();
	th2.join();



	PrintConsole("Batch job finished, press 'x': ");
	bConsoleDone = true;

	ExitSafe(ERR_SUCCESS);
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
	RunProcess("D:/conversion/tbdl/yt-dlp.exe -f bestaudio -o \"D:/conversion/tbdl/td5_output1.mp4\" \"https://www.youtube.com/watch?v=uog1U_Kemlk\"");
	PrintLog("\n\n");
	RunProcess("D:/conversion/tbdl/ffmpeg.exe -i \"D:/conversion/tbdl/td5_output1.mp4\" -c:a mp3 -b:a 192k -ar 44100 \"D:/conversion/tbdl/td5_output2_12.mp3\"");
	PrintLog("\n\n");
	RunProcess("cmd /c \"MOVE \"D:\\conversion\\tbdl\\td5_output1.mp4\" \"D:\\conversion\\tbdl\\Move to Recycle Bin\"\"");
}

void Console::RunProcess(std::string path)
{
	PrintLog("Executing process:\n" + path + "\n\n");

	STARTUPINFO startupInfo ={ };
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdOutput = hLogWrite;
	startupInfo.hStdError = hLogWrite;
	startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

	PROCESS_INFORMATION processInfo ={ };
	if (!CreateProcess(NULL, (wchar_t*)toWide(path).c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) ErrorWithCode("CreateProcess", GetLastError());

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




void Console::GetFileHandle(const wchar_t* path, DWORD dwCreationDisposition, HANDLE* hDest, bool bInheritable = false,
				   DWORD dwShareMode = NULL, DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE)
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





void Console::Error(std::string function, unsigned long exit_code = ERR_DEFAULT)
{
	std::string output = ErrorOutput(function);

	if (exit_code != ERR_SUCCESS) MessageDialog(output);
	ExitSafe(exit_code);
}

void Console::ErrorWithCode(std::string function, unsigned long external_code, unsigned long exit_code = ERR_DEFAULT)
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

void Console::ExitSafe(unsigned long error_code)
{
	for (int i = 0; i < ActiveHandles.size(); i++)
	{
		CloseProperHandle(ActiveHandles[ActiveHandles.size() - 1 - i]);
	}

	PrintConsole("\n\nSession finished\nExit code: " + NumToStr(error_code) + " (" + HexToStr(error_code) +")\n");
	//ExitProcess(error_code);
}

void Console::CloseProperHandle(HANDLE hHandle)
{
	if (hHandle != NULL && hHandle != INVALID_HANDLE_VALUE)
	{
		if (!CloseHandle(hHandle)) ErrorWithCode("CloseHandle", GetLastError());
	}
}

