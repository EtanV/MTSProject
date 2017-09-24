#ifndef __ATSPROJECTFRMApp_h__
#define __ATSPROJECTFRMApp_h__

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#else
    #include <wx/wxprec.h>
#endif

class ATSProjectFrmApp : public wxApp
{
    public:
    bool OnInit();
    int OnExit();
};

#endif
