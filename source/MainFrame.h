#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "id_enum.h"
#include "TextBox.h"
#include "validate.h"
#include "encoding.h"
#include "utils.h"
#include "Console.h"
#include "info.h"
#include "DropDown.h"
#include "MainDialog.h"
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

    Field(wxPoint _pos, wxSize _size) : pos(_pos), size(_size)
    {
        //
    }

    Field()
    {
        pos = wxPoint(0, 0);
        size = wxSize(0, 0);
    }
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
    
    wxButton buttonDownload;
    wxButton buttonUpdateYtDlp;
    wxCheckBox checkAlert;
    DropDown fBitrate;

    TextBox fOutput;
private:
    wxPoint defaultPos;

    std::vector<Field> fields;
    
    int fieldHeight;
    int fieldBreakV;
    int fieldBreakH;

    int buttonBreak;

    int verticalCheckBoxOffset;
    int verticalDropDownOffset;

    wxSize minDataFieldSize;
    wxSize maxDataFieldSize;

    wxSize minOutputFieldSize;
    wxSize maxOutputFieldSize;

    wxSize minButtonSize;

    Margin clientMargin;

    wxFont outputFont;
    wxFont secondaryFont;
private:
    std::thread workingThread;
    std::thread outputThread;
    bool bDone;
    bool bResetFields;
    bool bLoadedSettings;
    const bool bLog = true;
    unsigned int uMaxOutputLines;

private:
    // -- INIT METHODS - CALLED BY CONSTRUCTOR
    void InitValues();
    void InitMenuAndStatusBar();

    void InitFieldsDimensions();
    void InitFields();
    void InitFieldsLabels();
    void InitFieldsDimensionRanges();

    void InitBindings();

    void InitConsole();

    void InitThemes();
    void InitFonts();

    void InitTestValues();
    void InitBitrates();

    void InitWindowSize();
    void InitPosition();
    void InitSettings();

    void InitFocus();
    
    void InitDimensionsInfo();
    void InitVerifyExecutables();
    void InitTerminalOutput();
    // --





    wxSize FindMaxDistance(std::vector<Field>, std::vector<TextBox*>, std::vector<DropDown*>, std::vector<wxWindow*>);
    void SetFullSize();


    void SaveSettings();
    void OpenSettings();
private:
    // -- EVENT METHODS
    void OnButtonDownload(wxCommandEvent&);
    void OnButtonUpdate(wxCommandEvent&);

    void OnPanelResize(wxSizeEvent&);

    void OnSave(wxCommandEvent&);
    void OnExit(wxCommandEvent&);

    void OnAbout(wxCommandEvent&);
    void OnLicense(wxCommandEvent&);
    void OnReadMe(wxCommandEvent&);

    void OnClose(wxCloseEvent&);
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
    std::wstring workingDirectoryDefault;

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

    std::wstring playlistPageFilename;
    std::wstring playlistArtPageFilename;

    std::wstring tracksFilename;
    
    std::wstring albumsDirBackslashes;
    std::wstring workingDirBackslashes;
    
    std::wstring albumPathBackslashes;

    std::wstring consoleLogFilepath;

    std::wstring settingsFilename;

    std::wstring initialOutput;
    std::wstring dimensionsInfo;
    //std::wstring execsInfo;
    std::wstring converterExecInfo;
    std::wstring downloaderExecInfo;
private:
    std::wstring DownloadStage();
    std::vector<std::wstring> ConvertStage();
    std::wstring CreateTrashDirStage();
    std::vector<std::wstring> RemoveLeftoverStage();

    std::wstring GetTitlesStageAlt();

    std::vector<std::wstring> RenameFilesStage(std::wstring ext = L".mp3");

    void GetArtworkStage();
    std::wstring GetArtworkStageAlt(std::wstring);

    void GetTrackTitles();

    void AttachArtworkToAll();

    std::wstring CreateAlbumDirectoryStage();
    std::wstring MoveAudioStage();
    std::wstring MoveArtworkStage();
};



#endif

