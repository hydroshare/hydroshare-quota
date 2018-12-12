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

#include <curl/curl.h>
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

#include <string>
#include <libgen.h>

//---------------------------------------------------------

int msiHSRemoveFile(msParam_t* _string_param, 
                    msParam_t* _string_param2, 
                    msParam_t* _string_param3, 
                    msParam_t* _string_param4, 
                    msParam_t* _string_param5,
                    msParam_t* _string_param6,
                    msParam_t* _string_param7,
                    ruleExecInfo_t* _rei ) {

    char *filePath;
    char *bagsPath;
    char *quotaHolderAVU;
    char *serverRole;
    char *irodsDir;
    char *rootDir;
    char *user;
    char *pass;
    char *url;

    int result = paramCheck(_string_param,
                            _string_param2,
                            _string_param3,
                            _string_param4,
                            _string_param5,
                            _string_param6,
                            _string_param7,
                            &filePath, &bagsPath, &quotaHolderAVU, &serverRole, &irodsDir, &rootDir, &user, &pass, &url);

    if (result != 0) {
        return result;
    }

    char quotaHolder[MAX_NAME_LEN];

    rodsOpen();

    if (strstr(filePath, rootDir) == filePath) {
        bool haveQuotaHolder = getParentQuotaHolder(filePath, quotaHolderAVU, quotaHolder);

        if (haveQuotaHolder) {
            result = decreaseUsage(filePath, bagsPath, quotaHolder);
            callRestAPI(user, pass, concat(url, quotaHolder));
        }
        else {
            rodsLog(LOG_ERROR, "msiHSRemoteFile: file %s has no quota Holder", filePath);
        }
    }
    else {
        char *tmp = strpart(filePath, "/", 4);
        strcpy(quotaHolder, tmp); delete[] tmp;
        result = decreaseUsage(filePath, bagsPath, quotaHolder);
        callRestAPI(user, pass, concat(url, quotaHolder));
    }

    rodsClose();
/*
    delete[] bags;
    delete[] rootDir;
    delete[] filePath;
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
        msParam_t*,
        msParam_t*,
        msParam_t*,
        ruleExecInfo_t*>("msiHSRemoveFile",
                         std::function<int(
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             ruleExecInfo_t*)>(msiHSRemoveFile));
    return msvc;
}

