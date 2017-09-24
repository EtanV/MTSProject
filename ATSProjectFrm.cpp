#include <wx/ffile.h>
#include "ATSProjectFrm.h"
#include "Images/ATSProjectFrm_WxToolButton1_XPM.xpm"

//event table
BEGIN_EVENT_TABLE(ATSProjectFrm,wxFrame)

    EVT_CLOSE(ATSProjectFrm::OnClose)
    EVT_MENU(ID_WXTOOLBUTTON1,ATSProjectFrm::Mnuopen1003Click)
    EVT_MENU(ID_WXTOOLBUTTON2,ATSProjectFrm::Mnuopen1002Click)
    EVT_MENU(ID_MNU_OPEN_1003, ATSProjectFrm::Mnuopen1003Click)
    EVT_MENU(ID_MNU_EXIT_1005, ATSProjectFrm::Mnuexit1005Click)
    EVT_TREE_SEL_CHANGED(ID_WXTREECTRL2,ATSProjectFrm::WxTreeCtrl2SelChanged)
    EVT_TREE_ITEM_MENU(ID_WXTREECTRL2, ATSProjectFrm::OnItemMenu)
    EVT_MENU(ID_WXTREE_PES,ATSProjectFrm::ExtractItemPES_ES_PCM)
    EVT_MENU(ID_WXTREE_ES,ATSProjectFrm::ExtractItemPES_ES_PCM)
    EVT_MENU(ID_WXTREE_PCM,ATSProjectFrm::ExtractItemPES_ES_PCM)
    EVT_MENU(ID_WXTREE_XML_Exp,ATSProjectFrm::TreeItemXMLExport)

END_EVENT_TABLE()

ATSProjectFrm::ATSProjectFrm(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
    CreateGUIControls();
}

ATSProjectFrm::~ATSProjectFrm()
{
}

void ATSProjectFrm::CreateGUIControls()
{
    WxMenuBar1 = new wxMenuBar();
    wxMenu *ID_MNU_FILE_1002_Mnu_Obj = new wxMenu(0);
    ID_MNU_FILE_1002_Mnu_Obj->Append(ID_MNU_OPEN_1003, wxT("&Open\tCtrl+O"), wxT(""), wxITEM_NORMAL);
    ID_MNU_FILE_1002_Mnu_Obj->AppendSeparator();
    ID_MNU_FILE_1002_Mnu_Obj->Append(ID_MNU_EXIT_1005, wxT("E&xit"), wxT(""), wxITEM_NORMAL);
    WxMenuBar1->Append(ID_MNU_FILE_1002_Mnu_Obj, wxT("&File"));
    SetMenuBar(WxMenuBar1);

    WxToolBar1 = new wxToolBar(this, ID_WXTOOLBAR1, wxPoint(0, 0), wxSize(442, 26));

    wxBitmap WxToolButton1_BITMAP (ATSProjectFrm_WxToolButton1_XPM);
    wxBitmap WxToolButton2_BITMAP (ATSProjectFrm_WxToolButton2_XPM);
    wxBitmap WxToolButton1_DISABLE_BITMAP (wxNullBitmap);
    wxBitmap WxToolButton2_DISABLE_BITMAP (wxNullBitmap);
    WxToolBar1->AddTool(ID_WXTOOLBUTTON1, wxT(""), WxToolButton1_BITMAP, WxToolButton1_DISABLE_BITMAP, wxITEM_NORMAL, wxT("Open a transport stream file"), wxT(""));
        
    WxTreeCtrl1 = new wxTreeCtrl(this, ID_WXTREECTRL1, wxPoint(0, 28), wxSize(226, 443), wxTR_HAS_BUTTONS, wxDefaultValidator, wxT("WxTreeCtrl1"));
    WxTreeCtrl2 = new wxTreeCtrl(this, ID_WXTREECTRL2, wxPoint(0, 28), wxSize(226, 443), wxTR_HAS_BUTTONS, wxDefaultValidator, wxT("WxTreeCtrl1"));
    WxTreeCtrl1->Show(false);
    
    WxListCtrl1 = new wxListCtrl(this, ID_WXLISTCTRL1, wxPoint(226, 28), wxSize(367, 444), wxLC_REPORT, wxDefaultValidator, wxT("WxListCtrl1"));
    WxListCtrl1->InsertColumn(0,wxT("Parameters"),wxLIST_FORMAT_LEFT,220 );
    WxListCtrl1->InsertColumn(1,wxT("Values"),wxLIST_FORMAT_LEFT,143 );

    WxStatusBar1 = new wxStatusBar(this, ID_WXSTATUSBAR1);

    WxOpenFileDialog1 = new wxFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("*.*"), wxOPEN);
    WxOpenFileDialog2 = new wxFileDialog(this, wxT("Give the name of the output file"), wxT(""), wxT(""), wxT("*.*"), wxSAVE);

    SetStatusBar(WxStatusBar1);
    WxToolBar1->SetToolBitmapSize(wxSize(17,16));
    WxToolBar1->Realize();
    SetToolBar(WxToolBar1);
    SetTitle(wxT("Transport Stream Viewer 1.0"));
    SetIcon(wxNullIcon);;
    SetSize(74,144,600,536);
    Center();
}

void ATSProjectFrm::OnClose(wxCloseEvent& event)
{
    Destroy();
}


void ATSProjectFrm::Mnuexit1005Click(wxCommandEvent& event)
{
    Destroy();
}

void ATSProjectFrm::FillListCtrlRecursively(wxString selected, wxTreeItemId Level1)
{
    wxTreeItemIdValue cookie;

    int ccc = 0;

    while ( Level1.IsOk() )
    {
        wxTreeItemId Level2 = WxTreeCtrl1->GetFirstChild(Level1, cookie);
        if(Level2.IsOk()) FillListCtrlRecursively(selected, Level2);

        while (Level2.IsOk())
        {
            if (WxTreeCtrl1->GetItemText(Level1) == selected) 
            {
                int t1 = WxTreeCtrl1->GetItemText(Level2).Find(wxT(":"));
                WxListCtrl1->SetItem(WxListCtrl1->InsertItem(WxListCtrl1->GetItemCount(),
                                     WxTreeCtrl1->GetItemText(Level2).SubString(0, t1)),
                                     1,WxTreeCtrl1->GetItemText(Level2).SubString(t1+1,
                                     WxTreeCtrl1->GetItemText(Level2).Length()));
            }
            Level2 = WxTreeCtrl1->GetNextSibling(Level2);
        }
        Level1 = WxTreeCtrl1->GetNextSibling(Level1);
    }
}

void ATSProjectFrm::WxTreeCtrl2SelChanged(wxTreeEvent& event)
{
    wxTreeItemIdValue cookie;
    WxListCtrl1->DeleteAllItems();
    FillListCtrlRecursively(WxTreeCtrl2->GetItemText(WxTreeCtrl2->GetSelection()), WxTreeCtrl1->GetFirstChild(WxTreeCtrl1->GetRootItem(), cookie));
}

void ATSProjectFrm::VisualizeTSData(TSData *CurrentTSData2)
{
    //Read PAT part
    WxTreeCtrl1->DeleteAllItems();
    WxTreeCtrl2->DeleteAllItems();
    WxListCtrl1->DeleteAllItems();
    WxTreeCtrl1->AddRoot(wxT("Transport Stream Service Information"),-1,-1,NULL );
    WxTreeCtrl2->AddRoot(wxT("Transport Stream Service Information"),-1,-1,NULL ); 
    wxTreeItemId PATId = WxTreeCtrl1->AppendItem(WxTreeCtrl1->GetRootItem(),wxT("Program Association Table PID: 0"),-1,-1,NULL);
    WxTreeCtrl2->AppendItem(WxTreeCtrl2->GetRootItem(),wxT("Program Association Table PID: 0"),-1,-1,NULL);
    
    WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("table_id: %i"), CurrentTSData2->table_id), -1, -1, NULL);
    WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("section_syntax_indicator: %i"), CurrentTSData2->section_syntax_indicator), -1, -1, NULL);
    WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("section_length: %i"), CurrentTSData2->section_length), -1, -1, NULL);
    WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("transport_stream_id: %i"), CurrentTSData2->transport_stream_id), -1, -1, NULL);
    WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("version_number: %i"), CurrentTSData2->version_number), -1, -1, NULL);
    WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("current_next_indicator: %i"), CurrentTSData2->current_next_indicator), -1, -1, NULL);
    WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("section_number: %i"), CurrentTSData2->section_number), -1, -1, NULL);
    WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("last_section_number: %i"), CurrentTSData2->last_section_number), -1, -1, NULL);

    for (int i = 0; i < CurrentTSData2->CountPrg; i++)
    {
        WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("program_number: %i"), CurrentTSData2->CurrentPMT[i]->pr_num), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PATId, wxString::Format(wxT("program_map_PID: %i"), CurrentTSData2->CurrentPMT[i]->pr_PID), -1, -1, NULL);
    }

    wxTreeItemId PMTId1 = WxTreeCtrl1->AppendItem(WxTreeCtrl1->GetRootItem(), wxT("Program Map Tables"),-1,-1,NULL);
    wxTreeItemId PMTId2 = WxTreeCtrl2->AppendItem(WxTreeCtrl2->GetRootItem(), wxT("Program Map Tables"),-1,-1,NULL);

    //Read PMT part
    for (int i = 0; i < CurrentTSData2->CountPrg; i++) 
    {   
        wxTreeItemId PMTIdChild1 = WxTreeCtrl1->AppendItem(PMTId1, wxString::Format(wxT("program_number %i PID: %i"), CurrentTSData2->CurrentPMT[i]->pr_num, CurrentTSData2->CurrentPMT[i]->pr_PID), -1, -1, NULL);
        wxTreeItemId PMTIdChild2 = WxTreeCtrl2->AppendItem(PMTId2, WxTreeCtrl1->GetItemText(PMTIdChild1), -1, -1, NULL);

        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("table_id: %i"), CurrentTSData2->CurrentPMT[i]->table_id), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("section_syntax_indicator: %i"), CurrentTSData2->CurrentPMT[i]->section_syntax_indicator), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("section_length: %i"), CurrentTSData2->CurrentPMT[i]->section_length), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("program_number: %i"), CurrentTSData2->CurrentPMT[i]->program_number), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("version_number: %i"), CurrentTSData2->CurrentPMT[i]->version_number), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("current_next_indicator: %i"), CurrentTSData2->CurrentPMT[i]->current_next_indicator), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("section_number: %i"), CurrentTSData2->CurrentPMT[i]->section_number), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("last_section_number: %i"), CurrentTSData2->CurrentPMT[i]->last_section_number), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("PCR_PID: %i"), CurrentTSData2->CurrentPMT[i]->PCR_PID), -1, -1, NULL);
        WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("program_info_length: %i"), CurrentTSData2->CurrentPMT[i]->program_info_length), -1, -1, NULL);

        //Read video channels info
        for (int j = 0; j < CurrentTSData2->CurrentPMT[i]->CountVideoCh; j++)       
        {
            wxString tmpstring(CurrentTSData2->CurrentPMT[i]->VideoCh[j]->ch_name, wxConvUTF8);
            wxString tmpstring1(CurrentTSData2->CurrentPMT[i]->VideoCh[j]->ch_name, wxConvUTF8);
            wxTreeItemId ChannelID1 = WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("%s PID: %i"), tmpstring, CurrentTSData2->CurrentPMT[i]->VideoCh[j]->ch_PID), -1, -1, NULL);
            WxTreeCtrl2->AppendItem(PMTIdChild2, wxString::Format(wxT("%s PID: %i"), tmpstring1, CurrentTSData2->CurrentPMT[i]->VideoCh[j]->ch_PID), -1, -1, NULL);
            WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("ES_info_length: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->ES_info_length), -1, -1, NULL);

            if (CurrentTSData2->CurrentPMT[i]->VideoCh[j]->Filled)
            {
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("horizontal_size_value: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->horizontal_size_value), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("vertical_size_value: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->vertical_size_value), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("aspect_ratio_information: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->aspect_ratio_information), -1, -1, NULL);
                wxString tmpstring(CurrentTSData2->CurrentPMT[i]->VideoCh[j]->frame_rate_code, wxConvUTF8);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("frame_rate_code: %s"), tmpstring), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("bit_rate_value: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->bit_rate_value), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("marker_bit: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->marker_bit), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("vbv_buffer_size_value: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->vbv_buffer_size_value), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("constrained_parameters_flag: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->constrained_parameters_flag), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("load_intra_quantiser_matrix: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->load_intra_quantiser_matrix), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("load_non_intra_quantiser_matrix: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->load_non_intra_quantiser_matrix), -1, -1, NULL);

                if (CurrentTSData2->CurrentPMT[i]->VideoCh[j]->VideoSequenceExtensionFilled)
                {
                    wxString tmpstring1(CurrentTSData2->CurrentPMT[i]->VideoCh[j]->profile_indication, wxConvUTF8);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("profile_indication: %s"), tmpstring1), -1, -1, NULL);    
                    wxString tmpstring2(CurrentTSData2->CurrentPMT[i]->VideoCh[j]->level_indication, wxConvUTF8);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("level_indication: %s"), tmpstring2), -1, -1, NULL);    
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("progressive_sequence: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->progressive_sequence), -1, -1, NULL);
                    wxString tmpstring3(CurrentTSData2->CurrentPMT[i]->VideoCh[j]->chroma_format, wxConvUTF8);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("profile_indication: %s"), tmpstring3), -1, -1, NULL);                    
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("horizontal_size_extension: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->horizontal_size_extension), -1, -1, NULL);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("vertical_size_extension: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->vertical_size_extension), -1, -1, NULL);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("bit_rate_extension: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->bit_rate_extension), -1, -1, NULL);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("marker_bit_se: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->marker_bit_se), -1, -1, NULL);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("vbv_buffer_size_extension: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->vbv_buffer_size_extension), -1, -1, NULL);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("low_delay: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->low_delay), -1, -1, NULL);
                }

                if (CurrentTSData2->CurrentPMT[i]->VideoCh[j]->VideoSequenceDisplayExtensionFilled)
                {
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("video_format: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->video_format), -1, -1, NULL);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("colour_description: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->colour_description), -1, -1, NULL);
                    if (CurrentTSData2->CurrentPMT[i]->VideoCh[j]->colour_description)
                    {
                        WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("colour_primaries: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->colour_primaries), -1, -1, NULL);
                        WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("transfer_characteristics: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->transfer_characteristics), -1, -1, NULL);
                        WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("matrix_coefficients: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->matrix_coefficients), -1, -1, NULL);
                        WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("display_horizontal_size: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->display_horizontal_size), -1, -1, NULL);
                        WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("display_vertical_size: %i"), CurrentTSData2->CurrentPMT[i]->VideoCh[j]->display_vertical_size), -1, -1, NULL);

                    }
                }
            }
        }

        //Read audio channels info
        for (int j = 0; j < CurrentTSData2->CurrentPMT[i]->CountAudioCh; j++)
        {
            wxString tmpstring(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->ch_name, wxConvUTF8);
            wxString tmpstring1(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->ch_name, wxConvUTF8);
            wxTreeItemId ChannelID1 = WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("%s PID: %i"), tmpstring, CurrentTSData2->CurrentPMT[i]->AudioCh[j]->ch_PID), -1, -1, NULL);
            WxTreeCtrl2->AppendItem(PMTIdChild2, wxString::Format(wxT("%s PID: %i"), tmpstring1, CurrentTSData2->CurrentPMT[i]->AudioCh[j]->ch_PID), -1, -1, NULL);
            WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("ES_info_length: %i"), CurrentTSData2->CurrentPMT[i]->AudioCh[j]->ES_info_length), -1, -1, NULL);

            if (CurrentTSData2->CurrentPMT[i]->AudioCh[j]->Filled)
            {
                wxString tmpstring2(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->ID, wxConvUTF8);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("ID: %s"), tmpstring2), -1, -1, NULL);
                wxString tmpstring3(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->Layer, wxConvUTF8);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("%s"), tmpstring3), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("protection_bit: %i"), CurrentTSData2->CurrentPMT[i]->AudioCh[j]->protection_bit), -1, -1, NULL);
                wxString tmpstring4(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->bitrate, wxConvUTF8);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("bitrate: %s"), tmpstring4), -1, -1, NULL);
                wxString tmpstring5(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->sampling_frequency, wxConvUTF8);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("sampling_frequency: %s"), tmpstring5), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("padding_bit: %i"), CurrentTSData2->CurrentPMT[i]->AudioCh[j]->padding_bit), -1, -1, NULL);
                wxString tmpstring6(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->mode, wxConvUTF8);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("mode: %s"), tmpstring6), -1, -1, NULL);
                if ((tmpstring6 == _T("joint_stereo")) || (tmpstring6 == _T("single_channel")))
                {
                    wxString tmpstring7(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->mode_extension, wxConvUTF8);
                    WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("mode_extension: %s"), tmpstring7), -1, -1, NULL);
                }
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("copyright: %i"), CurrentTSData2->CurrentPMT[i]->AudioCh[j]->copyright), -1, -1, NULL);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("original: %i"), CurrentTSData2->CurrentPMT[i]->AudioCh[j]->original), -1, -1, NULL);
                wxString tmpstring8(CurrentTSData2->CurrentPMT[i]->AudioCh[j]->emphasis, wxConvUTF8);
                WxTreeCtrl1->AppendItem(ChannelID1, wxString::Format(wxT("emphasis: %s"), tmpstring8), -1, -1, NULL);
            }
        }

        //Read other channels info
        for (int j = 0; j < CurrentTSData2->CurrentPMT[i]->CountOtherCh; j++)
        {
            wxString tmpstring(CurrentTSData2->CurrentPMT[i]->OtherCh[j]->ch_name, wxConvUTF8);
            wxString tmpstring1(CurrentTSData2->CurrentPMT[i]->OtherCh[j]->ch_name, wxConvUTF8);
            wxTreeItemId ChannelID1 = WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("%s PID: %i"), tmpstring, CurrentTSData2->CurrentPMT[i]->OtherCh[j]->ch_PID), -1, -1, NULL);
            WxTreeCtrl2->AppendItem(PMTIdChild2, wxString::Format(wxT("%s PID: %i"), tmpstring1, CurrentTSData2->CurrentPMT[i]->OtherCh[j]->ch_PID), -1, -1, NULL);
            WxTreeCtrl1->AppendItem(PMTIdChild1, wxString::Format(wxT("ES_info_length: %i"), CurrentTSData2->CurrentPMT[i]->OtherCh[j]->ES_info_length), -1, -1, NULL);
        }
    }

    WxTreeCtrl1->Expand(WxTreeCtrl1->GetRootItem());
    WxTreeCtrl2->Expand(WxTreeCtrl2->GetRootItem());
}

void ATSProjectFrm::Mnuopen1003Click(wxCommandEvent& event)
{
    if (WxOpenFileDialog1->ShowModal() == wxID_OK)
    {
        wxString fileName = WxOpenFileDialog1->GetPath();
        WxStatusBar1->SetStatusText(fileName, 0);

        CurrentTSData1 = ProcessTSData(fileName.ToUTF8().data());
        if (CurrentTSData1) VisualizeTSData(CurrentTSData1);
        else wxMessageBox( _T("This is not a transport stream file!"), _T("Message about the content of the file"), wxOK);
    }
}

void ATSProjectFrm::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
    wxString title;
    if (id.IsOk())
    {
        wxMenu menu(title);
        title = WxTreeCtrl2->GetItemText(id);

        if (title.Find(wxT("MPEG1 Audio")) != -1)
        {
            menu.Append(ID_WXTREE_PES, wxT("Save as PES stream"));
            menu.Append(ID_WXTREE_ES, wxT("Save as ES stream"));
            menu.Append(ID_WXTREE_PCM, wxT("Save as decoded PCM audio"));
            menu.AppendSeparator();
            menu.Append(ID_WXTREE_XML_Exp, wxT("Export to XML"));

            PopupMenu(&menu, pt);
        }else if (title.Find(wxT("MPEG2 Video")) != -1)
        {
                menu.Append(ID_WXTREE_PES, wxT("Save as PES stream"));
                menu.Append(ID_WXTREE_ES, wxT("Save as ES stream"));
                menu.AppendSeparator();
                menu.Append(ID_WXTREE_XML_Exp, wxT("Export to XML file"));

                PopupMenu(&menu, pt);
        }else wxMessageBox( _T("There is no a valid audio or video stream!"), _T("Message about the content of the stream"), wxOK);
    }
    else wxMessageBox( _T("There is no a valid Id pointer!"), _T("Message about the content of the Item"), wxOK);
}

void ATSProjectFrm::ShowMenuXML_VMF(wxTreeItemId id, const wxPoint& pt)
{
    wxString title;
    if (id.IsOk())
    {
        wxMenu menu(title);
        title = WxTreeCtrl2->GetItemText(id);
        if (title.Find(wxT("program")) != -1)
        {
            menu.Append(ID_WXTREE_XML_Exp, wxT("Export to XML file"));
            menu.Append(ID_WXTREE_VMF_Exp, wxT("Export all metadata to VMF file"));
        }else menu.Append(ID_WXTREE_XML_Exp, wxT("Export to XML file"));

        PopupMenu(&menu, pt);
    }
}

void ATSProjectFrm::OnItemMenu(wxTreeEvent& event)
{
    wxString L1;

    wxTreeItemId itemId = event.GetItem();
    wxPoint clientpt = event.GetPoint();
    WxTreeCtrl2->SelectItem(itemId);

    if (WxTreeCtrl2->GetRootItem() != itemId) L1 = WxTreeCtrl2->GetItemText(WxTreeCtrl2->GetItemParent(itemId));

    if (L1.Find(wxT("program_number")) != -1) ShowMenu(itemId, clientpt);
    else ShowMenuXML_VMF(itemId, clientpt);
}

void ATSProjectFrm::ExtractItemPES_ES_PCM(wxCommandEvent& event)
{
    int evtID =  event.GetId();

    wxTreeItemId item = WxTreeCtrl2->GetSelection();
    wxString wxPID = WxTreeCtrl2->GetItemText(item);
    wxPID = wxPID.SubString(17, wxPID.Length());
    int intPID = wxAtoi(wxPID);

    if (evtID == 1017 || evtID == 1018)
    {
        if (WxOpenFileDialog2->ShowModal() == wxID_OK)
        {
            ExtractESF_PES_ES(WxOpenFileDialog1->GetPath().ToUTF8().data(), WxOpenFileDialog2->GetPath().ToUTF8().data(), intPID, evtID);
        }
    }else 
    {
        wxString dirPCM = WxOpenFileDialog1->GetDirectory();
    
        wxDirDialog dialog(this, _T("Choose a directory where to save PCM files"), dirPCM, wxDD_DEFAULT_STYLE);


        if (dialog.ShowModal() == wxID_OK)
        {
            ExtractESF_PCM(WxOpenFileDialog1->GetPath().ToUTF8().data(), dialog.GetPath().ToUTF8().data(), intPID);
        }
    }
}

void ATSProjectFrm::TreeItemXMLExport(wxCommandEvent& event)
{
    wxFileDialog dialog(this, wxT("Give the name of the output XML file"), wxT(""), wxT(""), wxT("*.*"), wxSAVE);

    if (dialog.ShowModal() == wxID_OK)
    {
        FILE *fp = fopen (dialog.GetPath().ToUTF8().data(), "wt");
    
        if (!fp)
        {
            fprintf(stderr, "Can't write in file %s\n", fp);
        }else
        {
            wxTreeItemIdValue cookie;
  
            fprintf(fp, "<?xml version=\"%1.1f\"?>\n", 1.0);

            if (WxTreeCtrl2->GetItemText(WxTreeCtrl2->GetSelection()) != WxTreeCtrl2->GetItemText(WxTreeCtrl1->GetRootItem())) 
            {
                fprintf(fp, "<TransportStreamServiceInformation>\n");
                ATS_XMLExp::XMLExport(WxTreeCtrl1, WxTreeCtrl1->GetRootItem(), WxTreeCtrl2->GetItemText(WxTreeCtrl2->GetSelection()), fp);
                fprintf(fp, "</TransportStreamServiceInformation>\n");
            }else ATS_XMLExp::XMLExport(WxTreeCtrl1, WxTreeCtrl1->GetRootItem(), WxTreeCtrl2->GetItemText(WxTreeCtrl2->GetSelection()), fp);
            fclose(fp);
        }
    }
}
