#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <wx/wx.h>
#pragma warning(disable: 4996)


static SOCKET clientSocket;

class IObserver {
public:
    virtual ~IObserver() {};
    virtual void update(const std::string& lastMsgFromSubject) = 0;
};

class IObservable {
public:
    virtual ~IObservable() {};

    virtual void addObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    virtual void notify(const std::string& lastMsg) = 0;
};

class Client: public IObservable
{
public:
    Client();
    ~Client();

    void bindSocket(const char* ip, u_short port);
    void sendingMsg(std::string msg);

    void addObserver(IObserver* observer) override {
        observers.push_back(observer);
    }
    void removeObserver(IObserver* observer) override {
        observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
    }
    void notify(const std::string& lastMsg) override {
        for (auto observer : observers) {
            observer->update(lastMsg);
        }
    }

private:
    std::vector<IObserver*> observers;
    SOCKADDR_IN addr;
};

class Field : public IObserver
{
public:
    Field(IObservable* obj, wxSizer* sizerIntoScroll, wxScrolled<wxPanel>* textPanel) : client(obj), sizerIntoScroll(*sizerIntoScroll), textPanel(*textPanel) {
        client->addObserver(this);
    };
    
    void update(const std::string& lastMsgFromSubject) {
        if (lastMsgFromSubject == "") {
            return;
        }

        wxFont textFont(wxFontInfo(wxSize(0, 16)));
        auto textInScroll = new wxStaticText(&textPanel, wxID_ANY, lastMsgFromSubject, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_WORDWRAP);
        textInScroll->SetFont(textFont);
        textInScroll->SetBackgroundColour(wxColor(225, 225, 225));
        textInScroll->Wrap((&textPanel)->GetClientSize().x);

        (&sizerIntoScroll)->Add(textInScroll, 1, wxALIGN_LEFT | wxALL);
        (&sizerIntoScroll)->Layout();
        (&sizerIntoScroll)->FitInside(&textPanel);
    };

private:
    IObservable* client;

    wxSizer& sizerIntoScroll;
    wxScrolled<wxPanel>& textPanel;
};