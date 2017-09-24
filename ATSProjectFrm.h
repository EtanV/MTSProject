#ifndef __ATSPROJECTFRM_H__
#define __ATSPROJECTFRM_H__

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/frame.h>
#else
    #include <wx/wxprec.h>
#endif

#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/filedlg.h>
#include "ATS_XMLVMFExp.h"

#undef ATSProjectFrm_STYLE
#define ATSProjectFrm_STYLE wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX

class ATSProjectFrm : public wxFrame
{
    private:
        DECLARE_EVENT_TABLE();

    public:
        ATSProjectFrm(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Transport Stream Viewer 1.0"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = ATSProjectFrm_STYLE);
        virtual ~ATSProjectFrm();
        void Mnuopen1002Click(wxCommandEvent& event);
        void Mnuopen1003Click(wxCommandEvent& event);
        void Mnuexit1005Click(wxCommandEvent& event);
        void WxTreeCtrl2SelChanged(wxTreeEvent& event);
        void FillListCtrlRecursively(wxString selected, wxTreeItemId Level);
        void VisualizeTSData(TSData *CurrentTSData2);
        void ShowMenu(wxTreeItemId id, const wxPoint& pt);
        void ShowMenuXML_VMF(wxTreeItemId id, const wxPoint& pt);
        void OnItemMenu(wxTreeEvent& event);
        void ExtractItemPES_ES_PCM(wxCommandEvent& event);
        void TreeItemXMLExport(wxCommandEvent& event);
        wxTreeCtrl *WxTreeCtrl2;
        TSData* CurrentTSData1;

    private:

        wxTreeCtrl *WxTreeCtrl1;
        wxListCtrl *WxListCtrl1;
        wxMenuBar *WxMenuBar1;
        wxToolBar *WxToolBar1;
        wxStatusBar *WxStatusBar1;
        wxFileDialog *WxOpenFileDialog1;
        wxFileDialog *WxOpenFileDialog2;

    private:

        enum
        {
            ID_WXTOOLBUTTON2 = 1011,
            ID_WXSTATUSBAR1 = 1010,
            ID_WXTREECTRL1 = 1008,
            ID_WXTOOLBUTTON1 = 1007,
            ID_WXTOOLBAR1 = 1006,
            ID_MNU_FILE_1002 = 1002,
            ID_MNU_OPEN_1003 = 1003,
            ID_MNU_EXIT_1005 = 1005,
            ID_WXLISTCTRL1 = 1015,
            ID_WXTREECTRL2 = 1016,
            ID_WXTREE_PES = 1017,
            ID_WXTREE_ES = 1018,
            ID_WXTREE_PCM = 1019,
            ID_WXTREE_XML_Exp = 1020,
            ID_WXTREE_VMF_Exp = 1021,
            ID_DUMMY_VALUE_ 
        };

    private:
        void OnClose(wxCloseEvent& event);
        void CreateGUIControls();
};

#endif
