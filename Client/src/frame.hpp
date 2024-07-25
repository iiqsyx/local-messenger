#include <wx/wx.h>
#include <string>
#include "Client.hpp"

class SMSFrame : public wxFrame {
public:
    SMSFrame(wxString ip, u_short port);

    wxPanel* mainPanel;
    wxScrolled<wxPanel>*textPanel;
    wxPanel* insertPanel;
    
    wxBoxSizer*sizerInAllWindow;
    wxSizer* sizerIntoScroll;
    wxSizer* sizerIntoInsert;

    wxTextCtrl* msgToSend;
    wxFont textFont;
    
private:
    Client* client;
    Field* field;

    void ClientHandler();
    void OnSendEntered(wxCommandEvent& event);
};

class ConnectionFrame : public wxFrame
{
public:
    ConnectionFrame();

    wxPanel* mainPanel;
    wxStaticText* idText;
    wxStaticText* portText;

    wxTextCtrl* idCtrl;
    wxTextCtrl* portCtrl;

    wxButton* connectButton;
    wxButton* disconnectButton;

    wxString ip;
    u_short port;
private:
    void OnConnect(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
};