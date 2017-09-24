#include "ATS_XMLVMFExp.h"
#define TT_LOG_THIS TT_LOG_CATEGORY_MAIN

wxString ATS_XMLExp::Tag_Value(wxString *tag2)
{
    wxString tag2v;
    int tp = -1;
    if (tag2->Find(_T("PID:")) != -1)
    {
         if (tag2->Find(_T("program_number")) != -1)
         {
            tp = tag2->Find(_T("PID:"));
            tag2v = _T("PrNum=\"") + tag2->SubString(15, tp - 2) + _T("\"");
            tag2v += _T(" PID=\"") + tag2->SubString(tp + 5, tag2->Length()) + _T("\"");
            *tag2 = _T("program_number");
         }else
         {
            tp = tag2->Find(_T("PID:"));
            tag2v = _T("PID=\"") + tag2->SubString(tp + 5, tag2->Length()) + _T("\""); 
            *tag2 = tag2->SubString(0, tp - 1); 
            tag2->Replace(_T(" "), _T(""), true);
            tag2->Replace(_T("("), _T(""), true);
            tag2->Replace(_T(")"), _T(""), true);
            tag2->Replace(_T("/"), _T(""), true);
         }
    }else if (tag2->Find(_T(":")) != -1)
    {
            tp = tag2->Find(_T(":"));
            tag2v = tag2->SubString(tp + 2, tag2->Length());
            *tag2 = tag2->SubString(0, tp - 1);
            tag2->Replace(_T(" "), _T(""), true);
            tag2->Replace(_T("("), _T(""), true);
            tag2->Replace(_T(")"), _T(""), true);
            tag2->Replace(_T("/"), _T(""), true);

    }else
    {
           tag2v = _T(""); 
           tag2->Replace(_T(" "), _T(""), true);
           tag2->Replace(_T("("), _T(""), true);
           tag2->Replace(_T(")"), _T(""), true);
           tag2->Replace(_T("/"), _T(""), true);
    }
    return tag2v;
}

void ATS_XMLExp::XMLExportSubLevels(wxTreeCtrl *WxTreeCtrl22 , wxTreeItemId Level, FILE *fp)
{
    wxTreeItemIdValue cookie;

    while (Level.IsOk())
    {
        wxString tag2v;
        wxString tag2 = WxTreeCtrl22->GetItemText(Level);
        wxTreeItemId Level1 = WxTreeCtrl22->GetFirstChild(Level, cookie);

        tag2v = Tag_Value(&tag2);

        if (tag2v.Find(_T("PID=")) != -1)
        {
            fprintf(fp, "<%s>\n", (tag2 + _T(" ") + tag2v).ToUTF8().data());
            //fprintf(fp, "%s\n", tag2v.SubString(0, tag2v.Find(_T("PID=")) - 2).ToUTF8().data());
            //fprintf(fp, "%s\n", tag2v.SubString(tag2v.Find(_T("PID=")), tag2v.Length()).ToUTF8().data());
        }else 
        {   
            fprintf(fp, "<%s>\n",tag2.ToUTF8().data());
            fprintf(fp, "%s\n",tag2v.ToUTF8().data());
        }

        XMLExportSubLevels(WxTreeCtrl22, Level1, fp);

        fprintf(fp, "</%s>\n",tag2.ToUTF8().data());

        Level = WxTreeCtrl22->GetNextSibling(Level);
    }
}

void ATS_XMLExp::XMLExport(wxTreeCtrl *WxTreeCtrl22 , wxTreeItemId Level1, wxString selected, FILE *fp)
{
    wxTreeItemIdValue cookie;

    while ( Level1.IsOk() )
    {
        wxTreeItemId Level2 = WxTreeCtrl22->GetFirstChild(Level1, cookie);
        if(Level2.IsOk()) XMLExport(WxTreeCtrl22, Level2, selected, fp);

        while (Level2.IsOk())
        {
            if ((WxTreeCtrl22->GetItemText(Level1) == selected) & !XMLinit)
            {
                wxString tag2v;
                wxString tag2 = WxTreeCtrl22->GetItemText(Level1);

                tag2v = Tag_Value(&tag2);

                if (tag2v.Find(_T("PID=")) != -1)
                {
                    fprintf(fp, "<%s>\n", (tag2 + _T(" ") + tag2v).ToUTF8().data());
                    //fprintf(fp, "%s\n", tag2v.SubString(0, tag2v.Find(_T("PID:")) - 2).ToUTF8().data());
                    //fprintf(fp, "%s\n", tag2v.SubString(tag2v.Find(_T("PID:")), tag2v.Length()).ToUTF8().data());
                }else 
                {
                    fprintf(fp, "<%s>\n",tag2.ToUTF8().data());
                    fprintf(fp, "%s\n",tag2v.ToUTF8().data());
                }

                XMLExportSubLevels(WxTreeCtrl22, Level2, fp);

                fprintf(fp, "</%s>\n",tag2.ToUTF8().data());
                XMLinit = 1;
            }
            if(Level2.IsOk()) Level2 = WxTreeCtrl22->GetNextSibling(Level2);
        }
        Level1 = WxTreeCtrl22->GetNextSibling(Level1);
    }
    XMLinit = 0;
}

sc_result ATS_VMFExp::RegisterParser()
{
    sc_result sr;

    sr = InitializeMetaDataWriter();
    if (SC_IS_ERROR(sr))
    {
        fprintf(stderr, "InitializeMetaDataWriter failed!");
        exit(1);
    }

    return sr;
}

sc_result ATS_VMFExp::TSParserCreateSession(char *Metafname, TSParserSession *iMData)
{
    sc_result sr;
    sc_file_object  essence_file;
    wchar_t wcstring[256];

    mbstowcs(wcstring, Metafname, 256);
    essence_file.tc_name = wcstring;

    sr = StartWriteMetadata(&essence_file, &iMData->writer_session);
    if (SC_IS_ERROR(sr))
    {
        fprintf(stderr, "StartWriteMetadata failed!");
        exit(1);
    }
    
    iMData->ts_parser_state = TS_PARSER_STATE_SESSION_CREATED;

    return sr;
}

sc_result ATS_VMFExp::PrepareForParsing(const char *TSfname, TSData *CurrentTSData3, TSParserSession *iMData)
{
    sc_result              sr = SC_MAKE_SUCCESS(SC_CODE_GENERIC);
    sc_uint32              ulCreationTime; 
    sc_uint32              ulModificationTime;
    sc_uint64              ulSize;
    sc_file_object         ts_file;
    sc_file_handle         hFile;
    wchar_t                wcstring[256];

    if (iMData->ts_parser_state == TS_PARSER_STATE_PARSING_COMPLETED
    {
        return SC_MAKE_SUCCESS(SC_CODE_END_OF_FILE);
    }

    memset(&iMData->video_params, 0, sizeof(VideoParameters));
    memset(&iMData->audio_params, 0, sizeof(AudioParameters));
    memset(&iMData->track_params, 0, sizeof(TrackParameters));

    mbstowcs(wcstring, TSfname, 256);
    ts_file.tc_name = wcstring;

    sr = sc_open(&ts_file, &hFile, IO_FLAG_READ);
    if(SC_IS_ERROR(sr))
    {
        fprintf(stderr, "sc_open failed!");
        return sr;
    }

    ulCreationTime = sc_file_creation_time(hFile);
    ulModificationTime = sc_file_modification_time(hFile);
    sr = SetFileDate(ulCreationTime, ulModificationTime, iMData->writer_session);
    if (SC_IS_ERROR(sr))
    {
        fprintf(stderr, "SetFileDate failed!");
        sc_close(hFile);
        exit(1);
    }

    ulSize = sc_file_size(hFile);
    sr = SetFileSize(ulSize, iMData->writer_session);
    if (SC_IS_ERROR(sr))
    {
        fprintf(stderr, "SetFileSize failed!");
        sc_close(hFile);
        exit(1);
    }

    sr = GenerateUniqueID(hFile, ulSize, iMData);
    if (SC_IS_ERROR(sr))
    {
        fprintf(stderr, "GenerateUniqueID failed!");
        sc_close(hFile);
        exit(1);
    }

    sr = SetOriginalFormat(M2T, iMData->writer_session);
    if (SC_IS_ERROR(sr))
    {
        fprintf(stderr, "SetOriginalFormat failed!");
        sc_close(hFile);
        exit(1);
    }

    sr = sc_close(hFile);
    if (SC_IS_ERROR(sr))
    {
        fprintf(stderr, "sc_close failed!");
        exit(1);
    }

    iMData->end_write_metadata_reason = MD_SUCCESS_COMPLETED;

    return sr;
}

sc_result ATS_VMFExp::TSParse(TSData *CurrentTSData3, TSParserSession *iMData)
{
    sc_result       sr;
    sc_uint16       current_track;
    sc_uint16       audio_track;
    sc_uint16       video_track;
    AudioParameters audio_params;
    
    iMData->numTracks = CurrentTSData3->CurrentPMT[0].CountAudioCh + CurrentTSData3->CurrentPMT[0].CountVideoCh;

    sr = SetNumberOfTracks(iMData->numTracks, iMData->writer_session);
    if (SC_IS_ERROR(sr))
    {
        fprintf(stderr, "SetNumberOfTracks failed!");
        exit(1);
    }

    for (audio_track = 0; audio_track < CurrentTSData3->CurrentPMT[0].CountAudioCh ; ++audio_track)
    {
        InitializeTrackParams(&iMData->track_params, AUDIO_TRACK);

        sr = SetAudioParameters(iMData, CurrentTSData3, iMData->track_params.trackID, audio_track);
        if (SC_IS_ERROR(sr))
        {
            fprintf(stderr, "SetAudioParameters failed!");
            exit(1);
        }
    }

    for (video_track = 0; video_track < CurrentTSData3->CurrentPMT[0].CountVideoCh ; ++video_track)
    {
        InitializeTrackParams(&iMData->track_params, VIDEO_TRACK);

        sr = SetVideoParameters(iMData, CurrentTSData3, iMData->track_params.trackID, video_track);
        if(SC_IS_ERROR(sr))
        {
            fprintf(stderr, "SetVideoParameters failed!");
            exit(1);
        }

    }

    sr = EndWriteMetadata(MD_SUCCESS_COMPLETED, iMData->writer_session);

    if(SC_IS_ERROR(sr))
    {
        fprintf(stderr, "EndWriteMetadata failed!");
        exit(1);
    }*/

    return sr;
}