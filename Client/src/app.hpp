#include <wx/wx.h>

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

DECLARE_APP(MyApp);