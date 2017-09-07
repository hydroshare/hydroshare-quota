
rcComm_t *conn;
rodsEnv myEnv;

char *EMPTY          = "   NULL   ";

char *usageSize      = "-usage";
char *usageQuota     = "-quota";

//---------------------------------------------------------

char* concat(const char *s1, const char *s2);
char * lltostr(long long num);

long long getRodsFileSize(char *srcPath);

char * getDirAVU( char *name, char *attrName);
char * getUserAVU( char *name, char *attrName);

int setAVU(char *objType, char *objName, char *attrName, char *attrValue);

void _debug(char *str) {
        std::cout << " __DEBUG__: " << str << std::endl;
}

//---------------------------------------------------------
void _debug(long long val) {
        std::cout << " __DEBUG__: " << val << std::endl;
}

//---------------------------------------------------------
long long reScanQuotaDir(char * dirPath, char * userName, char * quotaHolderAVU) {
    long long dirSize = 0;
    int status;
    int queryFlags;
    collHandle_t collHandle;
    collEnt_t collEnt;

    queryFlags = DATA_QUERY_FIRST_FG;

    status = rclOpenCollection( conn, dirPath, queryFlags, &collHandle );

    while ( ( status = rclReadCollection( conn, &collHandle, &collEnt ) ) >= 0 ) {
        if ( collEnt.objType == DATA_OBJ_T ) {
            char *t1 = concat(dirPath, "/");
            char *t2 = concat(t1, collEnt.dataName);
            dirSize = dirSize + getRodsFileSize(t2);
            setAVU("-d", t2, quotaHolderAVU, userName);
	    delete[] t2; delete[] t1;
        }
        else {
            dirSize = dirSize + reScanQuotaDir(collEnt.collName, userName, quotaHolderAVU);
        }
    }
    rclCloseCollection( &collHandle );
    return dirSize;
}

//---------------------------------------------------------
void resetRootDir(char * dirPath, char * rodsUser, char * quotaHolderAVU) {
    long long dirSize = 0;
    int status;
    int queryFlags;
    collHandle_t collHandle;
    collEnt_t collEnt;

    queryFlags = DATA_QUERY_FIRST_FG;

    char *emptySize = "0";

    status = rclOpenCollection( conn, dirPath, queryFlags, &collHandle );
    while ( ( status = rclReadCollection( conn, &collHandle, &collEnt ) ) >= 0 ) {
        if ( collEnt.objType == COLL_OBJ_T ) {
            char *userName = getDirAVU(collEnt.collName, quotaHolderAVU);
//	    std::cout << " __DEBUG__: " << collEnt.collName << ":" << userName << std::endl;
            if (strcmp(userName, EMPTY) != 0) {
                char *avuUsage = concat(userName, usageSize);
                setAVU("-u", rodsUser, avuUsage, emptySize);
                delete[] avuUsage; delete[] userName;
            }
        }
    }
    rclCloseCollection( &collHandle );
}

//---------------------------------------------------------
void reScanRootDir(char * dirPath, char * rodsUser, char * quotaHolderAVU) {
    long long dirSize = 0;
    int status;
    int queryFlags;
    collHandle_t collHandle;
    collEnt_t collEnt;

    queryFlags = DATA_QUERY_FIRST_FG;

    status = rclOpenCollection( conn, dirPath, queryFlags, &collHandle );
    while ( ( status = rclReadCollection( conn, &collHandle, &collEnt ) ) >= 0 ) {
        if ( collEnt.objType == COLL_OBJ_T ) {
            char *userName = getDirAVU(collEnt.collName, quotaHolderAVU);
            if (strcmp(userName, EMPTY) != 0) {
                char *avuUsage = concat(userName, usageSize);
                char *tmpSize  = lltostr(strtoll(getUserAVU(rodsUser, avuUsage), 0, 0) + reScanQuotaDir(collEnt.collName, userName, quotaHolderAVU));
                setAVU("-u", rodsUser, avuUsage, tmpSize);
                delete[] avuUsage; delete[] userName; delete[] tmpSize;
            }
        }
    }
    rclCloseCollection( &collHandle );
}

//---------------------------------------------------------

void rodsOpen() {

    signal( SIGPIPE, SIG_IGN );

    int status = 0;
    rErrMsg_t errMsg;

    getRodsEnv( &myEnv );

    conn = rcConnect(
               myEnv.rodsHost,
               myEnv.rodsPort,
               myEnv.rodsUserName,
               myEnv.rodsZone,
               0, &errMsg );

    if ( conn == NULL ) {
        exit( 2 );
    }

    irods::api_entry_table&  api_tbl = irods::get_client_api_table();
    irods::pack_entry_table& pk_tbl  = irods::get_pack_table();
    init_api_table( api_tbl, pk_tbl );
    if ( strcmp( myEnv.rodsUserName, PUBLIC_USER_NAME ) != 0 ) {
        status = clientLogin( conn );
        if ( status != 0 ) {
            rcDisconnect( conn );
            exit( 7 );
        }
    }

}

//---------------------------------------------------------

void rodsClose() {
    printErrorStack( conn->rError );
    rcDisconnect( conn );
}

//---------------------------------------------------------
int removeAVU(char *objType, char *objName, char *attrName, char *attrValue) {

    modAVUMetadataInp_t modAVUMetadataInp;
    int status;

    modAVUMetadataInp.arg0 = "rm";
    modAVUMetadataInp.arg1 = objType;
    modAVUMetadataInp.arg2 = objName;
    modAVUMetadataInp.arg3 = attrName;
    modAVUMetadataInp.arg4 = attrValue;
    modAVUMetadataInp.arg5 = "";
    modAVUMetadataInp.arg6 = "";
    modAVUMetadataInp.arg7 = "";
    modAVUMetadataInp.arg8 = "";
    modAVUMetadataInp.arg9 = "";

    return rcModAVUMetadata(conn, &modAVUMetadataInp);
} 

//---------------------------------------------------------
int setAVU(char *objType, char *objName, char *attrName, char *attrValue) {

    modAVUMetadataInp_t modAVUMetadataInp;
    int status;

    modAVUMetadataInp.arg0 = "set";
    modAVUMetadataInp.arg1 = objType;
    modAVUMetadataInp.arg2 = objName;
    modAVUMetadataInp.arg3 = attrName;
    modAVUMetadataInp.arg4 = attrValue;
    modAVUMetadataInp.arg5 = "";
    modAVUMetadataInp.arg6 = "";
    modAVUMetadataInp.arg7 = "";
    modAVUMetadataInp.arg8 = "";
    modAVUMetadataInp.arg9 = "";

    return rcModAVUMetadata(conn, &modAVUMetadataInp);
} 

//---------------------------------------------------------
char * getDirAVU( char *name, char *attrName) {
    genQueryOut_t *genQueryOut = NULL;
    int i1a[10];
    int i1b[10];
    int i2a[10];
    char *condVal[10];

    char fullName[MAX_NAME_LEN];
    int  status;

    genQueryInp_t genQueryInp;
    memset( &genQueryInp, 0, sizeof( genQueryInp ) );

    char *columnNames[] = {"attribute", "value", "units"};

    i1a[0] = COL_META_COLL_ATTR_NAME;
    i1b[0] = 0; /* currently unused */
    i1a[1] = COL_META_COLL_ATTR_VALUE;
    i1b[1] = 0;
    i1a[2] = COL_META_COLL_ATTR_UNITS;
    i1b[2] = 0;
    genQueryInp.selectInp.inx = i1a;
    genQueryInp.selectInp.value = i1b;
    genQueryInp.selectInp.len = 3;

    strncpy( fullName, name, MAX_NAME_LEN );

    i2a[0] = COL_COLL_NAME;
    std::string v1;
    v1 =  "='";
    v1 += fullName;
    v1 += "'";

    condVal[0] = const_cast<char*>( v1.c_str() );

    std::string v2;
    i2a[1] = COL_META_COLL_ATTR_NAME;
    v2 =  "= '";
    v2 += attrName;
    v2 += "'";

    condVal[1] = const_cast<char*>( v2.c_str() );

    genQueryInp.sqlCondInp.inx = i2a;
    genQueryInp.sqlCondInp.value = condVal;
    genQueryInp.sqlCondInp.len = 2;

    genQueryInp.maxRows = 10;
    genQueryInp.continueInx = 0;
    genQueryInp.condInput.len = 0;

    status = rcGenQuery( conn, &genQueryInp, &genQueryOut );
    if ( status == CAT_NO_ROWS_FOUND ) {
        return EMPTY;
    }

    return genQueryOut->sqlResult[1].value;
}

//---------------------------------------------------------
char * getFileAVU( char *name, char *attrName) {
    genQueryOut_t *genQueryOut = NULL;
    int i1a[10];
    int i1b[10];
    int i2a[10];
    char *condVal[10];

    char fullName[MAX_NAME_LEN];
    char myDirName[MAX_NAME_LEN];
    char myFileName[MAX_NAME_LEN];

    int  status;

    genQueryInp_t genQueryInp;
    memset( &genQueryInp, 0, sizeof( genQueryInp ) );

    char *columnNames[] = {"attribute", "value", "units"};

    i1a[0] = COL_META_DATA_ATTR_NAME;
    i1b[0] = 0; /* currently unused */
    i1a[1] = COL_META_DATA_ATTR_VALUE;
    i1b[1] = 0;
    i1a[2] = COL_META_DATA_ATTR_UNITS;
    i1b[2] = 0;
    genQueryInp.selectInp.inx = i1a;
    genQueryInp.selectInp.value = i1b;
    genQueryInp.selectInp.len = 3;

    strncpy( fullName, name, MAX_NAME_LEN );
    splitPathByKey( fullName, myDirName, MAX_NAME_LEN, myFileName, MAX_NAME_LEN, '/' );

    i2a[0] = COL_COLL_NAME;
    std::string v1;
    v1  = "='";
    v1 += myDirName;
    v1 += "'";

    condVal[0] = const_cast<char*>( v1.c_str() );

    std::string v2;
    i2a[1] = COL_DATA_NAME;
    v2  = "='";
    v2 += myFileName;
    v2 += "'";

    condVal[1] = const_cast<char*>( v2.c_str() );

    std::string v3;
    i2a[2] = COL_META_DATA_ATTR_NAME;
    v3  = "='";
    v3 += attrName;
    v3 += "'";

    condVal[2] = const_cast<char*>( v3.c_str() );

    genQueryInp.sqlCondInp.inx = i2a;
    genQueryInp.sqlCondInp.value = condVal;
    genQueryInp.sqlCondInp.len = 3;

    genQueryInp.maxRows = 10;
    genQueryInp.continueInx = 0;
    genQueryInp.condInput.len = 0;

    status = rcGenQuery( conn, &genQueryInp, &genQueryOut );
    if ( status == CAT_NO_ROWS_FOUND ) {
        return EMPTY;
    }

    return genQueryOut->sqlResult[1].value;
}

//---------------------------------------------------------
char * getUserAVU( char *name, char *attrName) {
    genQueryOut_t *genQueryOut = NULL;
    int i1a[10];
    int i1b[10];
    int i2a[10];
    char *condVal[10];

    int  status;

    char userName[NAME_LEN];
    char userZone[NAME_LEN];

    status = parseUserName( name, userName, userZone );
    if ( status ) {
        printf( "Invalid username format\n" );
        return 0;
    }
    if ( userZone[0] == '\0' ) {
        snprintf( userZone, sizeof( userZone ), "%s", myEnv.rodsZone );
    }

    genQueryInp_t genQueryInp;
    memset( &genQueryInp, 0, sizeof( genQueryInp ) );

    char *columnNames[] = {"attribute", "value", "units"};

    i1a[0] = COL_META_USER_ATTR_NAME;
    i1b[0] = 0; /* currently unused */
    i1a[1] = COL_META_USER_ATTR_VALUE;
    i1b[1] = 0;
    i1a[2] = COL_META_USER_ATTR_UNITS;
    i1b[2] = 0;
    genQueryInp.selectInp.inx = i1a;
    genQueryInp.selectInp.value = i1b;
    genQueryInp.selectInp.len = 3;

    i2a[0] = COL_USER_NAME;
    std::string v1;
    v1  = "='";
    v1 += userName;
    v1 += "'";

    condVal[0] = const_cast<char*>( v1.c_str() );

    std::string v2;
    i2a[1] = COL_USER_ZONE;
    v2  = "='";
    v2 += userZone;
    v2 += "'";

    condVal[1] = const_cast<char*>( v2.c_str() );

    std::string v3;
    i2a[2] = COL_META_USER_ATTR_NAME;
    v3  = "='";
    v3 += attrName;
    v3 += "'";

    condVal[2] = const_cast<char*>( v3.c_str() );

    genQueryInp.sqlCondInp.inx = i2a;
    genQueryInp.sqlCondInp.value = condVal;
    genQueryInp.sqlCondInp.len = 3;

    genQueryInp.maxRows = 10;
    genQueryInp.continueInx = 0;
    genQueryInp.condInput.len = 0;

    status = rcGenQuery( conn, &genQueryInp, &genQueryOut );
    if ( status == CAT_NO_ROWS_FOUND ) {
        return EMPTY;
    }

    return genQueryOut->sqlResult[1].value;
}

//---------------------------------------------------------

long long getRodsFileSize(char *srcPath) {

    int status;
    genQueryOut_t *genQueryOut = NULL;
    char myColl[MAX_NAME_LEN], myData[MAX_NAME_LEN];
    char condStr[MAX_NAME_LEN];
    int queryFlags;

    genQueryInp_t genQueryInp;
    initCondForLs( &genQueryInp );

    rodsArguments_t rodsArgs;
    memset( &rodsArgs, 0, sizeof( rodsArguments_t ) );
    rodsArgs.longOption = True;

    queryFlags = setQueryFlag( &rodsArgs );
    setQueryInpForData( queryFlags, &genQueryInp );
    genQueryInp.maxRows = MAX_SQL_ROWS;

    memset( myColl, 0, MAX_NAME_LEN );
    memset( myData, 0, MAX_NAME_LEN );

    if ( ( status = splitPathByKey(srcPath, myColl, MAX_NAME_LEN, myData, MAX_NAME_LEN, '/' ) ) < 0 ) {
        rodsLogError(LOG_ERROR, status, "rodsFileSize: splitPathByKey for %s error, status = %d", srcPath, status);
        return status;
    }

    clearInxVal( &genQueryInp.sqlCondInp );
    snprintf( condStr, MAX_NAME_LEN, "='%s'", myColl );
    addInxVal( &genQueryInp.sqlCondInp, COL_COLL_NAME, condStr );
    snprintf( condStr, MAX_NAME_LEN, "='%s'", myData );
    addInxVal( &genQueryInp.sqlCondInp, COL_DATA_NAME, condStr );

    status =  rcGenQuery( conn, &genQueryInp, &genQueryOut );

    if ( status < 0 ) {
        if ( status == CAT_NO_ROWS_FOUND ) {
            rodsLog( LOG_ERROR, "%s does not exist or user lacks access permission", srcPath );
        }
        else {
            rodsLogError( LOG_ERROR, status, "rodsFileSize: rcGenQuery error for %s", srcPath );
        }
        return status;
    }

    sqlResult_t *dataSize = 0;

    char *tmpDataId = 0;

    if ( ( dataSize = getSqlResultByInx( genQueryOut, COL_DATA_SIZE ) ) == NULL ) {
        rodsLog( LOG_ERROR, "rodsFileSize: getSqlResultByInx for COL_DATA_SIZE failed" );
        return UNMATCHED_KEY_OR_INDEX;
    }

    return strtoll(&dataSize->value[0], 0, 0);
}

//---------------------------------------------------------
char * lltostr(long long num) {
    std::string number;
    std::stringstream strstream;
    strstream << num;
    strstream >> number;
    char * writable = new char[number.size() + 1];
    std::copy(number.begin(), number.end(), writable);
    writable[number.size()] = '\0';
    return writable;
}

//---------------------------------------------------------
char* concat(const char *s1, const char *s2)
{
    char *result = (char *)malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

//---------------------------------------------------------
bool getParentQuotaHolder(char *dirPath, char * quotaHolderAVU, char * quotaHolder) {

    char myColl[MAX_NAME_LEN], myData[MAX_NAME_LEN], srcPath[MAX_NAME_LEN];

    memset( myColl, 0, MAX_NAME_LEN );
    memset( myData, 0, MAX_NAME_LEN );

    strcpy( srcPath, dirPath);

    while (true) {
        splitPathByKey(srcPath, myColl, MAX_NAME_LEN, myData, MAX_NAME_LEN, '/' );
        int cmp = strcmp(myColl, "/");
	if (cmp == 0) return false;
	char * qHolder = getDirAVU(myColl, quotaHolderAVU);
        cmp = strcmp(qHolder, EMPTY);
        if (cmp == 0) {
            strcpy(srcPath, myColl);
            continue;
        }
        strcpy(quotaHolder, qHolder);
        return true;
    }
}

//---------------------------------------------------------
int decreaseUsage(char * filePath, char * rodsUser, char * oldOwner) {

    long long fileSize = getRodsFileSize(filePath);
    if (fileSize <= 0) return fileSize;

    char *avuOwner = concat(oldOwner, usageSize);
    long long userNewSize = strtoll(getUserAVU(rodsUser, avuOwner), 0, 0) - fileSize;

    if (userNewSize < 0) userNewSize = 0;

    int result = setAVU("-u", rodsUser, avuOwner, lltostr(userNewSize));
    delete[] avuOwner;
    if (result != 0) {
        rodsLog(LOG_ERROR, "removeUsage remove %s got error %d [RODS user level]", rodsUser, result);
    }

    return result;
}

//---------------------------------------------------------
int increaseUsage(char * filePath, char * rodsUser, char * newOwner) {

    long long fileSize = getRodsFileSize(filePath);
    if (fileSize <= 0) return fileSize;

    //-----------------------------------------

    char *avuOwner = concat(newOwner, usageSize);
    long long userNewSize = fileSize + strtoll(getUserAVU(rodsUser, avuOwner), 0, 0);

    char * tmpSize = lltostr(userNewSize);
    int result = setAVU("-u", rodsUser, avuOwner, tmpSize);
    delete[] avuOwner; delete[] tmpSize;
    if (result != 0) {
        rodsLog(LOG_ERROR, "increaseUsage add %s got error %d [RODS user level]", rodsUser, result);
	return result;
    }

    //-----------------------------------------

    avuOwner = concat(newOwner, usageQuota);
    long long maxSize = strtoll(getUserAVU(rodsUser, avuOwner), 0, 0);
    if ((maxSize > 0) && (maxSize < userNewSize)) {
        dataObjInp_t dataObjInp;
        bzero (&dataObjInp, sizeof (dataObjInp));
        rstrcpy (dataObjInp.objPath, filePath, MAX_NAME_LEN);
        addKeyVal (&dataObjInp.condInput, FORCE_FLAG_KW, "");
        result = rcDataObjUnlink (conn, &dataObjInp);
        delete[] avuOwner;
        if (result < 0) {
            rodsLog(LOG_ERROR, "increaseUsage: got error while delete file overquota %s", filePath);
            return result;
        }
        else {
            rodsLog(LOG_ERROR, "increaseUsage: file overquota %s was deleted", filePath);
        }
    }
    else {
        delete[] avuOwner;
    }

    //-----------------------------------------

    return 0;
}

