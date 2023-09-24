#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "TextBox.h"
#include "validate.h"
#include "encoding.h"
#include "utils.h"
#include "Console.h"
#include "info.h"
#include "DropDown.h"
#include <thread>
#include <mutex>
#include <chrono>

struct Field
{
    wxPoint pos;
    wxSize size;

    Field(int x, int y, wxSize _size)
    {
        pos = wxPoint(x, y);
        size = _size;
    }

    Field()
    {
        pos = wxPoint(0, 0);
        size = wxSize(0, 0);
    }
};

struct Margin
{
    unsigned int top, left, bottom, right;
};

class MainFrame : public wxFrame
{
public:
    MainFrame();
    ~MainFrame();

    Console mainConsole;
    void StartOutputUpdate();
private:
    wxPanel mainPanel;

    TextBox fAlbumsDir;
    TextBox fWorkingDir;
    TextBox fConverterDir;
    TextBox fArtist;
    TextBox fAlbumName;
    TextBox fAlbumYear;
    TextBox fURL;
    TextBox fArtworkURL;
    
    wxButton bnRunScript;
    wxButton bnUpdateDownloader;
    wxCheckBox checkAlert;
    DropDown fBitrate;

    TextBox fOutput;

private:
    unsigned int FullWidth, FullHeight;
    unsigned int taskbarHeight;

    wxPoint defaultPos;

    std::vector<Field> fields;
    unsigned int horizontalMax;
    unsigned int verticalMax;
    unsigned int horizontalMaxDistance;
    unsigned int verticalMaxDistance;

    Margin clientMargin;


private:
    std::thread workingThread;
    std::thread outputThread;
    bool bDone;
    bool bResetFields;
    const bool bLog = true;
    unsigned int uMaxOutputLines;

private:
    // -- INIT METHODS - CALLED BY CONSTRUCTOR
    void InitValues();
    void InitMenuAndStatusBar();

    void InitFieldsSize();
    void InitFieldsAdjustment();
    void InitFields();

    void InitBindings();

    void InitConsole();

    void InitThemes();
    void InitFonts();

    void InitTestValues();
    void InitBitrates();

    void InitWindowSize();
    void InitPosition();
    void InitSettings();
    
    void InitDimensionsInfo();
    void InitVerifyExecutables();
    void InitTerminalOutput();

    void InitSizers();
    void InitFocus();
    // --





    void FindMaxDistanceFields();
    void SetFullSize();


    void SaveSettings();
    void OpenSettings();
private:
    // -- EVENT METHODS
    void OnButtonGet(wxCommandEvent& event);
    void OnButtonUpdate(wxCommandEvent& event);

    void OnPanelResize(wxSizeEvent& event);

    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);
    // --
private:
    void GetAlbum();
    void UpdateDownloader();

    void ExecuteBatchSession(bool = true);
    void UpdateOutput();

    void LoadTrackTitles();
    void ResetTracksFile();

    void AttachArtwork(std::wstring, std::wstring);

    void ValidateFilenameStr(std::wstring&);
    void ValidateTrackTitles();
    bool VerifyFile(std::wstring, std::wstring);

    bool ValidateFields();
    bool ValidateFieldsUpdate();

    void DisableFields();
    void EnableFields();

    void PrintTracks();
private:
    std::wstring URL;
    std::wstring artworkURL;

    std::wstring albumsDirectory;
    std::wstring workingDirectory;
    std::wstring converterDirectory;

    std::wstring converterExec;
    std::wstring downloaderExec;
    std::wstring configName;

    unsigned int bitrate;


    std::wstring artist;
    std::wstring albumName;
    std::wstring albumYear;

    std::vector<std::wstring> trackTitles;
    std::wstring artworkFilename;
    std::wstring artworkBrokenFilename;

    std::wstring trashFoldername;

    std::wstring thumbnailURL;
    std::wstring playlistPageFilename;
    std::wstring playlistArtPageFilename;

    std::wstring tracksFilename;
    
    std::wstring albumsDirBackslashes;
    std::wstring workingDirBackslashes;
    
    std::wstring albumPathBackslashes;

    std::wstring consoleLogFilepath;

    std::wstring settingsFilename;
    std::wstring defaultSettings;

    std::wstring initialOutput;
    std::wstring dimensionsInfo;
    std::wstring execsInfo;
private:
    std::wstring DownloadStage();
    std::vector<std::wstring> ConvertStage();
    std::wstring CreateTrashDirStage();
    std::vector<std::wstring> RemoveLeftoverStage();

    std::wstring GetTitlesStageAlt();

    std::vector<std::wstring> RenameFilesStage(std::wstring ext = L".mp3");

    void GetArtworkStage();
    std::wstring GetArtworkStageAlt();

    void GetTrackTitles();

    void AttachArtworkToAll();

    std::wstring CreateAlbumDirectoryStage();
    std::wstring MoveAudioStage();
    std::wstring MoveArtworkStage();
};



#endif

