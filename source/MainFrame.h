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
    TextBox fArtist;
    TextBox fAlbumName;
    TextBox fAlbumYear;
    TextBox fURL;
    TextBox fArtworkURL;
    
    wxButton bnRunScript;
    wxCheckBox checkAlert;
    DropDown selectBitrate;

    TextBox fOutput;

private:
    unsigned int ClientWidth, ClientHeight;

    wxPoint defaultPos;

    std::vector<Field> fields;


private:
    std::thread workingThread;
    std::thread outputThread;
    bool bDone;
    bool bResetFields;
    const bool bLog = true;
    unsigned int uMaxOutputLines;

    std::wstring consoleLogFilepath;


    std::string thumbnailURL;
    std::string resourceFilename;

    std::string tracksFilename;


private:
    void OnButtonPress(wxCommandEvent& event);

    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);
private:
    void InitValues();
    void InitFields();
    void InitConsole();
    void InitThemes();
    void InitFonts();
    void InitBindings();
    void InitControls();
    void InitClientSize();
    void InitTestValues();

    void SizeFields(wxSize, wxSize, wxSize);


    void SaveSettings();
    void OpenSettings();
private:
    void GetAlbum();

    void ExecuteBatchSession(bool = true);
    void UpdateOutput();

    void LoadTrackTitles();
    void ResetTracksFile();

    void AttachArtwork(std::wstring, std::wstring);

    void ValidateFilesystemString(std::wstring& str);
    void ValidateTrackTitles();

    bool ValidateFields();

    void DisableFields();
    void EnableFields();

    void PrintTracks();
private:
    std::wstring URL;
    std::wstring artworkURL;

    std::wstring albumsDirectory;
    std::wstring workingDirectory;

    std::wstring converterExec;
    std::wstring downloaderExec;
    std::wstring configName;

    std::wstring artist;
    std::wstring albumName;
    std::wstring albumYear;

    std::vector<std::wstring> trackTitles;
    std::wstring artworkFilename;

    std::wstring albumsDirBackslashes;
    std::wstring albumPathBackslashes;
    std::wstring workingDirBackslashes;

    std::wstring initialOutput;
private:
    std::wstring DownloadStage();
    std::vector<std::wstring> ConvertStage();
    std::wstring CreateTrashDirStage();
    std::vector<std::wstring> RemoveLeftoverStage();

    std::wstring GetTitlesStage();

    std::vector<std::wstring> RenameFilesStage(std::wstring ext = L".mp3");

    void GetArtworkPre();
    void GetArtworkPost();
    std::wstring GetArtworkStage();

    void AttachArtworkToAll();

    std::wstring CreateAlbumDirectoryStage();
    std::wstring MoveAudioStage();
    std::wstring MoveArtworkStage();
};



#endif

