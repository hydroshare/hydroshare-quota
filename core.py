import os.path
from genquery import Query, AS_DICT

def getQuotaOwner(rule_args, callback, rei):
    objPath = rule_args[0]
    hsUserName = ""
    hsColParts = os.path.normpath(objPath).split(os.path.sep)
    callback.writeLine("serverLog", "hsColParts = [{}]".format(hsColParts))
    if len(hsColParts) > 7:
        # WHAT HYDROSHARE USER OWNS THIS DATA OBJECT?
        #     This comes from an AVU on the HS resource collection, which will be deleted prior
        #     to the execution of the delayed microservice, and thus must be computed and passed
	#     prior to deletion.
        #         Use the first four subcollection names to form the HS resource collection path.
        #         Look for the value of the attribute called quotaUserName on the HS resource collection.
        hsColPath = os.path.join(os.path.sep, *hsColParts[1:5])
        callback.writeLine("serverLog", "hsColPath = [{}]".format(hsColPath))
        rows = list(Query(callback,
                          ["META_COLL_ATTR_VALUE",],
                          "COLL_NAME = '{}' and META_COLL_ATTR_NAME = 'quotaUserName'".format(hsColPath),
                          AS_DICT, 0, 2))
        callback.writeLine("serverLog", "len(rows) = [{}]".format(len(rows)))
        if rows:
            hsUserName = rows[0]["META_COLL_ATTR_VALUE"]
    rule_args[1] = hsUserName
    return
