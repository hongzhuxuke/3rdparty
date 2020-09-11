#ifndef __INACTIVE_SOCKET_INCLUDE_H__
#define __INACTIVE_SOCKET_INCLUDE_H__    

#define SIZE_RECV 35536
#define MSG_SIZE 4096

#define MSG_TYPE_ENGINE_START "startEngine"
#define MSG_TYPE_ENGINE_STOP  "stopEngine"
#define MSG_TYPE_CLOSE_STREAM  "closeStream"
#define MSG_TYPE_HEARBEAT "hearbeat"
#define MSG_TYPE_ENGINE_QUERY "queryEngine"
#define MSG_TYPE_UPDATE_SOFTWARE "updateSoftware"

#define MSG_TYPE_HELPER_QUERY "queryHelperStartup"



#define MSG_TYPE_NOTIFY_PUBLISH_SUCCESS  "publishSuccess"
#define MSG_TYPE_NOTIFY_PUBLISH_FAILED  "publishFailed"
#define MSG_TYPE_NOTIFY_PUBLISH_STOPED  "publishStoped"
#define MSG_TYPE_NOTIFY_PUBLISH_BEGIN "publishBegin"
#define MSG_TYPE_NOTIFY_HELPER_LIVE  "helperLive"
#define MSG_TYPE_NOTIFY_HELPER_DIED  "helperDied"


#define MSG_BODY "{ \"type\": \"%s\",  \"stream\" : \"%s\"}"

/*
{ "code": "200","msg":"success","type": "%s", "data" : "stream  [%s] success!"}
*/
#define REPLY_MSG_SUCESS "{ \"code\": \"200\",\"msg\":\"success\",\"type\": \"%s\", \"data\" : \"  [%s] \"}"
/*
{ "code": "400","msg":"bad request","type": "%s", "data" : "  [%s] "}
*/
#define REPLY_MSG_BADREQ "{ \"code\": \"400\",\"msg\":\"bad request\",\"type\": \"%s\", \"data\" : \"  [%s] \"}"
/*
{ "code": "500","msg":"operation failed","type": "%s", "data" : "stream  [%s] success!"}
*/
#define REPLY_MSG_ERROR "{ \"code\": \"500\",\"msg\":\"operation failed\",\"type\": \"%s\", \"data\" : \"  [%s] \"}"


#define POLICY_REQ "<policy-file-request/>"
#define POLICY_RES  "<cross-domain-policy> "\
   "<allow-access-from domain=\"*\" to-ports=\"*\"/>"\
   "</cross-domain-policy> "


#define LOCAL_EVENT_NAME    L"Global\\vhallheaper_event"
#define SERVER_PORT 966
#define MENGZHU_SERVER_PORT 967
#define SERVICE_ADDR "127.0.0.1"

#endif