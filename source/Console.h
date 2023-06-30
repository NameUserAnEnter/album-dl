#ifndef CONSOLE_H
#define CONSOLE_H

#include "utils.h"
#include <thread>
#include <mutex>

#define _UNICODE
#include <wchar.h>
#include <tchar.h>

#include <locale.h>
#include <wchar.h>


#define ERR_SUCCESS		0x00
#define ERR_DEFAULT		0x01
#define ERR_WRITE		0x02
#define ERR_READ		0x03
#define ERR_FILE		0x04
#define ERR_PIPE		0x05


class Console
{
public:
	Console(std::wstring, std::wstring*);
	~Console();

	void AddCmdLine(std::wstring cmdLine);
public:
	std::mutex outputBufMutex;
	std::mutex filePosMutex;
	std::mutex handleMutex;
private:
	std::thread thWrite;
	std::thread thRead;
private:
	std::vector<HANDLE> ActiveHandles;
	HANDLE hLogWrite;
	HANDLE hLogRead;
	bool bDone;
public:
	bool bConsoleDone;
private:
	std::wstring logFilepath;
	std::wstring* pOutputBuffer;

	std::vector<std::wstring> cmdLines;
public:
	void RunConsole();
private:
	void WriteLog();
	void ReadLog();

	void RunBatch();
	void RunProcess(std::wstring);

	void PrintLogNarrow(std::string);
	void PrintLog(std::wstring buf);
	void PrintConsole(std::wstring buf);



	void Write(HANDLE hOut, std::string buf);
	void Read(HANDLE hIn, std::string& buf);




	void GetFileHandle(std::wstring, DWORD, HANDLE*, bool = false, DWORD = NULL, DWORD = GENERIC_READ | GENERIC_WRITE);
	

	void AddActiveHandle(HANDLE hActive);
	void RemoveActiveHandle(HANDLE hActive);

	void Error(std::string function, unsigned long exit_code = ERR_DEFAULT);
	void ErrorWithCode(std::string function, unsigned long external_code, unsigned long exit_code = ERR_DEFAULT);
	std::string ErrorOutput(std::string function);
	std::string ErrorOutput(std::string function, unsigned long external_code);


	void ExitSafe(unsigned long);
	void CloseProperHandle(HANDLE hHandle);
};

#endif

