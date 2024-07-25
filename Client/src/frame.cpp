#include "frame.hpp"
#include <thread>
#include <chrono>
#include <mutex>
#include <wx/wx.h>

enum {
    ID_Connect,
    ID_Disconnect,
    ID_SendMsg,
    ID_TEXTPANEL,
};

std::mutex mutex;

///////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------
Client::Client() {
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void Client::bindSocket(const char* ip, u_short port) {
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    if (connect(clientSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw("ERROR\tbind faild : " + WSAGetLastError());
    }
}

void Client::sendingMsg(std::string msg) {
    int msgSize = msg.size();
    send(clientSocket, (char*)&msgSize, sizeof(int), NULL);
    send(clientSocket, msg.c_str(), msgSize, NULL);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

Client::~Client() {
    closesocket(clientSocket);
}
//-----------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////


ConnectionFrame::ConnectionFrame()
    : wxFrame(nullptr, wxID_ANY, "Network connection", wxDefaultPosition, wxSize(300, 300)) {
    wxFont textFont(wxFontInfo(wxSize(0, 24)));

//---------------------------------------------------------------------------------------------------------
// PANELS
    mainPanel = new wxPanel(this);
    wxPanel* panelTopLeft = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(100,150));
    wxPanel* panelTopRight = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(200, 150));
    wxPanel* panelDown = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(300, 150));


//---------------------------------------------------------------------------------------------------------
//ITEMS
    idText = new wxStaticText(panelTopLeft, wxID_ANY, "IPv4", wxPoint(30, 24), wxSize(60, 40), wxALIGN_RIGHT);
    portText = new wxStaticText(panelTopLeft, wxID_ANY, "Port", wxPoint(30, 94), wxSize(60, 40), wxALIGN_RIGHT);

    idCtrl = new wxTextCtrl(panelTopRight, wxID_ANY, wxEmptyString, wxPoint(0, 20), wxSize(130, 30));
    portCtrl = new wxTextCtrl(panelTopRight, wxID_ANY, wxEmptyString, wxPoint(0, 90), wxSize(130, 30));

    connectButton = new wxButton(panelDown, ID_Connect, "Connect", wxPoint(30, 20), wxSize(240, 40));

//---------------------------------------------------------------------------------------------------------
// SIZERS
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxSizerFlags flags = wxSizerFlags().CenterHorizontal();

    wxSizer* sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(panelTopLeft,1, wxEXPAND);
    sizerTop->Add(panelTopRight,2, wxEXPAND);

    wxSizer* sizerDown = new wxBoxSizer(wxVERTICAL);
    sizerDown->Add(panelDown, 1, wxEXPAND);
    sizer->AddStretchSpacer(1);
    sizer->Add(sizerTop, flags);
    sizer->Add(sizerDown, flags);
    sizer->AddStretchSpacer(1);

    mainPanel->SetSizer(sizer);
    sizer->SetSizeHints(this);

//---------------------------------------------------------------------------------------------------------
// BIND
    Bind(wxEVT_BUTTON, &ConnectionFrame::OnConnect, this, ID_Connect);
}


void ConnectionFrame::OnConnect(wxCommandEvent& event) {
    ip = idCtrl->GetValue();
    port = atoi(portCtrl->GetValue().c_str());

    WSADATA wsaData;
    WORD DLLVersion = MAKEWORD(2, 2);
    if (WSAStartup(DLLVersion, &wsaData)) {
        wxLogError("Faild connection");
        return;
    }

    SMSFrame* secondFrame = new SMSFrame(ip, port);
    this->Destroy();
    secondFrame->Center();
    secondFrame->Show(true);
    secondFrame->SetFocus();
}

void SMSFrame::ClientHandler() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    int msgSize;

    while (true) {
        recv(clientSocket, (char*)&msgSize, sizeof(int), NULL);
        char* msg = new char[msgSize+1];
        msg[msgSize] = '\0';
        recv(clientSocket, msg, msgSize, NULL);
        {
            std::lock_guard g(mutex);
            wxTheApp->CallAfter([this, msg]() {
                client->notify(msg);
                });
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        delete[] msg;
    }
}

SMSFrame::SMSFrame(wxString ip, u_short port)
    : wxFrame(nullptr, wxID_ANY, "Chat", wxDefaultPosition, wxSize(600, 800)){
    client = new Client();
    client->bindSocket(ip.mb_str(), port);
    
    textFont.SetPixelSize(wxSize(0, 16));

//---------------------------------------------------------------------------------------------------------
// PANELS AND SIZERS
    wxPanel* mainPanel = new wxPanel(this);
    textPanel = new wxScrolled<wxPanel>(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(600, 700));
    insertPanel = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(600, 100));

//---------------------------------------------------------------------------------------------------------
// ITEMS
    msgToSend = new wxTextCtrl(insertPanel, ID_SendMsg, wxEmptyString, wxPoint(50, 30), wxSize(500, 40), wxTE_PROCESS_ENTER);
    msgToSend->SetFont(textFont);
//---------------------------------------------------------------------------------------------------------
// SIZERS
    sizerIntoScroll = new wxBoxSizer(wxVERTICAL);
    sizerInAllWindow = new wxBoxSizer(wxVERTICAL);
    sizerIntoInsert = new wxBoxSizer(wxVERTICAL);

    textPanel->SetScrollRate(0, FromDIP(10));
    textPanel->SetSizer(sizerIntoScroll);
    insertPanel->SetSizer(sizerIntoInsert);

    sizerInAllWindow->Add(textPanel, 7, wxEXPAND);
    sizerInAllWindow->Add(insertPanel, 1, wxALIGN_CENTER_HORIZONTAL);

    mainPanel->SetSizer(sizerInAllWindow);
    sizerInAllWindow->SetSizeHints(this);
  
//---------------------------------------------------------------------------------------------------------
    field = new Field(client, sizerIntoScroll, textPanel);
    std::thread recvMsgThread(&SMSFrame::ClientHandler, this);
    recvMsgThread.detach();

    Bind(wxEVT_TEXT_ENTER, &SMSFrame::OnSendEntered, this, ID_SendMsg);
};

void SMSFrame::OnSendEntered(wxCommandEvent& event) {
    wxString textFromField;
    textFromField = msgToSend->GetValue();

    if (textFromField == "") {
        return;
    }

    {
        std::lock_guard g(mutex);
        msgToSend->Clear();

        auto textInScroll = new wxStaticText(textPanel, wxID_ANY, textFromField, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_WORDWRAP);
        textInScroll->SetFont(textFont);
        textInScroll->SetBackgroundColour(wxColor(225, 225, 225));
        textInScroll->Wrap(textPanel->GetClientSize().x);

        sizerIntoScroll->Add(textInScroll, 1, wxALIGN_RIGHT | wxALL, FromDIP(10));
        sizerIntoScroll->Layout();
        sizerIntoScroll->FitInside(textPanel);

        std::string msg = std::string(textFromField.mb_str());
        client->sendingMsg(msg);
    }
};