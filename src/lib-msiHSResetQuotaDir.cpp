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

int msiHSResetQuotaDir(msParam_t* _string_param, 
                       msParam_t* _string_param2, 
                       msParam_t* _string_param3, ruleExecInfo_t* _rei ) {

    char *hydroshareRootPath = parseMspForStr( _string_param );
    if( !hydroshareRootPath ) {
        std::cout << "null PATH" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    char *rootDir = parseMspForStr( _string_param2 );
    if( !rootDir ) {
        std::cout << "null rootDir" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    char *pos = strstr(hydroshareRootPath, rootDir);
    if ((pos == NULL) || (pos != hydroshareRootPath)) {
        rodsLog(LOG_ERROR, "msiHSRemoteFile: ignore %s: out of root: %s", hydroshareRootPath, rootDir);
        return 0;
    }

    char *rodsUser = concat(strpart(hydroshareRootPath, "/", 4), concat("#", strpart(hydroshareRootPath, "/", 2)));

    char *quotaHolderAVU = parseMspForStr( _string_param3 );
    if( !quotaHolderAVU ) {
        std::cout << "null quotaHolder AVU" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    rodsOpen();

    resetRootDir (hydroshareRootPath, rodsUser, quotaHolderAVU);
    reScanRootDir(hydroshareRootPath, rodsUser, quotaHolderAVU);

    rodsClose();

    return 0;
}

//---------------------------------------------------------

extern "C"
irods::ms_table_entry* plugin_factory() {
    irods::ms_table_entry* msvc = new irods::ms_table_entry(3);
    msvc->add_operation<
        msParam_t*,
        msParam_t*,
        msParam_t*,
        ruleExecInfo_t*>("msiHSResetQuotaDir",
                         std::function<int(
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             ruleExecInfo_t*)>(msiHSResetQuotaDir));
    return msvc;
}

