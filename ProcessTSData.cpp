#include "ProcessTSData.h"

int ExtractDataBits(int startbit, int countbits, unsigned char bufPAT[], int startbyte)
{
    unsigned int p = bufPAT[startbyte + 4];
    p <<= 8;
    p += bufPAT[startbyte + 5];

    if (p != 65535)
    {
         int nbytes = (countbits / 8);
         if (countbits % 8) nbytes++;

         for (int i = 0; i < nbytes; i++)
         {
                 p <<= 8;
                 p += bufPAT[startbyte + i];
         }

         p <<= 39 - nbytes*8 - startbit;
         p >>= 32 - countbits;
         return p;
    }else return p;
}

int ExtractDataBitsN(unsigned char **indexin, int countbits)
{
    unsigned int p = 0;

    if (p != 65535)
    {
         int nbytes = (countbits / 8);
         if (countbits % 8) nbytes++;

         for (int i = 0; i < nbytes; i++)
         {
                 p <<= 8;
                 p += **indexin;
                 (*indexin)++;
                 
         }

         p <<= 39 - nbytes*8 - 4;
         p >>= 32 - countbits;
         return p;
    }else return p;
}

void skip_n_bits(ts_bit_stream *bs, int countbits)
{
    int nbytes = countbits / 8;
    if (countbits % 8) nbytes++;

    for (int i = 0; i < nbytes; i++)
    {
        if(countbits > bs->cur_bit_idx) 
        {
            bs->cur_byte_idx++;
            bs->cur_byte_ptr++;                 
        }
    }

    for (int j = 0; j < countbits; j++)
    {
        bs->cur_bit_idx--;
        if(bs->cur_bit_idx<0) bs->cur_bit_idx = 7;
    }
}

char* PMTStream_type(int data)
{
    char *result_type = "";
     
    if ((data >= 0x15) & (data <= 0x7F)) return result_type = "Reserved"; 
    else
    {
        switch (data)
        {
            case 0x00: result_type = "Reserved"; break;
            case 0x01: result_type = "Video"; break;
            case 0x02: result_type = "MPEG2 Video"; break;
            case 0x03: result_type = "MPEG1 Audio"; break;
            case 0x04: result_type = "13818-3 Audio"; break;
            case 0x05: result_type = "13818-1 private_sections"; break;
            case 0x06: result_type = "PES private data"; break;
            case 0x07: result_type = "MHEG"; break;
            case 0x08: result_type = "DSM-CC"; break;
            case 0x09: result_type = "ITU-T Rec. H.222.1"; break;
            case 0x0A: result_type = "13818-6 type A"; break;
            case 0x0B: result_type = "DSMCC carousel"; break;
            case 0x0C: result_type = "13818-6 type C"; break;
            case 0x0D: result_type = "13818-6 type D"; break;
            case 0x0E: result_type = "13818-1 auxiliary"; break;
            case 0x0F: result_type = "Audio with ADTS"; break;
            case 0x10: result_type = "Visual"; break;
            case 0x11: result_type = "Audio with the LATM"; break;
            case 0x12: result_type = "SL-packetized stream in PES packets"; break;
            case 0x13: result_type = "SL-packetized stream ISO/IEC14496_sections"; break;
            case 0x14: result_type = "Synchronized Download Protocol"; break;
        
            default: result_type = "User Private";
        }
        return result_type;
    }
}

void ProcessPMTData(int p, ts_bit_stream *bs, TSData *CurrTSData)
{
    skip_n_bits(&(*bs), 16);
    CurrTSData->CurrentPMT[p]->table_id = ExtractDataBitsN(&(*bs), 8);
    CurrTSData->CurrentPMT[p]->section_syntax_indicator = ExtractDataBitsN(&(*bs), 1);
    skip_n_bits(&(*bs), 3);
    CurrTSData->CurrentPMT[p]->section_length = ExtractDataBitsN(&(*bs), 12);
    CurrTSData->CurrentPMT[p]->program_number = ExtractDataBitsN(&(*bs), 16);
    skip_n_bits(&(*bs), 2);
    CurrTSData->CurrentPMT[p]->version_number = ExtractDataBitsN(&(*bs), 5);
    CurrTSData->CurrentPMT[p]->current_next_indicator = ExtractDataBitsN(&(*bs), 1);
    CurrTSData->CurrentPMT[p]->section_number = ExtractDataBitsN(&(*bs), 8);
    CurrTSData->CurrentPMT[p]->last_section_number = ExtractDataBitsN(&(*bs), 8);
    skip_n_bits(&(*bs), 3);
    CurrTSData->CurrentPMT[p]->PCR_PID = ExtractDataBitsN(&(*bs), 13);
    skip_n_bits(&(*bs), 4);
    CurrTSData->CurrentPMT[p]->program_info_length = ExtractDataBitsN(&(*bs), 12); 
    skip_n_bits(&(*bs), CurrTSData->CurrentPMT[p]->program_info_length*8);

    for (int i = 0; i < 255; i++)
    {
        int tmpPAT1 = 0;
        int tmpPAT2 = ExtractDataBitsN(&(*bs), 8);
        if (tmpPAT2 < 65535)
        {
            CurrTSData->CurrentPMT[p]->CountPrgCh++;
            skip_n_bits(&(*bs), 3);
            int tmpPAT3 = ExtractDataBitsN(&(*bs), 13);
            char *tmpStream_type = PMTStream_type(tmpPAT2);

            if (!strcmp(tmpStream_type, "MPEG2 Video"))
            {
                struct Video  ts,*ts_p,**ts_pp;    
                signed char *ts_cc;
                memset(&ts,0,sizeof(ts));

                ts.ch_name = tmpStream_type;
                ts.ch_PID = tmpPAT3;
                skip_n_bits(&(*bs), 4);
                tmpPAT1 = ExtractDataBitsN(&(*bs), 12);
                ts.ES_info_length = tmpPAT1;
                ts.Filled = false;
                CurrTSData->CountAVOCh++;

                ts_p = (Video*) malloc(sizeof(*ts_p));
                memcpy(ts_p,&ts,sizeof(*ts_p));
                CurrTSData->CurrentPMT[p]->VideoCh = (Video**) realloc(CurrTSData->CurrentPMT[p]->VideoCh, (CurrTSData->CurrentPMT[p]->CountVideoCh+1)*4);
                ts_cc =  (signed char *) CurrTSData->CurrentPMT[p]->VideoCh + 4 * (CurrTSData->CurrentPMT[p]->CountVideoCh)++;
                ts_pp = (Video**) ts_cc;
                *ts_pp = ts_p;

            } else if (!strcmp(tmpStream_type, "MPEG1 Audio"))
            {
                struct Audio  ts,*ts_p,**ts_pp;    
                signed char *ts_cc;
                memset(&ts,0,sizeof(ts));

                ts.ch_name = tmpStream_type;
                ts.ch_PID = tmpPAT3;
                skip_n_bits(&(*bs), 4);
                tmpPAT1 = ExtractDataBitsN(&(*bs), 12);
                ts.ES_info_length = tmpPAT1;
                ts.Filled = false;
                CurrTSData->CountAVOCh++;

                ts_p = (Audio*) malloc(sizeof(*ts_p));
                memcpy(ts_p,&ts,sizeof(*ts_p));
                CurrTSData->CurrentPMT[p]->AudioCh = (Audio**) realloc(CurrTSData->CurrentPMT[p]->AudioCh, (CurrTSData->CurrentPMT[p]->CountAudioCh+1)*4);
                ts_cc =  (signed char *) CurrTSData->CurrentPMT[p]->AudioCh + 4 * (CurrTSData->CurrentPMT[p]->CountAudioCh)++;
                ts_pp = (Audio**) ts_cc;
                *ts_pp = ts_p;
            } else 
            {  
                struct OtherType  ts,*ts_p,**ts_pp;    
                signed char *ts_cc;
                memset(&ts,0,sizeof(ts));

                ts.ch_name = tmpStream_type;
                ts.ch_PID = tmpPAT3;
                skip_n_bits(&(*bs), 4);
                tmpPAT1 = ExtractDataBitsN(&(*bs), 12);
                ts.ES_info_length = tmpPAT1;
                ts.Filled = false;
                CurrTSData->CountAVOCh++;

                ts_p = (OtherType*) malloc(sizeof(*ts_p));
                memcpy(ts_p,&ts,sizeof(*ts_p));
                CurrTSData->CurrentPMT[p]->OtherCh = (OtherType**) realloc(CurrTSData->CurrentPMT[p]->OtherCh, (CurrTSData->CurrentPMT[p]->CountOtherCh+1)*4);
                ts_cc =  (signed char *) CurrTSData->CurrentPMT[p]->OtherCh + 4 * (CurrTSData->CurrentPMT[p]->CountOtherCh)++;
                ts_pp = (OtherType**) ts_cc;
                *ts_pp = ts_p;
            }
            ExtractDataBitsN(&(*bs), tmpPAT1*8);
        } else break;
    }
}

int PrgNum(ts_bit_stream *bs)
{
    int tmpPrgNum = 0;
    for (int i = 13; i < 65535; i += 4)
    {
        int tmpPAT2 = ExtractDataBitsN(&(*bs), 16);
        
        ExtractDataBitsN(&(*bs), 16);

        if (tmpPAT2 < 65535) 
        {
            if (tmpPAT2) tmpPrgNum++;
        }else return tmpPrgNum;
    }
    return tmpPrgNum;
}

void ProcessPATData(ts_bit_stream *bs, TSData *CurrTSData)
{
    struct PMT  ts,*ts_p,**ts_pp;

    signed char *ts_cc;
    memset(&ts,0,sizeof(ts));

    skip_n_bits(&(*bs), 16);
    CurrTSData->table_id = ExtractDataBitsN(&(*bs), 8);
    CurrTSData->section_syntax_indicator = ExtractDataBitsN(&(*bs), 1);
    skip_n_bits(&(*bs), 3);
    CurrTSData->section_length = ExtractDataBitsN(&(*bs), 12);
    CurrTSData->transport_stream_id = ExtractDataBitsN(&(*bs), 16);
    skip_n_bits(&(*bs), 2);
    CurrTSData->version_number = ExtractDataBitsN(&(*bs), 5);
    CurrTSData->current_next_indicator = ExtractDataBitsN(&(*bs), 1);
    CurrTSData->section_number = ExtractDataBitsN(&(*bs), 8);
    CurrTSData->last_section_number = ExtractDataBitsN(&(*bs), 8);
    CurrTSData->CountPrg = 0;

    for (int i = 0; i < 255; i++)
    {
        int tmpPAT2 = ExtractDataBitsN(&(*bs), 16);

        if (tmpPAT2 < 65535)
        {
            ts.pr_num = tmpPAT2;
            skip_n_bits(&(*bs), 3);
            ts.pr_PID = ExtractDataBitsN(&(*bs), 13);
            ts.CountPrgCh = 0;

            if (ts.pr_num)
            {
                ts_p = (PMT*) malloc(sizeof(*ts_p));
                memcpy(ts_p,&ts,sizeof(*ts_p));
                CurrTSData->CurrentPMT = (PMT**) realloc(CurrTSData->CurrentPMT, (CurrTSData->CountPrg+1)*4);
                ts_cc =  (signed char *) CurrTSData->CurrentPMT + 4 * (CurrTSData->CountPrg)++;
                ts_pp = (PMT**) ts_cc;
                *ts_pp = ts_p;
            }
        }else break;
    }
}

char* ESAudioBitrate(int layer, int bit_rate_index)
{
    char* result_bitrate = "";

    switch (layer)
    {
        case 3:
            {
                switch ( bit_rate_index )
                {
                    case 1: result_bitrate = "32 kbit/s" ; break;
                    case 2: result_bitrate = "64 kbit/s"; break;
                    case 3: result_bitrate = "96 kbit/s"; break;
                    case 4: result_bitrate = "128 kbit/s"; break;
                    case 5: result_bitrate = "160 kbit/s"; break;
                    case 6: result_bitrate = "192 kbit/s"; break;
                    case 7: result_bitrate = "224 kbit/s"; break;
                    case 8: result_bitrate = "256 kbit/s"; break;
                    case 9: result_bitrate = "288 kbit/s"; break;
                    case 10: result_bitrate = "320 kbit/s"; break;
                    case 11: result_bitrate = "352 kbit/s"; break;
                    case 12: result_bitrate = "384 kbit/s"; break;
                    case 13: result_bitrate = "416 kbit/s"; break;
                    case 14: result_bitrate = "448 kbit/s"; break;
                }
            }; break;
        case 2:
            {
                switch ( bit_rate_index )
                {
                    case 1: result_bitrate = "32 kbit/s"; break;
                    case 2: result_bitrate = "48 kbit/s"; break;
                    case 3: result_bitrate = "56 kbit/s"; break;
                    case 4: result_bitrate = "64 kbit/s"; break;
                    case 5: result_bitrate = "80 kbit/s"; break;
                    case 6: result_bitrate = "96 kbit/s"; break;
                    case 7: result_bitrate = "112 kbit/s"; break;
                    case 8: result_bitrate = "128 kbit/s"; break;
                    case 9: result_bitrate = "160 kbit/s"; break;
                    case 10: result_bitrate = "192 kbit/s"; break;
                    case 11: result_bitrate = "224 kbit/s"; break;
                    case 12: result_bitrate = "256 kbit/s"; break;
                    case 13: result_bitrate = "320 kbit/s"; break;
                    case 14: result_bitrate = "384 kbit/s"; break;
                }
            }; break;
        case 1: 
            {
                switch ( bit_rate_index )
                {
                    case 1: result_bitrate = "32 kbit/s"; break;
                    case 2: result_bitrate = "40 kbit/s"; break;
                    case 3: result_bitrate = "48 kbit/s"; break;
                    case 4: result_bitrate = "56 kbit/s"; break;
                    case 5: result_bitrate = "64 kbit/s"; break;
                    case 6: result_bitrate = "80 kbit/s"; break;
                    case 7: result_bitrate = "96 kbit/s"; break;
                    case 8: result_bitrate = "112 kbit/s"; break;
                    case 9: result_bitrate = "128 kbit/s"; break;
                    case 10: result_bitrate = "160 kbit/s"; break;
                    case 11: result_bitrate = "192 kbit/s"; break;
                    case 12: result_bitrate = "224 kbit/s"; break;
                    case 13: result_bitrate = "256 kbit/s"; break;
                    case 14: result_bitrate = "320 kbit/s"; break;
                }
            }; break;

        default: result_bitrate = "0 kbit/s";
    }
    return result_bitrate;
}

char* FFrame_rate_code(int data)
{
    char* result_rate = "";

    switch (data)
    {
        case 0: result_rate = "Forbidden"; break; //0000
        case 1: result_rate = "23.976000";break; //0001
        case 2: result_rate = "24";break; //0010
        case 3: result_rate = "25";break; //0011
        case 4: result_rate = "29.970000";break; //0100
        case 5: result_rate = "30"; break; //0101
        case 6: result_rate = "50";break; //0110
        case 7: result_rate = "59.940000";break; //0111
        case 8: result_rate = "60";break; //1000

        default: result_rate = "reserved";
    }
    return result_rate;
}

void VideoSequenceExtension(ts_bit_stream *bs, int i2, int j2, TSData *CurrTSData2)
{
    skip_n_bits(&(*bs), 1);

    switch (ExtractDataBitsN(&(*bs), 3))
    {
        case 1: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->profile_indication = "High"; break; 
        case 2: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->profile_indication = "Spatially Scalable"; break; 
        case 3: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->profile_indication = "SNR Scalable"; break;
        case 4: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->profile_indication = "Main"; break; 
        case 5: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->profile_indication = "Simple"; break; 

        default: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->profile_indication = "Reserved";
    }

    switch (ExtractDataBitsN(&(*bs), 4))
    {
        case 4: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->level_indication = "High"; break; 
        case 6: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->level_indication = "High 1440"; break; 
        case 8: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->level_indication = "Main"; break;
        case 10: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->level_indication = "Low"; break; 

        default: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->level_indication = "Reserved";
    }

    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->progressive_sequence = ExtractDataBitsN(&(*bs), 1);

    switch (ExtractDataBitsN(&(*bs), 2))
    {
        case 0: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->chroma_format = "Reserved"; break; 
        case 1: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->chroma_format = "4:2:0"; break; 
        case 2: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->chroma_format = "4:2:2"; break;
        case 3: CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->chroma_format = "4:4:4"; break;
    }

    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->horizontal_size_extension = ExtractDataBitsN(&(*bs), 2);
    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->vertical_size_extension = ExtractDataBitsN(&(*bs), 2);
    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->bit_rate_extension = ExtractDataBitsN(&(*bs), 12);
    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->marker_bit_se = ExtractDataBitsN(&(*bs), 1);
    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->vbv_buffer_size_extension = ExtractDataBitsN(&(*bs), 8);
    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->low_delay = ExtractDataBitsN(&(*bs), 1);
    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->frame_rate_extension_n = ExtractDataBitsN(&(*bs), 2);
    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->frame_rate_extension_d = ExtractDataBitsN(&(*bs), 5);

    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->VideoSequenceExtensionFilled = true;
}

void VideoSequenceDisplayExtension(ts_bit_stream *bs, int i2, int j2, TSData *CurrTSData2)
{
    skip_n_bits(&(*bs), 4);

    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->video_format = ExtractDataBitsN(&(*bs), 8);
    int tmpColour_description = ExtractDataBitsN(&(*bs), 1);
    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->colour_description = tmpColour_description;
    if (tmpColour_description)
    {
        CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->colour_primaries = ExtractDataBitsN(&(*bs), 8);
        CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->transfer_characteristics = ExtractDataBitsN(&(*bs), 8);
        CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->matrix_coefficients = ExtractDataBitsN(&(*bs), 8);
        CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->display_horizontal_size = ExtractDataBitsN(&(*bs), 14);
        CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->marker_bit_sde = ExtractDataBitsN(&(*bs), 1);
        CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->display_vertical_size = ExtractDataBitsN(&(*bs), 14);
    }

    CurrTSData2->CurrentPMT[i2]->VideoCh[j2]->VideoSequenceDisplayExtensionFilled = true;
}

void AddPESSectionAudio(ts_bit_stream *bs, int i1, int j1, TSData *CurrTSData)
{ 
    int tmpPAT3, tmpALayer;

    tmpPAT3 = ExtractDataBitsN(&(*bs), 1); 
    if (tmpPAT3) CurrTSData->CurrentPMT[i1]->AudioCh[j1]->ID = "1 (MPEG audio)";
    else CurrTSData->CurrentPMT[i1]->AudioCh[j1]->ID = "0 (Reserved)";

    tmpALayer = ExtractDataBitsN(&(*bs), 2);
    switch (tmpALayer)
    {
        case 3: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->Layer = "Layer: I"; break; //11
        case 2: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->Layer = "Layer: II"; break; //10
        case 1: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->Layer = "Layer: III"; break; //01
        case 0: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->Layer = "Reserved"; break; //00
    }

    CurrTSData->CurrentPMT[i1]->AudioCh[j1]->protection_bit = ExtractDataBitsN(&(*bs), 1);
    CurrTSData->CurrentPMT[i1]->AudioCh[j1]->bitrate = ESAudioBitrate(tmpALayer , ExtractDataBitsN(&(*bs), 4));

    switch (ExtractDataBitsN(&(*bs), 2))
    {
        case 0: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->sampling_frequency = "44.1 kHz"; break; //00
        case 1: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->sampling_frequency = "48 kHz"; break; //01
        case 2: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->sampling_frequency = "32 kHz"; break; //10
        case 3: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->sampling_frequency = "Reserved"; break; //11
    }

    CurrTSData->CurrentPMT[i1]->AudioCh[j1]->padding_bit = ExtractDataBitsN(&(*bs), 1);

    switch (ExtractDataBitsN(&(*bs), 2))
    {
        case 0: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode = "stereo"; break; //00
        case 1: {
                CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode = "joint_stereo"; //01
                switch (ExtractDataBitsN(&(*bs), 2))
                {
                    case 0: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode_extension = "subbands 4-31 in intensity_stereo, bound==4"; break; //00
                    case 1: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode_extension = "subbands 8-31 in intensity_stereo, bound==8"; break; //01
                    case 2: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode_extension = "subbands 12-31 in intensity_stereo, bound==12"; break; //10
                    case 3: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode_extension = "subbands 16-31 in intensity_stereo, bound==16"; break; //11
                }
            }; break;
        case 2: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode = "dual_channel"; break; //10
        case 3:
            {
                CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode = "single_channel"; 
                switch (ExtractDataBitsN(&(*bs), 2))
                {
                    case 0: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode_extension = "intensity_stereo-off, ms_stereo-off"; break; //00
                    case 1: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode_extension = "intensity_stereo-on, ms_stereo-off"; break; //01
                    case 2: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode_extension = "intensity_stereo-off, ms_stereo-on"; break; //10
                    case 3: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->mode_extension = "intensity_stereo-on, ms_stereo-on"; break; //11
                }
            }; break;
    }

    CurrTSData->CurrentPMT[i1]->AudioCh[j1]->copyright = ExtractDataBitsN(&(*bs), 1);
    CurrTSData->CurrentPMT[i1]->AudioCh[j1]->original = ExtractDataBitsN(&(*bs), 1);

    switch (ExtractDataBitsN(&(*bs), 2))
    {
        case 0: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->emphasis = "no emphasis"; break; //00
        case 1: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->emphasis = "50/15 microsec. emphasis"; break; //01
        case 2: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->emphasis = "Reserved"; break; //10
        case 3: CurrTSData->CurrentPMT[i1]->AudioCh[j1]->emphasis = "CCITT J.17"; break; //11
    }

    CurrTSData->CurrentPMT[i1]->AudioCh[j1]->Filled = true;
}

void AddPESSectionVideo(ts_bit_stream *bs, int i1, int j1, TSData *CurrTSData)
{ 
    int pos_extension_start_code;

    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->horizontal_size_value = ExtractDataBitsN(&(*bs), 12);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->vertical_size_value = ExtractDataBitsN(&(*bs), 12);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->aspect_ratio_information = ExtractDataBitsN(&(*bs), 4);
    int tmpFrame_rate_code = ExtractDataBitsN(&(*bs), 4);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->frame_rate_code = FFrame_rate_code(tmpFrame_rate_code);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->bit_rate_value = ExtractDataBitsN(&(*bs), 18);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->marker_bit = ExtractDataBitsN(&(*bs), 1);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->vbv_buffer_size_value = ExtractDataBitsN(&(*bs), 10);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->constrained_parameters_flag = ExtractDataBitsN(&(*bs), 1);
    int tmpLIQM = ExtractDataBitsN(&(*bs), 1);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->load_intra_quantiser_matrix = tmpLIQM;
    if (tmpLIQM) CurrTSData->CurrentPMT[i1]->VideoCh[j1]->load_non_intra_quantiser_matrix = ExtractDataBitsN(&(*bs), 1);;
    else CurrTSData->CurrentPMT[i1]->VideoCh[j1]->load_non_intra_quantiser_matrix = ExtractDataBitsN(&(*bs), 1);
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->VideoSequenceExtensionFilled = false;
    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->VideoSequenceDisplayExtensionFilled = false;

    do
    {
        for(int i = bs->cur_byte_idx; i < 184; i++)
        {
            if (ExtractDataBitsN(&(*bs), 32) == 0x1B5) 
            {
                break; 
            }else
            {
                bs->cur_byte_idx -= 3;
                bs->cur_byte_ptr -= 3;
            }
        }
        switch (ExtractDataBitsN(&(*bs), 4))
        {
            case 1: VideoSequenceExtension(&(*bs), i1, j1, CurrTSData); break;//Sequence Extension ID 
            case 2: VideoSequenceDisplayExtension(&(*bs), i1, j1, CurrTSData); break;//Sequence Extension ID
        }
        bs->cur_bit_idx = 7;
    }while((bs->cur_byte_idx >= 24) && (bs->cur_byte_idx < 172));

    CurrTSData->CurrentPMT[i1]->VideoCh[j1]->Filled = true;
}

signed int HaveTSReturnPos(FILE *pFile1)
{
    unsigned char buffer[188];
    signed short SyncBytePos[4] = {-1,};
    int nRead = 0;
    unsigned int i1 = 0;

    do
    {
        nRead += fread(buffer,1,188,pFile1);

        for (int j = 0; j < 188; j++)
        {
            if (buffer[j] == 0x47) 
            {
                SyncBytePos[i1++] = j;
                break;
            }
        }

        if ((SyncBytePos[i1] != -1) && (i1 == 4))
        {
            if (SyncBytePos[i1-1]==SyncBytePos[i1-2] && SyncBytePos[i1-2]==SyncBytePos[i1-3] && SyncBytePos[i1-3]==SyncBytePos[i1-4]) 
            {
                //fclose (pFile1);
                return SyncBytePos[i1-1] + nRead - 752;// 4 * 188 = 752
            }
        }
    }while (pFile1 != NULL);//( nRead > 0 );

    fclose (pFile1);
    return -1;
}

TSData* ProcessTSData(const char *TSFileName)
{
    ts_bit_stream bs;
    unsigned char buf[size_packet];
    int TSPacketPos = 0;
    int nRead = 0;
    int TSDataProcStep = 1;
    int tmpCounter = 0;
    int count = 0;
    int StartIndicator = 0;
    
    FILE *pFile = fopen (TSFileName, "rb");

    TSPacketPos = HaveTSReturnPos(pFile);

    if (TSPacketPos == -1)
    {
        //"This is not a transport stream file!"
        return NULL;
    } else
    {   
        TSData *CurrentTSData = (TSData *) malloc(sizeof(TSData));
        memset(CurrentTSData,0,sizeof(TSData));

        fseek (pFile , TSPacketPos , SEEK_SET);   

        do
        {
            nRead = fread (buf, 1, size_packet, pFile);
            bs.cur_byte_ptr = buf;
            bs.cur_bit_idx = 6;  
            bs.cur_byte_idx = 0;

            count++;
 
            if (*bs.cur_byte_ptr == 0x47)
            {
                bs.cur_byte_ptr++;
                bs.cur_byte_idx++;
                
                if (ExtractDataBitsN(&bs, 1)) StartIndicator = 1;
                skip_n_bits(&bs, 1);

                int tmpPID = ExtractDataBitsN(&bs, 13);
                
                if ((tmpPID == 0) & (TSDataProcStep == 1)) 
                {
                    ProcessPATData(&bs, CurrentTSData); //Add Program Association Table (PID 0)
                    if (CurrentTSData->CountPrg) TSDataProcStep = 2;
                } else if (TSDataProcStep == 2)
                {
                    for (int i = 0; i < CurrentTSData->CountPrg; i++)
                    {
                        if ((tmpPID == CurrentTSData->CurrentPMT[i]->pr_PID) & !CurrentTSData->CurrentPMT[i]->CountPrgCh)  
                        { 
                            ProcessPMTData(i, &bs, CurrentTSData); //Add Program MAP Tables
                            tmpCounter++;
                        } else if (tmpCounter == CurrentTSData->CountPrg)
                        {
                            fseek (pFile , TSPacketPos , SEEK_SET);
                            count = 0;
                            TSDataProcStep = 3;
                            tmpCounter = 0;
                            StartIndicator = 0;
                        }
                        
                    }
                }else if ((TSDataProcStep == 3) & StartIndicator)
                {       
                    int tmpAVID, tmpPESLength;

                    StartIndicator = 0;

                    skip_n_bits(&bs, 2);

                    if (ExtractDataBitsN(&bs, 2) == 3) 
                    {
                        skip_n_bits(&bs, 4);
                        skip_n_bits(&bs, ExtractDataBitsN(&bs, 8)*8);
                        skip_n_bits(&bs, 24);
                    }else skip_n_bits(&bs, 28);

                    tmpAVID = ExtractDataBitsN(&bs, 8);

                    skip_n_bits(&bs, 32);

                    tmpPESLength =  ExtractDataBitsN(&bs, 8);
                    if (tmpPESLength < 65535) 
                    {
                        bs.cur_byte_ptr = bs.cur_byte_ptr + tmpPESLength;
                        bs.cur_byte_idx  = bs.cur_byte_idx + tmpPESLength;
                    }
                    
                    if ((tmpAVID & 224) == 192) tmpPESLength = ExtractDataBitsN(&bs, 12);
                    else tmpPESLength = ExtractDataBitsN(&bs, 32);

                    if (((tmpAVID & 224) == 192) & (tmpPESLength == 4095)) //id_cw_audio
                    {                        
                        for (int i = 0; i < CurrentTSData->CountPrg; i++)
                        {      
                            for (int j = 0; j < CurrentTSData->CurrentPMT[i]->CountAudioCh; j++)
                            {
                                if ((tmpPID == CurrentTSData->CurrentPMT[i]->AudioCh[j]->ch_PID) & !CurrentTSData->CurrentPMT[i]->AudioCh[j]->Filled)
                                {
                                    CurrentTSData->CurrentPMT[i]->AudioCh[j]->AudioStartOffset = ftell(pFile);
                                    AddPESSectionAudio(&bs, i, j, CurrentTSData); //Add Audio Channels
                                }
                            }
                        }
                    } else if ((((tmpAVID & 224) == 224) & (tmpPESLength == 435)) || (((tmpAVID & 224) == 224) & (tmpPESLength == 1)))
                    {    
                        int i = 0;
                        if (tmpPESLength == 1) 
                        {
                            if (ExtractDataBitsN(&bs, 8)!= 179) i = 257;
                        }else skip_n_bits(&bs, 8);

                        for (i; i < CurrentTSData->CountPrg; i++)
                        {     
                            for (int j = 0; j < CurrentTSData->CurrentPMT[i]->CountVideoCh; j++)
                            {
                                tmpPESLength = 0;
                                if ((tmpPID == CurrentTSData->CurrentPMT[i]->VideoCh[j]->ch_PID) & !CurrentTSData->CurrentPMT[i]->VideoCh[j]->Filled)
                                {
                                    CurrentTSData->CurrentPMT[i]->VideoCh[j]->VideoStartOffset = ftell(pFile);
                                    AddPESSectionVideo(&bs, i, j, CurrentTSData); //Add Video Channels
                                }
                            }
                        }
                    }                                  
                }
            }
        }while (nRead > 0);
        fclose(pFile);
        return CurrentTSData;
    }
}

void ExtractESF_PES_ES(const char *TSFileNameIn, const char *TSFileNameOut, int iPID, int evtID)
{
    unsigned char buf[size_packet];
    unsigned char bufshift[size_packet];
    int nRead = 0;
    int frame_num = 0;
    int frame_num_video = 0;
    int frl = 0;
    int sz = 0;
    int d = 1;
    int suma = 0;

    FILE *pFileIn = fopen (TSFileNameIn, "rb");
    fseek (pFileIn , 0 , SEEK_SET);
    FILE *pFileOut = fopen (TSFileNameOut, "wb");
    FILE *pFileCompare;

    if (!pFileIn)
    {
        fprintf(stderr, "Can't read from file %s\n", TSFileNameIn);
    }else if (!pFileOut)
    {
        fprintf(stderr, "Can't write in file %s\n", TSFileNameOut);
    }else
    {
         do
         {
             nRead = fread(buf,1, size_packet, pFileIn);
        
             if (buf[0] == 0x47)
             {
                 frame_num++;

                 int tmpPID = ExtractDataBits(4, 13, buf, 1);
                 if ((tmpPID == iPID)&&1)
                 {
                     frame_num_video++;

                    if (frame_num == 339336) 
                        d = 1;
                     if (evtID == 1017)
                     {
                         fwrite (buf , 1 , sizeof(buf), pFileOut);
                     }else 
                     {
                         pFileCompare = fopen (TSFileNameCompare, "at");
                         char bufshiftComp[10] = {0,};

                         if (ExtractDataBits(6, 1, buf, 1))
                         {                          
                             frl = ExtractDataBits(7, 8, buf, 12);
                             if (ExtractDataBits(7, 8, buf, 12) == 10)
                             {
                                unsigned char *bufshift1 = &buf[23];

                                if (d) fwrite (bufshift1 , 1 , sizeof(buf)-23, pFileOut);
                                sz += (188-23);

                                int tmp = 188-23;
                                suma = tmp;
                                itoa(tmp, bufshiftComp, 10);
                                if (d) fwrite ( bufshiftComp, 1 , 4, pFileCompare);
                                fclose(pFileCompare);
                             }else if (ExtractDataBits(7, 8, buf, 12) == 5)
                             {
                                unsigned char *bufshift1 = &buf[18];

                                if (d) fwrite (bufshift1 , 1 , sizeof(buf)-18, pFileOut);
                                sz += (188-18);

                                int tmp = 188-18;
                                suma = tmp;
                                itoa(tmp, bufshiftComp, 10);
                                if (d) fwrite ( bufshiftComp, 1 , 4, pFileCompare);
                                fclose(pFileCompare);
                             }
                                
                         }else if (ExtractDataBits(5, 1, buf, 3))
                         {
                             frl = ExtractDataBits(7, 8, buf, 4);
                             unsigned char *bufshift1 = &buf[frl+5];
                             if (d) fwrite (bufshift1 , 1 , sizeof(buf) - (frl+5), pFileOut);
                             sz += (188-(frl+5));

                             int tmp = 188-(frl+5);

                             if (tmp == 133) d = 1;
                             itoa(tmp, bufshiftComp, 10);
                             if (d) fwrite ( bufshiftComp, 1 , 4, pFileCompare);
                             fclose(pFileCompare);
                             suma += tmp;
                             suma = 0;
                         }else
                         {
                             unsigned char *bufshift1 = &buf[4];
                             if (d) fwrite (bufshift1 , 1 , sizeof(buf)-4, pFileOut);
                             sz += (188-4);

                             int tmp = 188-4;
                             suma += tmp;
                             itoa(tmp, bufshiftComp, 10);
                             if (d) fwrite ( bufshiftComp, 1 , 4, pFileCompare);
                             fclose(pFileCompare);
                         }
                     }
                 }
             }
         }while (nRead > 0);
        
         fclose(pFileIn);
         fclose(pFileOut);
         fclose(pFileCompare);
    }
}

void ExtractESF_PCM(const char *TSFileNameIn, const char *TSFilesDirOut, int iPID)
{
    unsigned char buf[size_packet];
    unsigned char bufshift[size_packet];
    int nRead = 0;
    char pfoutL[256] = {};
    char pfoutR[256] = {};
    char pfoutTmp[256] = {};
    strcat(pfoutL, TSFilesDirOut);
    strcat(pfoutL, "\\L_ch.wav");
    strcat(pfoutR, TSFilesDirOut);
    strcat(pfoutR, "\\R_ch.wav");
    strcat(pfoutTmp, TSFilesDirOut);
    strcat(pfoutTmp, "\\tmp.bin");


    FILE *pFileIn = fopen (TSFileNameIn, "rb");
    FILE *pFileTmpOut = fopen (pfoutTmp, "wb");

    if (!pFileIn)
    {
        fprintf(stderr, "Can't read from file %s\n", TSFileNameIn);
    }else if (!pFileTmpOut)
    {
        fprintf(stderr, "Can't write in temp file %s\n", pFileTmpOut);
    }else
    {
         do
         {
             nRead = fread(buf,1, size_packet, pFileIn);
        
             if (buf[0] == 0x47)
             {
                 int tmpPID = ExtractDataBits(4, 13, buf, 1);
                 if (tmpPID == iPID)
                 {
                     if (ExtractDataBits(6, 1, buf, 1))
                     {
                         unsigned char *bufshift1 = &buf[18];
                         fwrite (bufshift1 , 1 , sizeof(buf)-18, pFileTmpOut);
                     }else if (ExtractDataBits(5, 1, buf, 3))
                     {
                         unsigned char *bufshift1 = &buf[126];
                         fwrite (bufshift1 , 1 , sizeof(buf)-126, pFileTmpOut);
                     }else
                     {
                         unsigned char *bufshift1 = &buf[4];
                         fwrite (bufshift1 , 1 , sizeof(buf)-4, pFileTmpOut);
                     }
                 }
             }
         }while (nRead > 0);
        
         fclose(pFileIn);
         fclose(pFileTmpOut);

         MPALayer2Decoder(pfoutTmp, pfoutL, pfoutR);

         remove(pfoutTmp);
    }
}
