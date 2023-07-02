#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <wininet.h>
#include "utils.h"
#include "Console.h"

namespace net
{
    Console* con = nullptr;

    inline void SetConsole(Console*);
    inline void PrintConsole(std::string text);
    inline void PrintConsole(std::wstring text);
}

inline void net::SetConsole(Console* _con)
{
    net::con = _con;
}

inline void net::PrintConsole(std::string text)
{
    if (net::con != nullptr) net::con->PrintLogAndConsoleNarrow(text);
}

inline void net::PrintConsole(std::wstring text)
{
    if (net::con != nullptr) net::con->PrintLogAndConsole(text);
}


inline int GetResource(const char* host, const char* resource, const char* outputFilename)
{
    using namespace net;

    // Figure out what does the "Retrieves configuration from the registry" mean for INTERNET_OPEN_TYPE_PRECONFIG
    // dwAccessType: INTERNET_OPEN_TYPE_PRECONFIG or INTERNET_OPEN_TYPE_DIRECT
    // dwFlags: INTERNET_FLAG_ASYNC or NULL
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
    if (hInternet == NULL)
    {
        PrintConsole("InternetOpen() failed: " + std::to_string(GetLastError()) + "\n");

        return 1;
    }


    HINTERNET hConnection = InternetConnectA(hInternet, host, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP,
                                             INTERNET_FLAG_SECURE, INTERNET_NO_CALLBACK);
    if (hConnection == NULL)
    {
        PrintConsole("InternetConnection() failed: " + std::to_string(GetLastError()) + "\n");
        InternetCloseHandle(hInternet);

        return 2;
    }


    const char* acceptTypes[] ={ "text/*", "image/avif", "image/webp", "image/apng", "*/*", NULL };
    // Investigate flags
    HINTERNET hRequest = HttpOpenRequestA(hConnection, "GET", resource, "HTTP/1.1", NULL, acceptTypes,
                                          INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_SECURE, INTERNET_NO_CALLBACK);
    if (hRequest == NULL)
    {
        PrintConsole("HttpOpenRequest() failed: " + std::to_string(GetLastError()) + "\n");
        InternetCloseHandle(hConnection);
        InternetCloseHandle(hInternet);

        return 3;
    }

    // lpszHeaders: "host: en.wikipedia.org" or NULL
    BOOL bResult;
    bResult = HttpSendRequestA(hRequest, NULL, 0, NULL, 0);
    if (bResult != TRUE)
    {
        PrintConsole("HttpSendRequest() failed: " + std::to_string(GetLastError()) + "\n");
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnection);
        InternetCloseHandle(hInternet);

        return 4;
    }

    unsigned long totalBytesRead = 0;
    bool bContinue = true;

    FILE* outputFile = nullptr;
    fopen_s(&outputFile, outputFilename, "w");
    if (outputFile == nullptr)
    {
        PrintConsole("Failed to open: " + std::string(outputFilename) + "\n");
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnection);
        InternetCloseHandle(hInternet);

        return 9;
    }
    fclose(outputFile);

    PrintConsole("Bytes available:\n");
    unsigned int countPrint = 0;
    while (bContinue)
    {
        unsigned long bytes = 0;
        bResult = InternetQueryDataAvailable(hRequest, &bytes, NULL, NULL);
        if (bResult != TRUE)
        {
            PrintConsole("InternetQueryDataAvailable() failed: " + std::to_string(GetLastError()) + "\n");
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hInternet);

            return 5;
        }
        if (bytes == 0)
        {
            bContinue = false;
            break;
        }
        PrintConsole(NumToStr(bytes, 10, 4, ' '));

        countPrint++;
        if (countPrint % 16 == 0) PrintConsole("\n");
        else PrintConsole(" ");


        unsigned char* buf = (unsigned char*)calloc(bytes, sizeof(unsigned char));
        if (buf == nullptr)
        {
            PrintConsole("Failed to allocate memory.\n");
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hInternet);

            return 6;
        }

        unsigned long bytesRead;
        bResult = InternetReadFile(hRequest, buf, bytes, &bytesRead);
        if (bResult != TRUE)
        {
            free(buf);
            PrintConsole("InternetReadFile() failed: " + std::to_string(GetLastError()) + "\n");
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hInternet);

            return 7;
        }

        FILE* outputFile = nullptr;
        fopen_s(&outputFile, outputFilename, "a");
        if (outputFile == nullptr)
        {
            free(buf);
            PrintConsole("Failed to open: " + std::string(outputFilename) + "\n");
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnection);
            InternetCloseHandle(hInternet);

            return 8;
        }

        for (int i = 0; i < bytesRead; i++)
        {
            putc(buf[i], outputFile);
        }
        fclose(outputFile);


        totalBytesRead += bytesRead;
        free(buf);
    }


    PrintConsole("\nBytes read: " + std::to_string(totalBytesRead) + '\n');


    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnection);
    InternetCloseHandle(hInternet);
    PrintConsole("Success.\n");

    return 0;
}

inline int GetThumbnailURL(std::string* returnURL, const char* inputFilename)
{
    using namespace net;

    FILE* resourceFile = nullptr;
    fopen_s(&resourceFile, inputFilename, "r");
    if (resourceFile == nullptr)
    {
        PrintConsole("Failed to open: " + std::string(inputFilename) + "\n");
        return 1;
    }

    char currentChar;
    std::string currentWord = "";
    std::string searchQuery[] ={ "PlaylistThumbnail", "\"url\":\"", "maxresdefault.jpg", "\\u0026" };
    unsigned int count = 0;

    unsigned int currentIndex = 0;
    unsigned int queryIndex = 0;
    do
    {
        currentChar = getc(resourceFile);

        if (searchQuery[queryIndex] == "maxresdefault.jpg")
        {
            if (currentChar == '\"')
            {
                currentIndex = 0;
                bool bFound = false;
                std::string testWord = "";
                for (int j = 0; j < currentWord.size(); j++)
                {
                    if (currentWord[j] == searchQuery[queryIndex][currentIndex])
                    {
                        testWord += currentWord[j];
                        if (testWord == searchQuery[queryIndex])
                        {
                            bFound = true;
                            queryIndex++;
                            break;
                        }
                        else
                        {
                            currentIndex++;
                        }
                    }
                    else
                    {
                        testWord = "";
                        currentIndex = 0;
                    }
                }

                if (bFound)
                {
                    break;
                }
                else
                {
                    currentWord = "";
                    currentIndex = 0;
                    queryIndex--;
                }
            }
            else
            {
                currentWord += currentChar;
            }
        }
        else if (currentChar == searchQuery[queryIndex][currentIndex])
        {
            currentWord += currentChar;
            if (currentWord == searchQuery[queryIndex])
            {
                currentWord = "";
                currentIndex = 0;

                queryIndex++;
            }
            else
            {
                currentIndex++;
            }
        }
        else
        {
            currentWord = "";
            currentIndex = 0;
        }
    } while (currentChar != EOF);

    fclose(resourceFile);

    ///*
    std::string target = currentWord;
    currentWord = "";
    currentIndex = 0;
    count = 0;
    std::string targetCorrect = "";
    for (int i = 0; i < target.size(); i++)
    {
        targetCorrect += target[i];
        if (target[i] == searchQuery[queryIndex][currentIndex])
        {
            currentWord += target[i];
            if (currentWord == searchQuery[queryIndex])
            {
                currentWord = "";
                currentIndex = 0;

                count++;

                for (int j = 0; j < searchQuery[queryIndex].size(); j++)
                {
                    targetCorrect.pop_back();
                }
                targetCorrect += '&';
            }
            else
            {
                currentIndex++;
            }
        }
        else
        {
            currentIndex = 0;
            currentWord = "";
        }
    }

    *returnURL = targetCorrect;
    PrintConsole("target:\n" + target + "\n\n");
    PrintConsole("targetCorrect:\n" + targetCorrect + "\n\n");
    PrintConsole("queryIndex: " + std::to_string(queryIndex) + '\n');
    PrintConsole("count: " + std::to_string(count) + '\n');
    //*/

    return 0;
}





#endif





