// t3d - Copyright (c) Gustav

// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/docmdi.h>
#include <wx/docview.h>

class MyApp : public wxApp {
 public:
  virtual bool OnInit();
};

class MyFrame : public wxDocMDIParentFrame {
 public:
  explicit MyFrame(const wxString& title);

 private:
  void OnHello(wxCommandEvent& event);  // NOLINT - wx callbacks
  void OnExit(wxCommandEvent& event);   // NOLINT - wx callbacks
  void OnAbout(wxCommandEvent& event);  // NOLINT - wx callbacks
  wxDECLARE_EVENT_TABLE();

 private:
  wxDocManager docManager_;
};

enum {
  ID_Hello = 1
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::OnHello)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
  MyFrame* frame = new MyFrame("Hello World");
  frame->Show(true);
  return true;
}

MyFrame::MyFrame(const wxString& title) : wxDocMDIParentFrame() {
  wxDocMDIParentFrame::Create(&docManager_, NULL, wxID_ANY, title,
                              wxPoint(50, 50), wxSize(450, 340));

  wxMenu* menuFile = new wxMenu;
  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                   "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);
  wxMenu* menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");
  SetMenuBar(menuBar);
  CreateStatusBar();
  SetStatusText("Welcome to wxWidgets!");
}

void MyFrame::OnExit(wxCommandEvent& event) {  // NOLINT - wx callback
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {  // NOLINT - wx callback
  wxMessageBox("This is a wxWidgets' Hello world sample", "About Hello World",
               wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event) {  // NOLINT - wx callback
  wxLogMessage("Hello world from wxWidgets!");
}
