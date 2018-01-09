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

int msiHSAddQuotaHolder(msParam_t* _string_param, 
                        msParam_t* _string_param2, 
                        msParam_t* _string_param3, 
                        ruleExecInfo_t* _rei ) {

    char *dirPath = parseMspForStr( _string_param );
    if( !dirPath ) {
        std::cout << "null FILE PATH" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    char *rootDir = parseMspForStr( _string_param2 );
    if( !rootDir ) {
        std::cout << "null rootDir" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    char *pos = strstr(dirPath, rootDir);
    if ((pos == NULL) || (pos != dirPath)) {
        rodsLog(LOG_ERROR, "msiHSAddQuotaHolder: ignore %s: out of root: %s", dirPath, rootDir);
        return 0;
    }

    char *rodsUser = concat(strpart(dirPath, "/", 4), concat("#", strpart(dirPath, "/", 2)));

    char *newOwner = parseMspForStr( _string_param3 );
    if(  !newOwner ) {
        std::cout << "null NEW OWNER" << std::endl;
        return SYS_INVALID_INPUT_PARAM;
    }

    rodsOpen();
    char *avuOwner = concat(newOwner, usageSize);

    long long dirUsage  = reScanDirUsage(dirPath);
    long long userUsage = strtoll(getUserAVU(rodsUser, avuOwner), 0, 0) + dirUsage;

    if (userUsage < 0) userUsage = 0;

    setAVU("-u", rodsUser, avuOwner, lltostr(userUsage));

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
        ruleExecInfo_t*>("msiHSAddQuotaHolder",
                         std::function<int(
                             msParam_t*,
                             msParam_t*,
                             msParam_t*,
                             ruleExecInfo_t*)>(msiHSAddQuotaHolder));
    return msvc;
}

