#ifndef CONSOLE_H
#define CONSOLE_H

#include "encoding.h"
#include "utils.h"
#include <mutex>



#define ERR_SUCCESS		0x00
#define ERR_DEFAULT		0x01
#define ERR_WRITE		0x02
#define ERR_READ		0x03
#define ERR_FILE		0x04
#define ERR_PIPE		0x05
#define ERR_INTERRUPTED	0x06

enum OUTPUT_MODE
{
	FIXED_UNICODE16,
	WINDOWS1250,
	UTF8,
	DEFAULT				// DEFAULT SETS TO ONE OF THE OTHERS IN AddCmd()s
};

struct CMD
{
	std::wstring line;
	OUTPUT_MODE mode;
};


class Console
{
public:
	Console();
	void InitConsole(std::wstring, std::wstring*, bool = false, unsigned int = 80);
	~Console();

	void AddCmd(std::wstring, OUTPUT_MODE = DEFAULT);
	void AddCmd(std::vector<std::wstring>, OUTPUT_MODE = DEFAULT);
	void TrashCmds();

	std::mutex* GetPrintMutex();
private:
	std::mutex printMutex;
	bool bDumpBytes;
	bool bInit;
	bool bLogOpen;

	unsigned int uCurrentColumn;
	unsigned int uBufferWidth;
	bool bWrap;

	std::vector<HANDLE> ActiveHandles;
	HANDLE hLogWrite;

	HANDLE hSubOutWr;
	HANDLE hSubOutRd;
private:
	std::wstring logFilepath;
	std::wstring* pOutputBuffer;

	std::vector<CMD> cmdLines;
	unsigned int currentCmdIndex;
public:
	void OpenLog();
	void CloseLog();
	void RunSession();
private:
	void AppendBytesDump(std::string);
	std::string GetModeStr();
	
	void InitValues();

	void InitSubOutputPipe();
	void GetSubOutput();
	unsigned long GetPipeBufSize();

	void RunBatch();
	void RunProcess(std::wstring);
public:
	void PrintLogAndConsoleNarrow(std::string);
	void PrintLogAndConsole(std::wstring buf);
	void PrintConsole(std::wstring buf);
private:


	void Write(HANDLE hOut, std::string buf);
	void Read(HANDLE hIn, std::string& buf);




	void GetFileHandle(std::wstring, DWORD, HANDLE*, bool = false, DWORD = NULL, DWORD = GENERIC_READ | GENERIC_WRITE);
	

	void AddActiveHandle(HANDLE hActive);
	void RemoveActiveHandle(HANDLE hActive);

	void Error(std::string function, unsigned long exit_code = ERR_DEFAULT);
	void ErrorWithCode(std::string function, unsigned long external_code, unsigned long exit_code = ERR_DEFAULT);


	void CloseProperHandle(HANDLE hHandle);
};

#endif

