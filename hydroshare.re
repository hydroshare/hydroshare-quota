
#
# The second parameter is always the absoluted path of the "bags" directory
# e.g. /hydroshareZone/home/cuahsiDataProxy/bags => from "bags"'s path we could analyze to have:
#      1. Where is used to store HS users usage: /hydroshareZone/home/cuahsiDataProxy/bags
#      2. The HydroShare Root Dir: /hydroshareZone/home/cuahsiDataProxy
#      3. The iRODS home directory for all local users: /hydroshareZone/home
#      4. All others directory inside iRODS home directory are needed to scan
#
# The last parameter is server role which is used to determine the iRODS server is HydroShare Data Sever or Federated Server
#      1. If its value is "HydroShare" => the server acts as the HydroShare Data Server and micro-services only take care files on HydroShare Root Dir
#      2. Otherwise, the server's role is a Federated Server and micro-services will take care files on the iRODS home directory
##

# catch "irm" command
acDataDeletePolicy () {
    *logicalPath = $objPath;
    *quotaOwner = "";
    *collName = "";
    *dataName = "";
    *dataSize = "";
    writeLine("serverLog", "=== Within acDataDeletePolicy for [*logicalPath].");
    getQuotaOwner(*logicalPath, *quotaOwner);
    writeLine("serverLog", "The quota owner is [*quotaOwner].");
    msiSplitPath(*logicalPath, *collName, *dataName)
    writeLine("serverLog", "The collection name is [*collName].");
    writeLine("serverLog", "The data object name is [*dataName].");
    foreach (*row in select DATA_SIZE where COLL_NAME = "*collName" and DATA_NAME = "*dataName" and DATA_REPL_STATUS = "1") {
        *dataSize = *row.DATA_SIZE;
    }
    writeLine("serverLog", "The data object size is [*dataSize].");
    *ownerAndSize = *quotaOwner ++ ";" ++ *dataSize
    writeLine("serverLog", "Combined quota owner and data size is [*ownerAndSize].");
    delay("<PLUSET>5s</PLUSET><INST_NAME>irods_rule_engine_plugin-irods_rule_language-instance</INST_NAME>") {
        msiHSRemoveFile(*logicalPath, "/hydroshareZone/home/cuahsiDataProxy/bags", "quotaUserName", "Federated", "hsuser", "dummy", "local.hs.org/hsapi/_internal/update_quota_usage/", *ownerAndSize);
    }
    writeLine("serverLog", "=== Exiting acDataDeletePolicy after placing msiHSRemoveFile on the delay queue.");
}

# catch "icp" command
acPostProcForCopy () {
    msiHSAddNewFile($objPath, "/hydroshareZone/home/cuahsiDataProxy/bags", "quotaUserName", "Federated", "hsuser", "dummy", "local.hs.org/hsapi/_internal/update_quota_usage/"); 
}

# catch "imv" command 
#acPostProcForObjRename (*sourceObject, *destObject) {
#    msiHSRemoveFile(*sourceObject, "/hydroshareZone/home/cuahsiDataProxy/bags", "quotaUserName", "Federated", "hsuser", "dummy", "local.hs.org/hsapi/_internal/update_quota_usage/");
#    msiHSAddNewFile(*destObject,   "/hydroshareZone/home/cuahsiDataProxy/bags", "quotaUserName", "Federated", "hsuser", "dummy", "local.hs.org/hsapi/_internal/update_quota_usage/");
#}

# catch "iput" command
acPostProcForPut () {
    # work around to not call msiHSAddNewFile in the case of irepl ($oprType==6) and only call it in the case of iput ($oprType==1)
    if ($oprType == 1)
    then {
        msiHSAddNewFile($objPath, "/hydroshareZone/home/cuahsiDataProxy/bags", "quotaUserName", "Federated", "hsuser", "dummy", "local.hs.org/hsapi/_internal/update_quota_usage/"); 
    }
}

# catch "imeta" command if any new "quotaUserName" or "resetQuotaDir" are set
acPostProcForModifyAVUMetadata (*cmd, *objType, *objPath, *avuName, *avuValue, *avuUnit) {
    # set a new quotaHolder to a Collection: will affect to all COLL inside HydroShare Root Dir
    if (((*cmd == "set") || (*cmd == "add")) && (*objType == "-C") && (*avuName == "quotaUserName"))
    then {
        msiHSAddQuotaHolder(*objPath, "/hydroshareZone/home/cuahsiDataProxy/bags", *avuValue, "Federated", "hsuser", "dummy", "local.hs.org/hsapi/_internal/update_quota_usage/");
    }

    # could be set in anywhere, in any object. 
    # micro-services will scan from the HydroShare Root Dir or the iRODS home directory depend on the last parameter is "HydroShare" or other value.
    if (((*cmd == "set")  || (*cmd == "add")) && (*objType == "-C") && (*avuName == "resetQuotaDir"))
    then {
        msiHSResetQuotaDir(*objPath, "/hydroshareZone/home/cuahsiDataProxy/bags", "quotaUserName", "Federated", "hsuser", "dummy", "local.hs.org/hsapi/_internal/update_quota_usage/");
    }

}

# catch "imeta" command if any "quotaUserName" AVU is removed
acPreProcForModifyAVUMetadata (*cmd, *objType, *objPath, *avuName, *avuValue, *avuUnit) {
    if ((*cmd == "rm") && (*objType == "-C") && (*avuName == "quotaUserName"))
    then {
        msiHSRemoveQuotaHolder(*objPath, "/hydroshareZone/home/cuahsiDataProxy/bags", *avuValue, "Federated", "hsuser", "dummy", "local.hs.org/hsapi/_internal/update_quota_usage/");
    }
}

