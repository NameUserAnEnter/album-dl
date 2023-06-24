#ifndef VALIDATE_H
#define VALIDATE_H

#include "utils.h"

inline bool validField(std::wstring data)
{
    for (int i = 0; i < data.size(); i++)
    {
        if ((unsigned char)data[i] >= (unsigned char)32 && (unsigned char)data[i] <= (unsigned char)126)
        {
            return true;
        }
    }
    return false;
}

inline bool validField(std::wstring data, std::wstring mandatoryFragment)
{
    if (!validField(data)) return false;
    if (!findInStr(data, mandatoryFragment)) return false;
    return true;
}

inline bool validField(std::wstring data, std::vector<std::wstring> allPossibleMandatoryFragments)
{
    if (!validField(data)) return false;
    for (int i = 0; i < allPossibleMandatoryFragments.size(); i++)
    {
        if (findInStr(data, allPossibleMandatoryFragments[i])) return true;
    }
    return false;
}

#endif


