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

#include <string>
#include <libgen.h>

//---------------------------------------------------------

int msiHSAddNewFile(msParam_t* _string_param, 
                    msParam_t* _string_param2, 
                    msParam_t* _string_param3, 
                    ruleExecInfo_t* _rei ) {

    char *newPath = parseMspForStr( _string_param );
    if( !newPath ) {
        std::cout << "null PATH" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    char *rootDir = parseMspForStr( _string_param2 );
    if( !rootDir ) {
        std::cout << "null rootDir" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    char *pos = strstr(newPath, rootDir);
    if ((pos == NULL) || (pos != newPath)) {
        rodsLog(LOG_ERROR, "msiHSAddNewFile: ignore %s: out of root: %s", newPath, rootDir);
        return 0;
    }

//    char *rodsUser = concat(strpart(newPath, "/", 4), concat("#", strpart(newPath, "/", 2)));
    char *tmp;
    char *bags = concat("/", strpart(newPath, "/", 2));               tmp = bags;
    bags = concat(bags, "/");                           delete[] tmp; tmp = bags;
    bags = concat(bags, strpart(newPath, "/", 3));      delete[] tmp; tmp = bags;
    bags = concat(bags, "/");                           delete[] tmp; tmp = bags;
    bags = concat(bags, strpart(newPath, "/", 4));      delete[] tmp; tmp = bags;
    bags = concat(bags, "/bags");                       delete[] tmp;

    char *quotaHolderAVU = parseMspForStr( _string_param3 );
    if( !quotaHolderAVU ) {
        std::cout << "null quotaHolder AVU" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    char quotaHolder[MAX_NAME_LEN];

    rodsOpen();

    int result = 0;
    bool haveQuotaHolder = getParentQuotaHolder(newPath, quotaHolderAVU, quotaHolder);

    if (haveQuotaHolder) {
        result = increaseUsage(newPath, bags, quotaHolder);
    }
    else {
        rodsLog(LOG_ERROR, "msiHSAddNewFile: file %s has no quotaHolder", newPath);
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
    irods::ms_table_entry* msvc = new irods::ms_table_entry(3);
    msvc->add_operation<
        msParam_t*,
        msParam_t*,
        msParam_t*,
        ruleExecInfo_t*>("msiHSAddNewFile",
                         std::function<int(
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             ruleExecInfo_t*)>(msiHSAddNewFile));
    return msvc;
}

