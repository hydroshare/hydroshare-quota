
acDataDeletePolicy () {
    msiHSRemoveFile($objPath, "/hydroshareZone/home", "quotaUserName"); 
}

acPostProcForCopy () {
    msiHSAddNewFile($objPath, "/hydroshareZone/home", "quotaUserName"); 
}

acPostProcForPut () {
    msiHSAddNewFile($objPath, "/hydroshareZone/home", "quotaUserName"); 
}

acPostProcForModifyAVUMetadata (*cmd, *objType, *objPath, *avuName, *avuValue, *avuUnit) {
    if (((*cmd == "set") || (*cmd == "add")) && (*objType == "-C") && (*avuName == "quotaUserName"))
    then {
        msiHSAddQuotaHolder(*objPath, "/hydroshareZone/home", *avuValue);
    }
    if (((*cmd == "set")  || (*cmd == "add")) && (*objType == "-C") && (*avuName == "resetQuotaDir"))
    then {
        msiHSResetQuotaDir(*objPath, "/hydroshareZone/home", "quotaUserName");
    }

}

acPreProcForModifyAVUMetadata (*cmd, *objType, *objPath, *avuName, *avuValue, *avuUnit) {
    if ((*cmd == "rm") && (*objType == "-C") && (*avuName == "quotaUserName"))
    then {
        msiHSRemoveQuotaHolder(*objPath, "/hydroshareZone/home", *avuValue);
    }
}

