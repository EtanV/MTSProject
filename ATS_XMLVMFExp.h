#include <wx/treectrl.h>
//#include "ATSProjectFrm.h"
//#include "ProcessTSData.h"

/*#include <sc_common.h>
#include <sc_io_types.h>
#include <sc_io.h>
#include <sc_memory.h>
#include <sc_byteorder.h>
#include <sc_log.h>

#include "md_common.h"
#include "flavour_calculators.h"
#include "vmf_def.h"
#include "md_writer_api.h"
#include "help_writer_functions.h"*/

#include "HelpFunctionsTSParser.h"

static int XMLinit = 0;

class ATS_XMLExp
{
    public:
        static void XMLExport(wxTreeCtrl *wxTreeCtrl22, wxTreeItemId Level0, wxString selected, FILE *fp);
        static void XMLExportSubLevels(wxTreeCtrl *WxTreeCtrl22 , wxTreeItemId Level5, FILE *fp);
        static wxString Tag_Value(wxString *tag2);
};