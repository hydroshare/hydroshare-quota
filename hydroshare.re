
acDataDeletePolicy () {
    msiHSRemoveFile($objPath, "cuahsi2UserProxy#hydroshareuserZone", "quotaUserName"); 
}

acPostProcForCopy () {
    msiHSAddNewFile($objPath, "cuahsi2UserProxy#hydroshareuserZone", "quotaUserName"); 
}

acPostProcForPut () {
    msiHSAddNewFile($objPath, "cuahsi2UserProxy#hydroshareuserZone", "quotaUserName"); 
}

acPostProcForModifyAVUMetadata (*cmd, *objType, *objPath, *avuName, *avuValue, *avuUnit) {
    if ((*cmd == "set") && (*objType == "-d") && (*avuName == "quotaUserName"))
    then {
        msiHSAddQuotaHolder(*objPath, "cuahsi2UserProxy#hydroshareuserZone", *avuValue);
    }
    if (((*cmd == "set")  || (*cmd == "add")) && (*objType == "-C") && (*avuName == "resetQuotaDir"))
    then {
        msiHSResetQuotaDir(*objPath, "cuahsi2UserProxy#hydroshareuserZone", "quotaUserName");
    }

}

acPreProcForModifyAVUMetadata (*cmd, *objType, *objPath, *avuName, *avuValue, *avuUnit) {
    if ((*cmd == "rm") && (*objType == "-d") && (*avuName == "quotaUserName"))
    then {
        msiHSRemoveQuotaHolder(*objPath, "cuahsi2UserProxy#hydroshareuserZone", *avuValue);
    }
}

