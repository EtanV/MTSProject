#include "MPALayer2Decoder.h"
#define size_packet 188

typedef struct TS_BIT_STREAM_tag{
    unsigned char   *cur_byte_ptr;   
    short cur_byte_idx;
    short  cur_bit_idx;       			       
} ts_bit_stream;

struct Video
{
    char* ch_name;
    short unsigned int ch_PID;
    short unsigned int ES_info_length;
    short unsigned int horizontal_size_value;
    short unsigned int vertical_size_value;
    short unsigned int aspect_ratio_information;
    char* frame_rate_code;
    unsigned int bit_rate_value;
    bool marker_bit;
    short unsigned int vbv_buffer_size_value;
    bool constrained_parameters_flag;
    bool load_intra_quantiser_matrix;
    bool load_non_intra_quantiser_matrix;
    bool VideoSequenceExtensionFilled;
    bool VideoSequenceDisplayExtensionFilled;
    char* profile_indication;
    char* level_indication;
    bool progressive_sequence;
    char* chroma_format;
    unsigned int horizontal_size_extension;
    unsigned int vertical_size_extension;
    unsigned int bit_rate_extension;
    bool marker_bit_se;
    unsigned int vbv_buffer_size_extension;
    bool low_delay;
    unsigned int frame_rate_extension_n;
    unsigned int frame_rate_extension_d;
    unsigned int video_format;
    bool colour_description;
    unsigned int colour_primaries;
    unsigned int transfer_characteristics;
    unsigned int matrix_coefficients;
    unsigned int display_horizontal_size;
    bool marker_bit_sde;
    unsigned int display_vertical_size;
    long VideoStartOffset;
    bool Filled;
} ;

typedef struct Video Video;

struct Audio
{
    char* ch_name;
    short unsigned int ch_PID;
    short unsigned int ES_info_length;
    char* ID;
    char* Layer;
    bool protection_bit;
    char* bitrate;
    char* sampling_frequency;
    bool padding_bit;
    char* mode;
    char* mode_extension;
    bool copyright;
    bool original;
    char* emphasis;
    long AudioStartOffset;
    bool Filled;
    short cCh;
};

typedef struct Audio Audio;

struct OtherType
{
    char* ch_name;
    short unsigned int ch_PID;
    short unsigned int ES_info_length;
    bool Filled;
};

typedef struct OtherType OtherType;

 struct PMT //PMT Part
{
    short unsigned int pr_num;
    short unsigned int pr_PID;
    short int table_id;
    bool section_syntax_indicator;
    short unsigned int section_length;
    short unsigned int program_number;
    short int version_number;
    bool current_next_indicator;
    short int section_number;
    short int last_section_number;
    short unsigned int PCR_PID;
    short unsigned int program_info_length;
    struct Video  **VideoCh;
    struct Audio  **AudioCh;
    struct OtherType  **OtherCh;
    short int CountPrgCh;
    unsigned short CountVideoCh;
    unsigned short CountAudioCh;
    short int CountOtherCh;
};

typedef struct PMT PMT;

struct TSData //PAT Part
{
	  FILE *pFile;
    int TSPacketPosStart; 
    short int table_id;
    bool section_syntax_indicator;
    short unsigned int section_length;
    short unsigned int transport_stream_id;
    short int version_number;
    bool current_next_indicator;
    short int section_number;
    short int last_section_number;
    unsigned long CountPrg;
    struct PMT  **CurrentPMT;
    short int CountAVOCh;
};

typedef struct TSData TSData;

struct pESF //PAT Part
{
    int tmp_1;
    FILE* pFile[];
};

TSData* ProcessTSData(const char *TSFileName);
void ExtractESF_PES_ES(const char *TSFileNameIn, const char *TSFileNameOut, int iPID, int evtID);
void ExtractESF_PCM(const char *TSFileNameIn, const char *TSFilesDirOut, int iPID);
int ExtractDataBits(int startbit, int countbits, unsigned char bufPAT[], int startbyte);
void MPALayer2Decoder(char *pAF, char *pLch, char *pRch);
signed int HaveTSReturnPos(FILE *pFile);
void ProcessPATData(unsigned char buf_frm[], TSData *CurrTSData);
void ProcessPMTData(int p, unsigned char buf_frm[], TSData *CurrTSData);
void AddPESSectionAudio(ts_bit_stream *bs, int i1, int j1, TSData *CurrTSData);
void AddPESSectionVideo(ts_bit_stream *bs, int i1, int j1, TSData *CurrTSData);
char* PMTStream_type(int data);
char* ESAudioBitrate(int layer, int bit_rate_index);
char* FFrame_rate_code(int data);
void VideoSequenceExtension(ts_bit_stream *bs, int i2, int j2, TSData *CurrTSData2);
void VideoSequenceDisplayExtension(ts_bit_stream *bs, int i2, int j2, TSData *CurrTSData2);