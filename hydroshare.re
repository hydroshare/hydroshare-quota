
acDataDeletePolicy () {
    msiHSRemoveFile($objPath, "root#hydroshareZone", "quotaUserName"); 
}

acPostProcForCopy () {
    msiHSAddNewFile($objPath, "root#hydroshareZone", "quotaUserName"); 
}

acPostProcForPut () {
    msiHSAddNewFile($objPath, "root#hydroshareZone", "quotaUserName"); 
}

acPostProcForModifyAVUMetadata (*cmd, *objType, *objPath, *avuName, *avuValue, *avuUnit) {
    if ((*cmd == "set") && (*objType == "-d") && (*avuName == "quotaUserName"))
    then {
        msiHSAddQuotaHolder(*objPath, "root#hydroshareZone", *avuValue);
    }
    if (((*cmd == "set")  || (*cmd == "add")) && (*objType == "-C") && (*avuName == "resetQuotaDir"))
    then {
        msiHSResetQuotaDir(*objPath, "root#hydroshareZone", "quotaUserName");
    }

}

acPreProcForModifyAVUMetadata (*cmd, *objType, *objPath, *avuName, *avuValue, *avuUnit) {
    if ((*cmd == "rm") && (*objType == "-d") && (*avuName == "quotaUserName"))
    then {
        msiHSRemoveQuotaHolder(*objPath, "root#hydroshareZone", *avuValue);
    }
}

