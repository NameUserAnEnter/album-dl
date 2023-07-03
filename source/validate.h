#ifndef VALIDATE_H
#define VALIDATE_H

#include "utils.h"

inline bool validField(std::wstring data)
{
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

#endif


