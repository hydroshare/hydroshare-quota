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
                    msParam_t* _string_param8,
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

    if (!_string_param8) {
        rodsLog(LOG_ERROR, "%s: missing quota holder username", __func__);
        return SYS_INVALID_INPUT_PARAM;
    }

    char* payload = parseMspForStr(_string_param8);
    if (!payload) {
        rodsLog(LOG_ERROR, "%s: could not extract quota holder username and data size from MsParam", __func__);
        return SYS_INVALID_INPUT_PARAM;
    }

    const std::string quota_holder_and_data_size = payload;
    if (quota_holder_and_data_size.empty()) {
        rodsLog(LOG_ERROR, "%s: missing quota holder username and data size. string is empty", __func__);
        return SYS_INVALID_INPUT_PARAM;
    }

    const auto semicolon_pos = quota_holder_and_data_size.find(';');
    if (semicolon_pos == std::string::npos) {
        rodsLog(LOG_ERROR, "%s: missing semicolon delimiter in final input argument. expected [<quota_holder_username>;<data_size>]", __func__);
        return SYS_INVALID_INPUT_PARAM;
    }

    const auto data_size_pos = semicolon_pos + 1;
    if (data_size_pos == std::string::npos) {
        rodsLog(LOG_ERROR, "%s: missing data size in final input argument. expected [<quota_holder_username>;<data_size>]", __func__);
        return SYS_INVALID_INPUT_PARAM;
    }

    auto quotaHolder = quota_holder_and_data_size.substr(0, semicolon_pos);
    if (quotaHolder.empty()) {
        rodsLog(LOG_ERROR, "%s: quota holder username is empty", __func__);
        return SYS_INVALID_INPUT_PARAM;
    }

    auto dataSize = quota_holder_and_data_size.substr(data_size_pos);
    if (dataSize.empty()) {
        rodsLog(LOG_ERROR, "%s: data size is empty", __func__);
        return SYS_INVALID_INPUT_PARAM;
    }

    long long data_size = 0;
    try {
        data_size = std::stoll(dataSize);
    }
    catch (const std::exception& e) {
        rodsLog(LOG_ERROR, "%s: could not parse data size string [%s] into integer", __func__, dataSize.c_str());
        return SYS_INVALID_INPUT_PARAM;
    }

    rodsOpen();

    if (strstr(filePath, rootDir) == filePath) {
        result = decreaseUsage(data_size, bagsPath, &quotaHolder[0]);
        callRestAPI(user, pass, concat(concat(concat(concat(concat(concat(concat("https://", user), ":"), pass), "@"), url), quotaHolder.c_str()), "/"));
    }
    else {
        char *tmp = strpart(filePath, "/", 4);
        strcpy(&quotaHolder[0], tmp); delete[] tmp;
        result = decreaseUsage(data_size, bagsPath, &quotaHolder[0]);
        callRestAPI(user, pass, concat(concat(concat(concat(concat(concat(concat("https://", user), ":"), pass), "@"), url), quotaHolder.c_str()), "/"));
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
    irods::ms_table_entry* msvc = new irods::ms_table_entry(8);
    msvc->add_operation<
        msParam_t*,
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
                             msParam_t*,
                             ruleExecInfo_t*)>(msiHSRemoveFile));
    return msvc;
}

