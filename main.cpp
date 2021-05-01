#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
//#include <wx/msw/wx.rc>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/colour.h>
#include <wx/stattext.h>
#include <wx/icon.h>
#include <fstream>
#include <wx/bitmap.h>
#include <wx/statbmp.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/filefn.h> 
#include <wx/msgdlg.h>
#include <wx/file.h>
#include <vector>
#include <set>
#include <string>

#endif

const long wxID_CBOX = wxNewId();
const long wxID_SIGNIN = wxNewId();
const long wxID_STATUS = wxNewId();
using namespace std;
class MineSweeper : public wxApp
{
public:
    virtual bool OnInit();
};

class GameFrame : public wxFrame
{
public:
    GameFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

private:
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnButtonClicked(wxCommandEvent &event);
    void OnToggleFullScreen(wxCommandEvent& WXUNUSED(event));
    void CreateBoard(int n);
    void SetMessageAndResetGame(string msg);
    void BFS(int x, int y);
    int get_adj_count(int x, int y);
    void disable_button(int x, int y, int mine_count);
    void OnResetGame(wxCommandEvent &event);
    
    wxTextCtrl* username;
    wxPanel* panel;
    wxBoxSizer* sizer1;
    wxStaticText* login_status;
    wxStaticText* game_title;
    wxStatusBar* status_bar;
    wxStaticBitmap* backgroundImage;
    wxStaticBitmap* userIconImg;
    wxStaticBitmap* keyIconImg;
    wxFont font;
    wxFont font24;
    wxButton*** buttons;
    wxBitmapButton *exit;
    wxBitmapButton *reset;
    int score;
    int gridSize;
    bool isFirstClick;
    int **isMinesThere;
    bool **visited;
    
    wxDECLARE_EVENT_TABLE();
};

enum
{
    // menu items
    exitID = wxID_EXIT,
    aboutID = wxID_ABOUT,
	wxFSRC  //id for fullscreen
};


wxBEGIN_EVENT_TABLE(GameFrame, wxFrame)

    EVT_MENU(exitID, GameFrame::OnExit)
    EVT_MENU(aboutID, GameFrame::OnAbout)
    EVT_MENU(wxFSRC, GameFrame::OnToggleFullScreen)
    
    wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MineSweeper);

bool MineSweeper::OnInit()
{
    wxInitAllImageHandlers();
    GameFrame *frame = new GameFrame("Mine Sweeper Game", wxDefaultPosition, wxSize(720,700));
    frame->Show(true);
//    frame->Maximize(true);
//    frame->ShowFullScreen(true);
//    frame->SetSize(wxSize(700,700));
   frame->CenterOnScreen(true);
    SetTopWindow(frame);

    return true;
}

GameFrame::GameFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    gridSize = 10;
    isFirstClick = true;
    score;
    wxString bg_path = wxT("./bg-2.jpg");
    bool bg_found=false;

    if(wxFileExists(bg_path)){
        wxImage img(bg_path);
        if (img.IsOk()) {
            backgroundImage = new wxStaticBitmap(this, -1, wxBitmap(img), wxDefaultPosition, wxDefaultSize, 0);
            bg_found = true;
        }
    }

    if(! bg_found){
        this->SetBackgroundColour(wxColor(195, 195, 213));
    }

    wxString exit_icon_path = wxT("./exit.png");
    if(wxFileExists(exit_icon_path)){
        wxImage img(exit_icon_path);

        if (img.IsOk()) {
            exit = new wxBitmapButton(this, -1, wxBitmap(img), wxPoint(600,0), wxDefaultSize, wxBORDER_NONE);
            // exit->SetBackgroundColour(wxColor(250, 250, 250, 0));
            exit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &GameFrame::OnExit, this);
        }
    }

    wxString reset_icon_path = wxT("./reset.png");
    if(wxFileExists(reset_icon_path)){
        wxImage img(reset_icon_path);

        if (img.IsOk()) {
            reset = new wxBitmapButton(this, -1, wxBitmap(img), wxPoint(700,0), wxDefaultSize, wxBORDER_NONE);
            // reset->SetBackgroundColour(wxColor(250, 250, 250, 0));
            reset->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &GameFrame::OnResetGame, this);

        }
            
    }

	panel = new wxPanel(this, -1, wxDefaultPosition, wxSize(700, 700));
	panel->SetBackgroundColour(wxColor(250, 250, 250));

    wxString icon_path = wxT("./icon.png");

    if(wxFileExists(icon_path))
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxBitmap(wxImage(icon_path)));
    	SetIcon(FrameIcon);
    }

    sizer1 = new wxBoxSizer(wxVERTICAL);

	wxPanel *top_margin = new wxPanel(panel, -1, wxDefaultPosition, wxSize(100,-1));
    wxPanel *bottom_margin = new wxPanel(panel, -1, wxDefaultPosition, wxSize(100,-1));

    sizer1->Add(top_margin, 1);
    sizer1->Add(panel, 0, wxALIGN_CENTER);
	sizer1->Add(bottom_margin, 1);

    this->SetSizerAndFit(sizer1);
    this->SetSize(wxSize(900,900));
    
    game_title = new wxStaticText(this, wxID_STATUS, wxString("Welcome to Mine Sweeper!!"), wxDefaultPosition, wxSize(300, 50), wxALIGN_CENTER);


    font = wxFont(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
    font24 = wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);

    game_title->SetFont(font);
    panel->SetBackgroundColour(wxColor(180, 180, 180));
    game_title->SetForegroundColour(wxColor(255,255,255));
    game_title->Show(true);

    CreateBoard(10);

    Center();
    panel->AcceptsFocus();
    panel->SetWindowStyle(wxBORDER_SUNKEN);


    // menu bar
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(aboutID, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(wxFSRC, _T("Toggle Fullscreen\tF11"), _T("Toggle fullscreen display"));
    fileMenu->Append(exitID, _T("E&xit\tAlt-X"), _T("Quit this program"));
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));
    SetMenuBar(menuBar);
    menuBar->Show(false);

    // status bar
    status_bar = new wxStatusBar(this, -1);
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    status_bar->SetFieldsCount(1, __wxStatusBarWidths_1);
    status_bar->SetStatusStyles(1, __wxStatusBarStyles_1);
    SetStatusBar(status_bar);
    status_bar->SetStatusText(wxT("Welcome to Mine Sweeper"));
    status_bar->SetBackgroundColour(wxColor("white"));
    status_bar->Show(false);

}

void GameFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void GameFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("Mine Sweeper",
                 "Game", wxOK | wxICON_INFORMATION);
}

void GameFrame::OnToggleFullScreen(wxCommandEvent& WXUNUSED(event))
{
	ShowFullScreen(!IsFullScreen(),wxFULLSCREEN_NOCAPTION);
}

void GameFrame::CreateBoard(int n){
    buttons = new wxButton**[n];
    for (int i=0;i<n; i++)
        buttons[i] = new wxButton*[n];
    
    isMinesThere = new int*[gridSize];
    visited = new bool*[gridSize];

    for (int i=0; i<gridSize; i++){
        isMinesThere[i] = new int[gridSize];
        visited[i] = new bool[gridSize];
    }

    for (int i=0; i<gridSize; i++){
        for (int j=0; j<gridSize; j++){
            buttons[i][j] = new wxButton(panel, 10000+i*n+j , wxEmptyString, wxPoint(i*70+2, j*70+2), wxSize(65,65));
            buttons[i][j]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &GameFrame::OnButtonClicked, this);
            buttons[i][j]->SetFont(font24);
            buttons[i][j]->SetBackgroundColour(wxColour(200,200,200));
            buttons[i][j]->SetForegroundColour(wxColour(50,50,50));

            isMinesThere[i][j] = 0;
            visited[i][j] = 0;
        }
    }
}

void GameFrame::OnButtonClicked(wxCommandEvent &event){
    int n=gridSize;
    int x = ( event.GetId() -10000 ) / n;
    int y = ( event.GetId() -10000 ) % n;

    if(isFirstClick){
        score = 0;
        int mines = 30;
        while(mines){
            int cx = rand() % n;
            int cy = rand() % n;
            bool is_valid = true;
            // for (int i:{1,0,-1}){
            //     for (int j:{1,0,-1}){
            //         if(x+i==cx && y+j==cy)
            //             is_valid = false;
            //     }
            // }
            if(is_valid && isMinesThere[cx][cy]==0 && ( cx != x || cy != y) ){
                isMinesThere[cx][cy]=-1;
                mines--;
            }
        }
        // score++;
        isFirstClick = false;
        game_title->SetLabel("Score : "+ std::to_string(score) + "/70");
        
    }

    buttons[x][y]->Enable(false);
    visited[x][y] = true;

    if(isMinesThere[x][y]==-1){
        // reset game;
        SetMessageAndResetGame("BOOOM!! - Game Over :(");
    }
    else{
        int mine_count = 0;
        for (int i:{-1, 0, 1}){
            for(int j:{-1,0, 1}){  
                if( x+i<0 || y+j<0 || x+i >= gridSize || y+j >= gridSize )
                    continue;
                else if(isMinesThere[x+i][y+j] == -1 ) {
                    mine_count++;
                }
            }
        }

        if (mine_count > 0 ){
            buttons[x][y]->SetLabel(std::to_string(mine_count));
        }
        // else if(mine_count==0){
        //     BFS(x, y);
        // }

        buttons[x][y]->SetBackgroundColour(wxColour(150,150,150));
        buttons[x][y]->Enable(false);
        score++;
        game_title->SetLabel("Score : "+ std::to_string(score) + "/70");
    }
    if(score == 70 ){
        // reset game;
        SetMessageAndResetGame("Wow!! You Won ;)");
    }

    event.Skip();
}
 
void GameFrame::SetMessageAndResetGame(string msg){
    msg += "\nScore : "+ std::to_string(score)+"/70";
    wxMessageBox(msg);
    for (int x=0; x<gridSize; x++){
        for (int y=0; y<gridSize; y++){
            isMinesThere[x][y] = 0;
            buttons[x][y]->SetLabel("");
            buttons[x][y]->Enable(true);
            buttons[x][y]->SetBackgroundColour(wxColour(200,200,200));
            visited[x][y]=0;
            game_title->SetLabel("Welcome to Mine Sweeper !!!");
            isFirstClick = true;
            score = 0;
        }
    }
}

void GameFrame::BFS(int x, int y){
    set<pair<int,int>> bfs_queue;
    bfs_queue.insert({x, y});
    while(!bfs_queue.empty()){
        auto cord = *bfs_queue.begin();
        x = cord.first;
        y = cord.second;
        int mine_count=0;
        bfs_queue.erase(bfs_queue.begin());

        for (int i:{-1, 0, 1}){
            for(int j:{-1,0, 1}){
                if( x+i<0 || y+j<0 || x+i >= gridSize || y+j >= gridSize )
                    continue;
                else{
                    if(visited[x+i][y+j])
                        continue;
                    mine_count = get_adj_count(x+i, y+j);                
                    disable_button(x+i, y+j, mine_count);
                    if(mine_count==0){
                        bfs_queue.insert({x+i, y+j});
                    }
                }
            }
        }
    }
}


int GameFrame::get_adj_count(int x, int y){
    int mine_count = 0;
    for (int i:{-1, 0, 1}){
        for(int j:{-1,0, 1}){
            if(  x+i<0 || y+j<0 || x+i >= gridSize || y+j >= gridSize )
                continue;
            else if(isMinesThere[x+i][y+j] == -1 ) {
                mine_count++;
            }
        }
    }
    return mine_count;
}


void GameFrame::disable_button(int x, int y, int mine_count){
    if(mine_count > 0)
        buttons[x][y]->SetLabel(std::to_string(mine_count));
    visited[x][y]=1;
    buttons[x][y]->SetBackgroundColour(wxColour(150,150,150));
    buttons[x][y]->Enable(false);
    score++;
    game_title->SetLabel("Score : "+ std::to_string(score) + "/70");

}

void GameFrame::OnResetGame(wxCommandEvent &event){
    for (int x=0; x<gridSize; x++){
        for (int y=0; y<gridSize; y++){
            isMinesThere[x][y] = 0;
            buttons[x][y]->SetLabel("");
            buttons[x][y]->Enable(true);
            buttons[x][y]->SetBackgroundColour(wxColour(200,200,200));
            game_title->SetLabel("Welcome to Mine Sweeper !!!");
            isFirstClick = true;
            visited[x][y] = 0;
            score = 0;
        }
    }    
}