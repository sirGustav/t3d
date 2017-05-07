#include "wx/wx.h"
#include "wx/glcanvas.h"

#include "pane.h"
#include "tilebrowser.h"

class App: public wxApp
{
  virtual bool OnInit();

};

IMPLEMENT_APP(App)

class MainFrame : wxFrame {
public:
  MainFrame();

  void OnNewTile(wxCommandEvent& event);

private:
  Pane* pane;
};

bool App::OnInit()
{
  new MainFrame();
  return true;
}

enum EventId {
  ID_NEWTILE = 1
};


MainFrame::MainFrame()
    : wxFrame((wxFrame *)NULL, -1,  wxT("T3D - Tiled 3d Editor"), wxPoint(50,50), wxSize(400,200))
{
  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  pane = new Pane(this);
  sizer->Add(pane, 1, wxEXPAND);

  wxMenuBar* m_pMenuBar = new wxMenuBar();

  Bind(wxEVT_MENU, &MainFrame::OnNewTile, this, ID_NEWTILE);

  // File Menu
  wxMenu* m_pFileMenu = new wxMenu();
  m_pFileMenu->Append(wxID_OPEN, _T("&Open"));
  m_pFileMenu->Append(wxID_SAVE, _T("&Save"));
  m_pFileMenu->AppendSeparator();
  m_pFileMenu->Append(ID_NEWTILE, _T("New &Tile\tCtrl-T"));
  m_pFileMenu->AppendSeparator();
  m_pFileMenu->Append(wxID_EXIT, _T("&Quit"));
  m_pMenuBar->Append(m_pFileMenu, _T("&File"));

  // About menu
  wxMenu* m_pHelpMenu = new wxMenu();
  m_pHelpMenu->Append(wxID_ABOUT, _T("&About"));
  m_pMenuBar->Append(m_pHelpMenu, _T("&Help"));

  SetMenuBar(m_pMenuBar);

  SetSizer(sizer);
  SetAutoLayout(true);

  Show();
}



void MainFrame::OnNewTile(wxCommandEvent &event) {
  TileBrowser dlg(this);
  dlg.ShowModal();
}
