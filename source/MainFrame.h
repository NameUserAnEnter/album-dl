#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "TextBox.h"
#include "validate.h"
#include "utils.h"
#include "Console.h"
#include <thread>
#include <mutex>


class MainFrame : public wxFrame
{
public:
    MainFrame();
    ~MainFrame();
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
    std::mutex consoleSessionMutex;
    bool bDone;

    std::wstring consoleLogFilepath;
    std::wstring consoleOutputBuf;
    Console* mainConsole;

    std::string thumbnailURL;
    std::string resourceFilename;


private:
    void OnButtonPress(wxCommandEvent& event);

    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);
public:
    void InitBindings();
    void InitGUI();
    void InitControls();
    void InitValues();

    void SaveSettings();
    void OpenSettings();
private:
    void GetAlbum();

    void ExecuteBatchSession(bool = true);

    void LoadTrackTitles();
    void ResetTracksFile();

    void AttachArtwork(std::wstring, std::wstring);

    void ValidateFilesystemString(std::wstring& str);
    void ValidateTrackTitles();

    void SetTracksField();
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
    std::wstring DownloadStage();
    std::wstring ConvertStage();
    std::wstring CreateTrashDirStage();
    std::vector<std::wstring> RemoveLeftoverStage();

    std::wstring GetTitlesStage();

    std::vector<std::wstring> RenameFilesStage();

    void GetArtworkPre();
    void GetArtworkPost();
    std::wstring GetArtworkStage();

    void AttachArtworkToAll();

    std::wstring CreateAlbumDirectoryStage();
    std::wstring MoveAudioStage();
    std::wstring MoveArtworkStage();
};



#endif

