#ifndef VALIDATE_H
#define VALIDATE_H

#include "utils.h"

inline bool validField(std::wstring data)
{
    if (data.empty()) return false;

    for (auto currentChar : data)
    {
        if (!iswprint(currentChar))
        {
            return false;
        }
    }
    return true;
}

inline bool validField(std::wstring data, std::wstring beginningWith)
{
    if (!validField(data)) return false;
    if (!beginWith(data, beginningWith)) return false;
    return true;
}

inline bool validField(std::wstring data, std::vector<std::wstring> allPossibleBeginnings)
{
    if (!validField(data)) return false;
    for (int i = 0; i < allPossibleBeginnings.size(); i++)
    {
        if (beginWith(data, allPossibleBeginnings[i])) return true;
    }
    return false;
}

inline bool validDir(std::wstring& dir)
{
    if (!dir.empty())
    {
        if (!validField(dir)) return false;
    }

    for (int i = 0; i < dir.size(); i++)
    {
        if (dir[i] == ':' && i == 1)
        {
            if (!((dir[0] >= 'a' && dir[0] <= 'z') || (dir[0] >= 'A' && dir[0] <= 'Z')))
            {
                return false;
            }
        }
        else if (dir[i] == '\\') dir[i] = '/';
        else if (dir[i] == ':' ||
                 dir[i] == '*' ||
                 dir[i] == '?' ||
                 dir[i] == '"' ||
                 dir[i] == '<' ||
                 dir[i] == '>' ||
                 dir[i] == '|')  // \ : * ? " < > |
        {
            return false;
        }

        if (i == dir.size() - 1 && dir[i] != '/')
        {
            dir += '/';
        }
    }
    return true;
}

#endif


