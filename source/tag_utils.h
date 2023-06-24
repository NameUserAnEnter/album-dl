#include "utils.h"


inline unsigned int fromSynchsafe(unsigned char synchsafe[4])
{
    unsigned int return_value = 0;
    for (int i = 0; i < 4; i++)
    {
        return_value += (synchsafe[4 - 1 - i] << 7 * i);
    }
    return return_value;
}

inline void toSynchsafe(unsigned int data, unsigned char* ptr)
{
    for (int i = 0; i < 4; i++)
    {
        ptr[4 - 1 - i] = (data >> 7 * i) & 127;
    }
}







inline int FindTheTag(const wchar_t* filename, unsigned int* returnTagStartIndex = nullptr, unsigned int* returnSizeMinusHeader = nullptr,
               std::string* returnTag = nullptr)
{
    std::string fileData;
    if (GetFileData(filename, &fileData)) return 1;


    bool bFound = false;
    unsigned int tagStartIndex;

    std::string tagHeader = "ID3";
    unsigned int tagHeaderIndex = 0;
    for (int i = 0; i < fileData.size(); i++)
    {
        if (!bFound)
        {
            if (fileData[i] == tagHeader[tagHeaderIndex])
            {
                if (tagHeaderIndex == 0)
                {
                    tagStartIndex = i;
                }

                tagHeaderIndex++;
                if (tagHeaderIndex == tagHeader.size())
                {
                    bFound = true;
                }
            }
            else
            {
                tagHeaderIndex = 0;
            }
        }
        else
        {
            tagHeader += fileData[i];
            if (tagHeader.size() >= 10) break;
        }
    }


    if (!bFound)
    {
        PrintConsole("Failed to find the ID3 tag.\n");
        return 1;
    }

    PrintConsole("Found the ID3 tag at:\t" + tagStartIndex + '\n');
    PrintConsole("Header (in hex):\t");
    for (int i = 0; i < 10; i++)
    {
        if (i != 0) PrintConsole(" ");
        PrintConsole(HexToStr((unsigned char)tagHeader[i]));
    }

    unsigned char tagSizeMinusHeaderSynchsafe[4] ={ 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        tagSizeMinusHeaderSynchsafe[4 - 1 - i] = tagHeader[10 - 1 - i];
    }
    unsigned int tagSizeMinusHeader = fromSynchsafe(tagSizeMinusHeaderSynchsafe);
    PrintConsole("\ntagSize (in dec):\t" + std::to_string(tagSizeMinusHeader) + "\n\n");



    std::string tag = "";
    for (int i = 0; i < tagHeader.size() + tagSizeMinusHeader; i++)
    {
        tag += fileData[tagStartIndex + i];
    }

    if (returnTagStartIndex != nullptr) *returnTagStartIndex = tagStartIndex;
    if (returnSizeMinusHeader != nullptr) *returnSizeMinusHeader = tagSizeMinusHeader;

    if (returnTag != nullptr) *returnTag = tag;
    return 0;
}


inline int UnsynchroData(std::string data, std::string* returnData)
{
    std::string newData = "";
    for (int i = 0; i < data.size(); i++)
    {
        newData += data[i];
        if ((unsigned char)data[i] == (unsigned char)0xFF)
        {
            if (i + 1 < data.size())
            {
                if ((data[i + 1] & 224) == 224 || (unsigned char)data[i + 1] == (unsigned char)0x00) newData += '\0';
            }
        }
    }

    *returnData = newData;
    return 0;
}

inline int ConstructPictureFrame(const wchar_t* filename, std::string* returnFrame,
                          std::string* returnFrameHeader = nullptr,
                          std::string* returnInQuestionData = nullptr,
                          std::string* returnPictureData = nullptr)
{
    // Constructing the APIC frame
    // frame = frameHeader + inQuestionData + pictureDataUnsynchronised;

    // 1) pictureData
    std::string pictureData = "";
    if (GetFileData(filename, &pictureData)) return 1;

    std::string pictureDataUnsynchronised;
    UnsynchroData(pictureData, &pictureDataUnsynchronised);



    // 2) inQuestionData
    std::string inQuestionData = "";



    std::string MIME_type = "image/png";	// the MIME media type

    unsigned char pictureType = 0x03; // stands for front cover

    unsigned char frameDataSizeSynchsafe[4];
    toSynchsafe(pictureData.size() + ((1) + (MIME_type.size() + 1) + (1 + 1)), frameDataSizeSynchsafe);		// raw picture data size in synchsafe 32-bit int


    // Write fields to the data and append null-termination to each field
    for (int i = 0; i < 4; i++)
    {
        inQuestionData += (unsigned char)frameDataSizeSynchsafe[i];
    }
    inQuestionData += '\0';

    for (int i = 0; i < MIME_type.size(); i++)
    {
        inQuestionData += (unsigned char)MIME_type[i];
    }
    inQuestionData += '\0';

    inQuestionData += pictureType;
    inQuestionData += '\0';



    // 3) frameHeader
    std::string frameHeader = "";
    frameHeader += "APIC";
    for (int i = 0; i < 4; i++)
    {
        frameHeader += (unsigned char)0x00;
    }
    frameHeader += (unsigned char)0x00;
    frameHeader += (unsigned char)0x03;	// (n, p) frame header format flags
    //frameHeader += (unsigned char)0x00;	// test for no flags

    unsigned int frameSize = frameHeader.size() + inQuestionData.size() + pictureDataUnsynchronised.size();

    // Frame header requires the (frameSize - frameHeaderSize) in the header
    // Check if the size does not exceed the 28-bit slot
    if (frameSize - frameHeader.size() > power(2, 28) - 1)
    {
        PrintConsole("Frame size exceeds the limit.\n");
        return 1;
    }

    unsigned char frameSizeSynchsafeMinusHeader[4] ={ 0, 0, 0, 0 };	// needed for frame header
    toSynchsafe(frameSize - frameHeader.size(), frameSizeSynchsafeMinusHeader);
    for (int i = 0; i < 4; i++)
    {
        frameHeader[4 + i] = frameSizeSynchsafeMinusHeader[i];
    }






    std::string frame = "";
    frame += frameHeader + inQuestionData + pictureDataUnsynchronised;

    // Output for testing
    if (returnFrameHeader != nullptr) *returnFrameHeader = frameHeader;
    if (returnInQuestionData != nullptr) *returnInQuestionData = inQuestionData;
    if (returnPictureData != nullptr) *returnPictureData = pictureDataUnsynchronised;


    *returnFrame = frame;
    return 0;
}



inline int AppendFrameToTag(std::string tag, std::string frame, std::string* returnTag)
{
    // Finding the padding start
    bool bPaddingFound = false;
    unsigned int paddingStart;

    unsigned int tagHeaderOffset = 10;
    unsigned int frameHeaderOffset = 10;
    unsigned int frameHeaderID_Offset = 4;
    unsigned int frameHeaderSizeOffset = 4;

    unsigned int index = tagHeaderOffset + frameHeaderID_Offset + frameHeaderSizeOffset;
    while (index < tag.size())
    {
        PrintConsole("index: " + std::to_string(index) + "\n");
        unsigned char frameSizeMinusHeaderSynchsafe[4] ={ tag[index - 4], tag[index - 3], tag[index - 2], tag[index - 1] };
        unsigned int frameSizeMinusHeader = fromSynchsafe(frameSizeMinusHeaderSynchsafe);

        index -= frameHeaderSizeOffset;
        PrintConsole("index: " + std::to_string(index) + "\n");
        index -= frameHeaderID_Offset;
        PrintConsole("index: " + std::to_string(index) + "\n");

        index += frameHeaderOffset;
        PrintConsole("index: " + std::to_string(index) + "\n");
        index += frameSizeMinusHeader;
        PrintConsole("index: " + std::to_string(index) + "\n");
        // To this point frame size is assumed to not exceed the tag size
        // check this just in case:
        if (index >= tag.size()) break;

        // Next frame or padding
        bool bPadding = true;
        for (int i = index; index + i < tag.size(); i++)
        {
            if (tag[index + i] != '\0')
            {
                bPadding = false;
                break;
            }
        }
        if (!bPadding)
        {
            index += frameHeaderID_Offset;
            index += frameHeaderSizeOffset;
        }
        else
        {
            bPaddingFound = true;
            paddingStart = index;
            break;
        }
        PrintConsole("\n\n");
    }

    // Add only the old tag data without padding
    std::string newTag = "";

    if (bPaddingFound)
    {
        PrintConsole("Padding found!\n");
        PrintConsole("Tag without padding:\n");
        for (int i = 0; i < paddingStart; i++)
        {
            newTag += tag[i];

            PrintConsole(HexToStr((unsigned char)tag[i]));
            if ((i + 1) % 16 == 0) PrintConsole("\n");
            else if (i != paddingStart - 1) PrintConsole(" ");
            else PrintConsole("\n");
        }
    }
    else
    {
        PrintConsole("Padding not found!\n");
        PrintConsole("Tag:\n");
        for (int i = 0; i < tag.size(); i++)
        {
            newTag += tag[i];

            PrintConsole(HexToStr((unsigned char)tag[i]));
            if ((i + 1) % 16 == 0) PrintConsole("\n");
            else if (i != tag.size() - 1) PrintConsole(" ");
            else PrintConsole("\n");
        }
    }

    // Append the new frame
    for (int i = 0; i < frame.size(); i++)
    {
        newTag += frame[i];
    }

    // Add 2048 bytes of padding
    for (int i = 0; i < 2048; i++)
    {
        newTag += '\0';
    }

    // Change the tag size value in header
    unsigned int newTagSizeMinusHeader = newTag.size() - 10;
    unsigned char newTagSizeMinusHeaderSynchsafe[4];
    toSynchsafe(newTagSizeMinusHeader, newTagSizeMinusHeaderSynchsafe);
    // 012 34 5 6789
    for (int i = 6; i < 10 && i < newTag.size(); i++)
    {
        newTag[i] = newTagSizeMinusHeaderSynchsafe[i - 6];
    }

    *returnTag = newTag;
    return 0;
}

inline int UpdateTagSize(std::string tag, std::string* returnTag)
{
    if (tag.size() < 10) return 1;
    unsigned int newSizeMinusHeader = tag.size() - 10;
    unsigned char newSizeMinusHeaderSynchsafe[4];
    toSynchsafe(newSizeMinusHeader, newSizeMinusHeaderSynchsafe);

    for (int i = 0; i < 4 && 6 + i < tag.size(); i++)
    {
        tag[6 + i] = newSizeMinusHeaderSynchsafe[i];
    }

    *returnTag = tag;
    return 0;
}

inline int ReplaceTag(const wchar_t* filename, std::string tag, const wchar_t* outputFilename = nullptr)
{
    std::string fileData;
    if (GetFileData(filename, &fileData)) return 1;

    std::string newFileData = "";

    unsigned int oldTagStartIndex;
    std::string oldTag;
    FindTheTag(filename, &oldTagStartIndex, nullptr, &oldTag);
    for (int i = 0; i < oldTagStartIndex; i++)
    {
        newFileData += fileData[i];
    }

    for (int i = 0; i < tag.size(); i++)
    {
        newFileData += tag[i];
    }

    for (int i = oldTagStartIndex + oldTag.size(); i < fileData.size(); i++)
    {
        newFileData += fileData[i];
    }


    if (outputFilename != nullptr) WriteDataToFile(newFileData, outputFilename);
    else WriteDataToFile(newFileData, filename);

    return 0;
}

inline int writeArtwork(const wchar_t* audioFilename, const wchar_t* artworkFilename, const wchar_t* outputAudioFilename = nullptr)
{
    unsigned int oldTagSizeMinusHeader, foundAtIndex;
    std::string tag;
    FindTheTag(audioFilename, &foundAtIndex, &oldTagSizeMinusHeader, &tag);




    std::string newFrame;

    std::string frameHeader;
    std::string inQuestionData;
    std::string pictureData;

    if (ConstructPictureFrame(artworkFilename, &newFrame, &frameHeader, &inQuestionData, &pictureData)) return 1;



    unsigned int newTagSizeMinusHeader = oldTagSizeMinusHeader + newFrame.size();
    unsigned char newTagSizeMinusHeaderSynchsafe[4] ={ 0, 0, 0, 0 };
    toSynchsafe(newTagSizeMinusHeader, newTagSizeMinusHeaderSynchsafe);
    PrintConsole("newTagSize (synchsafe):\n\t\t\t");
    for (int i = 0; i < 4; i++)
    {
        if (i != 0) PrintConsole(" ");
        PrintConsole(HexToStr(newTagSizeMinusHeaderSynchsafe[i]));
    }
    PrintConsole("\n\n");

    PrintConsole("newTagSize (in dec):\t" + std::to_string(newTagSizeMinusHeader) + "\n");
    PrintConsole("oldTagSize (in dec):\t" + std::to_string(oldTagSizeMinusHeader) + "\n");
    PrintConsole(" frameSize (in dec):\t" + std::to_string(newFrame.size()) + "\n");
    PrintConsole("\n");

    PrintConsole(" frameSize (in dec):\t" + std::to_string(newFrame.size()) + "\n");
    PrintConsole("     Check (in dec):\t" + std::to_string(frameHeader.size() + inQuestionData.size() + pictureData.size()) + "\n");
    PrintConsole("         - (in dec):\t" + std::to_string(frameHeader.size()) + "\n");
    PrintConsole("         - (in dec):\t" + std::to_string(inQuestionData.size()) + "\n");
    PrintConsole("         - (in dec):\t" + std::to_string(pictureData.size()) + "\n\n");


    PrintConsole("Tag:\n");
    for (int i = 0; i < tag.size(); i++)
    {
        PrintConsole(HexToStr((unsigned char)tag[i]));
        if ((i + 1) % 16 == 0) PrintConsole("\n");
        else if (i != tag.size() - 1) PrintConsole(" ");
        else PrintConsole("\n");
    }

    std::string newTag;
    AppendFrameToTag(tag, newFrame, &newTag);

    // switch the unsynchro flag:
    unsigned int flagIndex = 3 + 2;
    if (flagIndex < newTag.size()) newTag[flagIndex] += 0x80;
    else
    {
        PrintConsole("Failed to switch the unsynchro tag flag.\n");
        return 1;
    }
    PrintConsole("Flag & Size test:\n");
    for (int i = 0; i < newTag.size(); i++)
    {
        if (i == 0x40)
        {
            break;
        }

        if (i == 0 || i % 16 == 0) PrintConsole(HexToStr(i, 8) + ": ");
        PrintConsole(HexToStr((unsigned char)newTag[i]));
        if ((i + 1) % 16 == 0) PrintConsole("\n");
        else if (i != newTag.size() - 1) PrintConsole(" ");
        else PrintConsole("\n");
    }
    PrintConsole("\n");


    if (outputAudioFilename == nullptr) outputAudioFilename = audioFilename;
    ReplaceTag(audioFilename, newTag, outputAudioFilename);
    PrintConsole("\nDone!\n");


    std::string outputCheck;
    if (GetFileData(outputAudioFilename, &outputCheck)) return 1;
    for (int i = 0; i < outputCheck.size(); i++)
    {
        if (i == 0x40)
        {
            break;
        }

        if (i == 0 || i % 16 == 0) PrintConsole(HexToStr(i, 8) + ": ");
        PrintConsole(HexToStr((unsigned char)outputCheck[i]));
        if ((i + 1) % 16 == 0) PrintConsole("\n");
        else if (i != outputCheck.size() - 1) PrintConsole(" ");
        else PrintConsole("\n");
    }

    return 0;
}


inline void printHex(std::string data, std::string stopQuery)
{
    bool bStop = false;
    for (int i = 0; i < data.size(); i++)
    {
        if (i == 0 || i % 16 == 0) PrintConsole(HexToStr(i, 8) + ": ");
        PrintConsole(HexToStr((unsigned char)data[i]));
        if ((i + 1) % 16 == 0)
        {
            PrintConsole("\n");
            if (bStop)
            {
                //PrintConsole("Found: ");
                //for (int j = 0; j < stopQuery.size(); j++) PrintConsole(HexToStr((unsigned char)stopQuery[j]));
                //PrintConsole("\n");
                while (getc(stdin) != 'x') { }
                //PrintConsole("\n");
                bStop = false;
                //break;
            }
        }
        else if (i != data.size() - 1) PrintConsole(" ");
        else PrintConsole("\n");


        for (int j = 0; j < stopQuery.size() && i + j < data.size(); j++)
        {
            if ((unsigned char)data[i + j] != (unsigned char)stopQuery[j])
            {
                break;
            }

            if (j == stopQuery.size() - 1) bStop = true;
        }
    }
}

