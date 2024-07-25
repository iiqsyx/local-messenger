#include "app.hpp"
#include "frame.hpp"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    ConnectionFrame* mainFrame = new ConnectionFrame();
    mainFrame->Center();
    mainFrame->Show();

    return true;
};