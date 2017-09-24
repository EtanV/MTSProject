#include "TSProjectApp.h"
#include "ATSProjectFrm.h"

IMPLEMENT_APP(ATSProjectFrmApp)

bool ATSProjectFrmApp::OnInit()
{
    ATSProjectFrm* frame = new ATSProjectFrm(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int ATSProjectFrmApp::OnExit()
{
    return 0;
}
