#ifndef _H_PUBCONST_H_
#define _H_PUBCONST_H_



enum eLogRePortK
{
	eLogRePortK_StartStream = 12001,						//直播-开始推流
	eLogRePortK_StopStream = 12002,						//直播-停止推流
	eLogRePortK_LogIn = 12003,									//登录
	eLogRePortK_logOut = 12004,								//退出 

	eLogRePortK_StreamState = 12007,						//直播-推流状态 
	eLogRePortK_HostConfig = 12008,						//主机配置信息
   eLogRePortK_BasicInfo = 12009,						   //basic_info基本信息
	//eLogRePortK_StartStreamInfo = 12009,				//开始推流信息统计

	eLogRePortK_LiveListRefresh = 12013,					//直播列表刷新
	eLogRePortK_LiveListShare = 12016,					//活动列表-分享
	eLogRePortK_ChoiceActivityMode = 12018,			//选择直播类型
	eLogRePortK_JoinActivity = 12020,						//嘉宾连麦-进入直播
	eLogRePortK_Seting = 12021,						//直播界面-设置
	eLogRePortK_SystemSeting = 12022,						//直播界面-设置-系统设置
	eLogRePortK_VedioSeting = 12006,							//直播界面-设置-视频设置
	eLogRePortK_AudioSeting = 12005,							//直播界面-设置-音频设置
	eLogRePortK_Record = 12023,								//直播界面-设置-录制设置
	eLogRePortK_proxy = 12024,									//直播界面-设置-网络代理

	eLogRePortK_DesktopShare = 12028,					//直播-桌面演示
	eLogRePortK_DesktopShare_Camera = 12029,	//直播-桌面演示-摄像设备
	eLogRePortK_DesktopShare_Mic = 12030,			//直播-桌面演示-麦克风
	//eLogRePortK_DesktopShare_Chat = 12031,			//直播-桌面演示-聊天
	eLogRePortK_DesktopShare_StartLive = 12033,//直播-桌面演示-开始/结束直播
	eLogRePortK_DesktopShare_StopDeskTopShare = 12035,//直播-桌面演示-停止共享
	eLogRePortK_SoftwareShare = 12036,					//直播-软件演示

	eLogRePortK_SoftwareShare_Reflush = 12038,		//直播-软件演示-刷新
	eLogRePortK_SoftwareShare_Ok= 12040,			//直播-软件演示-确定
	eLogRePortK_RegionShare = 12041,					   //直播-区域演示
	eLogRePortK_RegionShare_Change = 12042,		//直播-区域演示-位置移动/大小改变

	eLogRePortK_Camera_FullScreen = 12043,			//直播-摄像设备-全屏操作
	eLogRePortK_Camera_State = 12044,					//直播-摄像设备-开启/关闭
	eLogRePortK_MultiMedia_InsertVedio = 12046,						//直播-多媒体-插入视频
	eLogRePortK_MultiMedia_InsertVedioList = 12047,					//直播-多媒体-插入视频列表 以 “/” 分开
	eLogRePortK_MultiMedia_DelVedio = 12048,					//直播-多媒体-插入视频 删除视频
	eLogRePortK_MultiMedia_Close = 12049,							//直播-多媒体-关闭
	eLogRePortK_MultiMedia_StartPlay = 12050,							//直播-多媒体-开始插播
	eLogRePortK_MultiMedia_InsertImage = 12051,						//直播-多媒体-插入图片
	eLogRePortK_MultiMedia_InsertImage_Ok = 12052,				//直播-多媒体-插入图片-确定
	eLogRePortK_MultiMedia_InsertText = 12053,						//直播-多媒体-插入文字
	eLogRePortK_MultiMedia_InsertText_Ok = 12054,					//直播-多媒体-插入文字-确定
	eLogRePortK_StartLive = 12055,											   //直播-开始/结束 
	eLogRePortK_LivePlugInUnit= 12056,						            //直播-直播插件
	eLogRePortK_ChangeMicNVal = 12057,													//直播-麦克风音量改变
	eLogRePortK_MicOpen = 12058,						                  //直播-麦克风开启
	eLogRePortK_MicMute = 12059,						                     //直播-麦克风静音
	//eLogRePortK_RightExtraChoice = 12061,						         //直播-右侧 聊天/公告/观众 模式选择
	//eLogRePortK_RightExtraDisplayOrHide = 12062,						//直播-右侧聊天/公告/观众 显示隐藏

	//eLogRePortK_InteractionWdgMin = 12064,								//互动界面最小化
	//eLogRePortK_InteractionWdgMax = 12065,								//互动界面最大化
	//eLogRePortK_InteractionWdgClose = 12066,							//互动界面关闭操作
	//eLogRePortK_InteractionWdgClose_StopLive = 12067,			//互动界面-关闭-结束直播
	//eLogRePortK_InteractionRenderWdg_DoubleClick = 12069,	//互动界面-中间绘图渲染界面双击
	//eLogRePortK_InteractionMemberList_Tab = 12070,				//互动-成员列表tab页（在线/踢出/禁言）点击
	//eLogRePortK_Interaction_InvitationOnLine = 12071,				//互动-邀请上线
	//eLogRePortK_Interaction_GagOrCancle = 12072,					//互动-禁言/取消禁言
	//eLogRePortK_Interaction_ShotOffOrCancle = 12073,			//互动-踢出/取消踢出

	eLogRePortK_Interaction_UnderWheat = 12076,					//互动-下麦
	//eLogRePortK_Interaction_HorListWidget_clicked = 12077,	//互动-嘉宾小屏幕点击事件
	//eLogRePortK_Interaction_DesktopShare = 12078,					//互动-桌面共享
	eLogRePortK_Interaction_Camera = 12079,								//互动-摄像设备 (开/关)
	eLogRePortK_Interaction_CameraChoice = 12080,					//互动-摄像设备/麦克风/扬声器 列表选择 选择
	eLogRePortK_Interaction_StartOrStopStream = 12081,			//互动-开始/结束推流
	//eLogRePortK_Interaction_Mic = 12082,									//互动-麦克风
	//eLogRePortK_Interaction_Player = 12083,								//互动-扬声器
	//eLogRePortK_Interaction_RightExtraDisplayOrHide = 12088,//互动-显示/隐藏右侧聊天、公告区域
	eLogRePortK_Interaction_HorOnWheat = 12089,					//互动-嘉宾端上麦/下麦操作
	eLogRePortK_Interaction_HorJonined = 12090,						//嘉宾端-嘉宾被邀请上麦回应
	//eLogRePortK_InteractionWdg_Quit = 12095,							//嘉宾端-退出直播


	eLogRePortK_USBCamera_OP = 12096,								//USB摄像设备拔插操作
	//eLogRePortK_USBCamera_Item = 12097,								//USB摄像设备拔插之后的设备列表项
	eLogRePortK_StartRecord = 12098,										//录制状态
	////////////////////////////
	//eLogRePortK_VHSDK_Camera = 12099,							//SDK 摄像头
	eLogRePortK_VHSDK_Mic = 12100,									//SDK麦克风
	eLogRePortK_VHSDK_DesktopShare = 12101,					//SDK桌面共享
	eLogRePortK_VHSDK_Player = 12102,								//SDK-扬声器
	//eLogRePortK_VHSDK_StartOrStopStream = 12103,		//SDK-开始/结束推流
	//eLogRePortK_VHSDK_Member,
	//eLogRePortK_SuspendOrRecovery = 12099,                         //录制暂停/恢复操作
	//eLogRePortK_UnConnectSourceNode = 14001,	//无法连接到源节点
	//eLogRePortK_NetworkAnomaly = 14002,				//网络状况差
	//eLogRePortK_HostPerformancePoor,						//主机性能差
	//eLogRePortK_ProCrash,											// 程序崩溃 
   //eLogRePortK_AudioDeviceCaptureErr = 14005,      //音频设备采集异常重试
   //eLogRePortK_CaptureSourceSyncTimestampErr = 14006, //音频混音不同步排序
};

//#define LOG_REPORT_KEY L"othersLog"
#define STR_DATETIME_Mil  "yyyy-MM-dd hh:mm:ss z"
#define STR_DATETIME_Sec  "yyyy-MM-dd hh:mm:ss"

/*录制状态*/
enum eRecordState
{
	eRecordState_Stop = 0,  //停止录制
	eRecordState_Recording, //录制中
	eRecordState_Suspend   //暂停录制
};

//小助手版本
enum eHostType
{
	eHostType_Standard = 0,		//标准版
	eHostType_Professional,		//专业版
	eHostType_Ultimate,				//旗舰版
};

//是否显示打点录制
enum eDispalyCutRecord
{
	eDispalyCutRecord_Hide = 0,  //不显示
	eDispalyCutRecord_Show		 //显示
};

enum eLiveType {
   eLiveType_Live = 0,        //直播
   eLiveType_TcActive,          //主持人发起的互动直播
   eLiveType_TcLoginActive,     //嘉宾通过口令加入的互动直播
   eLiveType_VhallActive,		//微吼互动直播
   eLiveType_LoginVhallActive,
   //eLiveType_Active,          //主持人发起的互动直播
   //eLiveType_LoginActive,     //嘉宾通过口令加入的互动直播
};

enum ePageType {
   ePageType_OnLineUser = 0,	//在线列表
   ePageType_ChatForbid,			//禁言
   ePageType_KickOut,				//踢出
   ePageType_RaiseHands,			//举手
   ePageType_cont
};


enum eStopWebNair
{
	eStopWebNair_SUC = 0,	//成功
	eStopWebNair_Fail,			//失败
	eStopWebNair_StopRecordFail,//暂停录制失败
    eStopWebNair_LiveShort,//直播时长过短
};


enum ILiveSDKErrCode {
   AV_ERR_FAILED = 1,//一般错误         =具体原因需要通过分析日志等来定位
   AV_ERR_REPEATED_OPERATION = 1001,//重复操作         =已经在进行某种操作，再次去做同样的操作
   AV_ERR_EXCLUSIVE_OPERATION = 1002,//互斥操作         =上次相关操作尚未完成
   AV_ERR_HAS_IN_THE_STATE = 1003,//=已处于所要状态    =对象已经处于将要进入的某种状态
   AV_ERR_INVALID_ARGUMENT = 1004,//=错误参数         =传入错误的参数
   AV_ERR_TIMEOUT = 1005,//=超时             =在规定的时间内，还未返回操作结果
   AV_ERR_NOT_IMPLEMENTED = 1006,//=未实现           =相应的功能还未支持
   AV_ERR_NOT_IN_MAIN_THREAD = 1007,//=不在主线程       =SDK对外接口要求在主线程执行
   AV_ERR_RESOURCE_IS_OCCUPIED = 1008,//=资源被占用       =需要用到某种资源被占用了
   AV_ERR_CONTEXT_NOT_EXIST = 1101,//=AVContext对象未处于CONTEXT_STATE_STARTED状态       =当AVContext对象未处于CONTEXT_STATE_STARTED状态，去调用需要处于这个状态才允许调用的接口时，则会产生这个错误。
   AV_ERR_CONTEXT_NOT_STOPPED = 1102,//=AVContext对象未处于CONTEXT_STATE_STOPPED状态      =当AVContext对象未处于CONTEXT_STATE_STOPPED状态，去调用需要处于这个状态才允许调用的接口时，则会产生这个错误。如不在这种状态下，去调用AVContext::DestroyContext时，就会产生这个错误。
   AV_ERR_ROOM_NOT_EXIST = 1201,//=AVRoom对象未处于ROOM_STATE_ENTERED状态       =当AVRoom对象未处于ROOM_STATE_ENTERED状态，去调用需要处于这个状态才允许调用的接口时，则会产生这个错误。
   AV_ERR_ROOM_NOT_EXITED = 1202,//=AVRoom对象未处于ROOM_STATE_EXITED状态      =当AVRoom对象未处于ROOM_STATE_EXITED状态，去调用需要处于这个状态才允许调用的接口时，则会产生这个错误。如不在这种状态下，去调用AVContext::StopContext时，就会产生这个错误。   
   AV_ERR_DEVICE_NOT_EXIST = 1301,//=设备不存在       =当设备不存在或者设备初始化未完成时，去使用设备，则会产生这个错误。
   AV_ERR_ENDPOINT_NOT_EXIST = 1401,//=AVEndpoint对象不存在       =当成员没有在发语音或视频时，去获取它的AVEndpoint对象时，就可能产生这个错误。
   AV_ERR_ENDPOINT_HAS_NOT_VIDEO = 1402,//=没有发视频       =当成员没有在发视频时，去做需要成员发视频的相关操作时，就可能产生这个错误。如当某个成员没有发送视频时，去请求他的画面，就会产生这个错误。
   AV_ERR_TINYID_TO_OPENID_FAILED = 1501,//=tinyid转identifier失败         =当收到某个成员信息更新的信令时，需要去把tinyid转成identifier，如果IMSDK库相关逻辑存在问题、网络存在问题等，则会产生这个错误。
   AV_ERR_OPENID_TO_TINYID_FAILED = 1502,//=identifier转tinyid失败        =当调用StartContext接口时，需要去把identifier转成tinyid，如果IMSDK库相关逻辑存在问题、网络存在问题、没有处于登录态时等，则会产生这个错误。
   AV_ERR_DEVICE_TEST_NOT_EXIST = 1601,//=AVDeviceTest对象未处于DEVICE_TEST_STATE_STARTED状态(windows特有)       =当AVDeviceTest对象未处于DEVICE_TEST_STATE_STARTED状态，去调用需要处于这个状态才允许调用的接口时，则会产生这个错误。    
   AV_ERR_DEVICE_TEST_NOT_STOPPED = 1602,//=AVDeviceTest对象未处于DEVICE_TEST_STATE_STOPPED状态（windows特有）      =当AVDeviceTest对象未处于DEVICE_TEST_STATE_STOPPED状态，去调用需要处于这个状态才允许调用的接口时，则会产生这个错误。如不在这种状态下，去调用AVContext::StopContext时，就会产生这个错误。   
   AV_ERR_INVITE_FAILED = 1801,//=发送失败         =发送邀请时产生的失败
   AV_ERR_ACCEPT_FAILED = 1802,//=接受失败         =接受邀请时产生的失败
   AV_ERR_REFUSE_FAILED = 1803,//=拒绝失败         =拒绝邀请时产生的失败
   AV_ERR_IMSDK_FAIL = 6999,//=IMSDK回调通知失败 =具体原因需通过分析日志确认
   AV_ERR_IMSDK_TIMEOUT = 7000,//=IMSDK回调通知等待超时 =具体原因需通过分析日志确认
   AV_ERR_SERVER_FAILED = 10001,//=一般错误         =具体原因需要通过分析日志确认
   AV_ERR_SERVER_INVALID_ARGUMENT = 10002,//=错误参数         =错误的参数
   AV_ERR_SERVER_NO_PERMISSION = 10003,//=没有权限         =没有权限使用某个功能
   AV_ERR_SERVER_TIMEOUT = 10004,//=进房间获取房间地址失败  =具体原因需要通过分析日志确认
   AV_ERR_SERVER_ALLOC_RESOURCE_FAILED = 10005,//=进房间连接房间失败 =具体原因需要通过分析日志确认
   AV_ERR_SERVER_ID_NOT_IN_ROOM = 10006,//=不在房间         =在不在房间内时，去执行某些操作
   AV_ERR_SERVER_NOT_IMPLEMENT = 10007,//=未实现           =调用SDK接口时，如果相应的功能还未支持
   AV_ERR_SERVER_REPEATED_OPERATION = 10008,//=重复操作         =具体原因需要通过分析日志确认
   AV_ERR_SERVER_ROOM_NOT_EXIST = 10009,//=房间不存在       =房间不存在时，去执行某些操作
   AV_ERR_SERVER_ENDPOINT_NOT_EXIST = 10010,//=成员不存在       =某个成员不存在时，去执行该成员相关的操作
   AV_ERR_SERVER_INVALID_ABILITY = 10011,//=错误能力         =具体原因需要通过分析日志确认
};

enum ILiveIMSDK_ERRCode {
   ILiveIMTimeOut = 6012,
   LogInAgainErr = 6015,//请做好接口调用控制，第一次login操作回调前，后续的login操作会返回该错误码

};

#endif//_H_PUBCONST_H_