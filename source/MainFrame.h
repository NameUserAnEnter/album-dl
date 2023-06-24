#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "TextBox.h"
#include "validate.h"
#include "utils.h"
#include "network_utils.h"
#include "tag_utils.h"
#include "Console.h"
#include <thread>


class MainFrame : public wxFrame
{
public:
    MainFrame();
private:
    TextBox* albumsDir_Field;
    TextBox* workingDir_Field;
    TextBox* artist_Field;
    TextBox* albumName_Field;
    TextBox* albumYear_Field;
    TextBox* output_Field;

    TextBox* URL_Field;
    TextBox* URL_Artwork_Field;

    wxCheckBox* checkAlert;

public:
    // FROM GLOBAL VARIABLES
    unsigned int ClientWidth, ClientHeight;
private:
    wxSize mainOffset;
    wxSize fieldBetweenSpace;

    wxSize TextBoxSize;
    wxSize LargeBoxSize;
    wxSize ButtonSize;

    RECT labelOffset;


private:
    std::thread consoleThread;

private:
    void OnButtonPress(wxCommandEvent& event);

    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);
public:
    void SaveSettings();
    void OpenSettings();
private:
    void LoadTrackTitles();
    void GetAlbum();
    void ResetTracksFile();

    void AttachArtwork(std::wstring, std::wstring);

    void ValidateFilesystemString(std::wstring& str);
    void ValidateTrackTitles();

    void SetTracksField();
    void CreateTrashDir();
private:
    std::wstring URL;
    std::wstring artworkURL;

    std::wstring albumsDirectory;
    std::wstring workingDirectory;

    std::wstring execName;
    std::wstring configName;

    std::wstring artist;
    std::wstring albumName;
    std::wstring albumYear;

    std::vector<std::wstring> trackTitles;
    std::wstring artworkFilename;

    std::wstring albumsDirBackslashes;
    std::wstring albumPathBackslashes;
    std::wstring workingDirBackslashes;
private:
    void DownloadStage();
    void ConvertStage();
    void RemoveLeftoverStage();

    void GetTitlesStage();

    void RenameFilesStage();

    void GetArtworkStage();
    void AttachArtworkStage();

    void CreateAlbumDirectoryStage();
    void MoveAudioStage();
    void MoveArtworkStage();
};



#endif

