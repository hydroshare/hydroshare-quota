// =-=-=-=-=-=-=-
// irods includes
#include "irods_error.hpp"
#include "irods_ms_plugin.hpp"

#include "rodsClient.h"
#include "parseCommandLine.h"
#include "rodsPath.h"
#include "lsUtil.h"

#include "miscUtil.h"
#include "rcMisc.h"
#include "rodsErrorTable.h"
#include "rodsLog.h"
#ifdef windows_platform
#include "irodsntutil.hpp"
#endif

#include "apiHandler.hpp"
#include "irods_plugin_base.hpp"
#include "irods_re_plugin.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include "irods_buffer_encryption.hpp"
#include "irods_client_api_table.hpp"
#include "irods_pack_table.hpp"

#include "rcGlobalExtern.h"

//---------------------------------------------------------

#include "common.cpp"

//---------------------------------------------------------


#include <libgen.h>

//---------------------------------------------------------

int msiHSAddNewFile(msParam_t* _string_param, 
                    msParam_t* _string_param2, 
                    msParam_t* _string_param3,
                    msParam_t* _string_param4,
                    ruleExecInfo_t* _rei ) {

    char *newPath;
    char *bagsPath;
    char *quotaHolderAVU;
    char *serverRole;
    char *irodsDir;
    char *rootDir;

    int result = paramCheck(_string_param, _string_param2, _string_param3, _string_param4, &newPath, &bagsPath, &quotaHolderAVU, &serverRole, &irodsDir, &rootDir);
    if (result != 0) {
	return result;
    }

    char quotaHolder[MAX_NAME_LEN];

    rodsOpen();

    if (strstr(newPath, rootDir) == newPath) {
        bool haveQuotaHolder = getParentQuotaHolder(newPath, quotaHolderAVU, quotaHolder);

        if (haveQuotaHolder) {
            result = increaseUsage(newPath, bagsPath, quotaHolder);
        }
        else {
            rodsLog(LOG_ERROR, "msiHSAddNewFile: file %s has no quotaHolder", newPath);
        }
    }
    else {
        char *tmp = strpart(newPath, "/", 4);
        strcpy(quotaHolder, tmp); delete[] tmp;
        result = increaseUsage(newPath, bagsPath, quotaHolder);
    }

    rodsClose();
/*
    delete[] bags;
    delete[] rootDir;
    delete[] newPath;
    delete[] quotaHolderAVU;
*/
    return result;
}

//---------------------------------------------------------

extern "C"
irods::ms_table_entry* plugin_factory() {
    irods::ms_table_entry* msvc = new irods::ms_table_entry(4);
    msvc->add_operation<
        msParam_t*,
        msParam_t*,
        msParam_t*,
        msParam_t*,
        ruleExecInfo_t*>("msiHSAddNewFile",
                         std::function<int(
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             ruleExecInfo_t*)>(msiHSAddNewFile));
    return msvc;
}

