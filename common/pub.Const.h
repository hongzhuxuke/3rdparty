#ifndef _H_PUBCONST_H_
#define _H_PUBCONST_H_



enum eLogRePortK
{
	eLogRePortK_StartStream = 12001,						//ֱ��-��ʼ����
	eLogRePortK_StopStream = 12002,						//ֱ��-ֹͣ����
	eLogRePortK_LogIn = 12003,									//��¼
	eLogRePortK_logOut = 12004,								//�˳� 

	eLogRePortK_StreamState = 12007,						//ֱ��-����״̬ 
	eLogRePortK_HostConfig = 12008,						//����������Ϣ
   eLogRePortK_BasicInfo = 12009,						   //basic_info������Ϣ
	//eLogRePortK_StartStreamInfo = 12009,				//��ʼ������Ϣͳ��

	eLogRePortK_LiveListRefresh = 12013,					//ֱ���б�ˢ��
	eLogRePortK_LiveListShare = 12016,					//��б�-����
	eLogRePortK_ChoiceActivityMode = 12018,			//ѡ��ֱ������
	eLogRePortK_JoinActivity = 12020,						//�α�����-����ֱ��
	eLogRePortK_Seting = 12021,						//ֱ������-����
	eLogRePortK_SystemSeting = 12022,						//ֱ������-����-ϵͳ����
	eLogRePortK_VedioSeting = 12006,							//ֱ������-����-��Ƶ����
	eLogRePortK_AudioSeting = 12005,							//ֱ������-����-��Ƶ����
	eLogRePortK_Record = 12023,								//ֱ������-����-¼������
	eLogRePortK_proxy = 12024,									//ֱ������-����-�������

	eLogRePortK_DesktopShare = 12028,					//ֱ��-������ʾ
	eLogRePortK_DesktopShare_Camera = 12029,	//ֱ��-������ʾ-�����豸
	eLogRePortK_DesktopShare_Mic = 12030,			//ֱ��-������ʾ-��˷�
	//eLogRePortK_DesktopShare_Chat = 12031,			//ֱ��-������ʾ-����
	eLogRePortK_DesktopShare_StartLive = 12033,//ֱ��-������ʾ-��ʼ/����ֱ��
	eLogRePortK_DesktopShare_StopDeskTopShare = 12035,//ֱ��-������ʾ-ֹͣ����
	eLogRePortK_SoftwareShare = 12036,					//ֱ��-�����ʾ

	eLogRePortK_SoftwareShare_Reflush = 12038,		//ֱ��-�����ʾ-ˢ��
	eLogRePortK_SoftwareShare_Ok= 12040,			//ֱ��-�����ʾ-ȷ��
	eLogRePortK_RegionShare = 12041,					   //ֱ��-������ʾ
	eLogRePortK_RegionShare_Change = 12042,		//ֱ��-������ʾ-λ���ƶ�/��С�ı�

	eLogRePortK_Camera_FullScreen = 12043,			//ֱ��-�����豸-ȫ������
	eLogRePortK_Camera_State = 12044,					//ֱ��-�����豸-����/�ر�
	eLogRePortK_MultiMedia_InsertVedio = 12046,						//ֱ��-��ý��-������Ƶ
	eLogRePortK_MultiMedia_InsertVedioList = 12047,					//ֱ��-��ý��-������Ƶ�б� �� ��/�� �ֿ�
	eLogRePortK_MultiMedia_DelVedio = 12048,					//ֱ��-��ý��-������Ƶ ɾ����Ƶ
	eLogRePortK_MultiMedia_Close = 12049,							//ֱ��-��ý��-�ر�
	eLogRePortK_MultiMedia_StartPlay = 12050,							//ֱ��-��ý��-��ʼ�岥
	eLogRePortK_MultiMedia_InsertImage = 12051,						//ֱ��-��ý��-����ͼƬ
	eLogRePortK_MultiMedia_InsertImage_Ok = 12052,				//ֱ��-��ý��-����ͼƬ-ȷ��
	eLogRePortK_MultiMedia_InsertText = 12053,						//ֱ��-��ý��-��������
	eLogRePortK_MultiMedia_InsertText_Ok = 12054,					//ֱ��-��ý��-��������-ȷ��
	eLogRePortK_StartLive = 12055,											   //ֱ��-��ʼ/���� 
	eLogRePortK_LivePlugInUnit= 12056,						            //ֱ��-ֱ�����
	eLogRePortK_ChangeMicNVal = 12057,													//ֱ��-��˷������ı�
	eLogRePortK_MicOpen = 12058,						                  //ֱ��-��˷翪��
	eLogRePortK_MicMute = 12059,						                     //ֱ��-��˷羲��
	//eLogRePortK_RightExtraChoice = 12061,						         //ֱ��-�Ҳ� ����/����/���� ģʽѡ��
	//eLogRePortK_RightExtraDisplayOrHide = 12062,						//ֱ��-�Ҳ�����/����/���� ��ʾ����

	//eLogRePortK_InteractionWdgMin = 12064,								//����������С��
	//eLogRePortK_InteractionWdgMax = 12065,								//�����������
	//eLogRePortK_InteractionWdgClose = 12066,							//��������رղ���
	//eLogRePortK_InteractionWdgClose_StopLive = 12067,			//��������-�ر�-����ֱ��
	//eLogRePortK_InteractionRenderWdg_DoubleClick = 12069,	//��������-�м��ͼ��Ⱦ����˫��
	//eLogRePortK_InteractionMemberList_Tab = 12070,				//����-��Ա�б�tabҳ������/�߳�/���ԣ����
	//eLogRePortK_Interaction_InvitationOnLine = 12071,				//����-��������
	//eLogRePortK_Interaction_GagOrCancle = 12072,					//����-����/ȡ������
	//eLogRePortK_Interaction_ShotOffOrCancle = 12073,			//����-�߳�/ȡ���߳�

	eLogRePortK_Interaction_UnderWheat = 12076,					//����-����
	//eLogRePortK_Interaction_HorListWidget_clicked = 12077,	//����-�α�С��Ļ����¼�
	//eLogRePortK_Interaction_DesktopShare = 12078,					//����-���湲��
	eLogRePortK_Interaction_Camera = 12079,								//����-�����豸 (��/��)
	eLogRePortK_Interaction_CameraChoice = 12080,					//����-�����豸/��˷�/������ �б�ѡ�� ѡ��
	eLogRePortK_Interaction_StartOrStopStream = 12081,			//����-��ʼ/��������
	//eLogRePortK_Interaction_Mic = 12082,									//����-��˷�
	//eLogRePortK_Interaction_Player = 12083,								//����-������
	//eLogRePortK_Interaction_RightExtraDisplayOrHide = 12088,//����-��ʾ/�����Ҳ����졢��������
	eLogRePortK_Interaction_HorOnWheat = 12089,					//����-�α�������/�������
	eLogRePortK_Interaction_HorJonined = 12090,						//�α���-�α������������Ӧ
	//eLogRePortK_InteractionWdg_Quit = 12095,							//�α���-�˳�ֱ��


	eLogRePortK_USBCamera_OP = 12096,								//USB�����豸�β����
	//eLogRePortK_USBCamera_Item = 12097,								//USB�����豸�β�֮����豸�б���
	eLogRePortK_StartRecord = 12098,										//¼��״̬
	////////////////////////////
	//eLogRePortK_VHSDK_Camera = 12099,							//SDK ����ͷ
	eLogRePortK_VHSDK_Mic = 12100,									//SDK��˷�
	eLogRePortK_VHSDK_DesktopShare = 12101,					//SDK���湲��
	eLogRePortK_VHSDK_Player = 12102,								//SDK-������
	//eLogRePortK_VHSDK_StartOrStopStream = 12103,		//SDK-��ʼ/��������
	//eLogRePortK_VHSDK_Member,
	//eLogRePortK_SuspendOrRecovery = 12099,                         //¼����ͣ/�ָ�����
	//eLogRePortK_UnConnectSourceNode = 14001,	//�޷����ӵ�Դ�ڵ�
	//eLogRePortK_NetworkAnomaly = 14002,				//����״����
	//eLogRePortK_HostPerformancePoor,						//�������ܲ�
	//eLogRePortK_ProCrash,											// ������� 
   //eLogRePortK_AudioDeviceCaptureErr = 14005,      //��Ƶ�豸�ɼ��쳣����
   //eLogRePortK_CaptureSourceSyncTimestampErr = 14006, //��Ƶ������ͬ������
};

//#define LOG_REPORT_KEY L"othersLog"
#define STR_DATETIME_Mil  "yyyy-MM-dd hh:mm:ss z"
#define STR_DATETIME_Sec  "yyyy-MM-dd hh:mm:ss"

/*¼��״̬*/
enum eRecordState
{
	eRecordState_Stop = 0,  //ֹͣ¼��
	eRecordState_Recording, //¼����
	eRecordState_Suspend   //��ͣ¼��
};

//С���ְ汾
enum eHostType
{
	eHostType_Standard = 0,		//��׼��
	eHostType_Professional,		//רҵ��
	eHostType_Ultimate,				//�콢��
};

//�Ƿ���ʾ���¼��
enum eDispalyCutRecord
{
	eDispalyCutRecord_Hide = 0,  //����ʾ
	eDispalyCutRecord_Show		 //��ʾ
};

enum eLiveType {
   eLiveType_Live = 0,        //ֱ��
   eLiveType_TcActive,          //�����˷���Ļ���ֱ��
   eLiveType_TcLoginActive,     //�α�ͨ���������Ļ���ֱ��
   eLiveType_VhallActive,		//΢�𻥶�ֱ��
   eLiveType_LoginVhallActive,
   //eLiveType_Active,          //�����˷���Ļ���ֱ��
   //eLiveType_LoginActive,     //�α�ͨ���������Ļ���ֱ��
};

enum ePageType {
   ePageType_OnLineUser = 0,	//�����б�
   ePageType_ChatForbid,			//����
   ePageType_KickOut,				//�߳�
   ePageType_RaiseHands,			//����
   ePageType_cont
};


enum eStopWebNair
{
	eStopWebNair_SUC = 0,	//�ɹ�
	eStopWebNair_Fail,			//ʧ��
	eStopWebNair_StopRecordFail,//��ͣ¼��ʧ��
    eStopWebNair_LiveShort,//ֱ��ʱ������
};


enum ILiveSDKErrCode {
   AV_ERR_FAILED = 1,//һ�����         =����ԭ����Ҫͨ��������־������λ
   AV_ERR_REPEATED_OPERATION = 1001,//�ظ�����         =�Ѿ��ڽ���ĳ�ֲ������ٴ�ȥ��ͬ���Ĳ���
   AV_ERR_EXCLUSIVE_OPERATION = 1002,//�������         =�ϴ���ز�����δ���
   AV_ERR_HAS_IN_THE_STATE = 1003,//=�Ѵ�����Ҫ״̬    =�����Ѿ����ڽ�Ҫ�����ĳ��״̬
   AV_ERR_INVALID_ARGUMENT = 1004,//=�������         =�������Ĳ���
   AV_ERR_TIMEOUT = 1005,//=��ʱ             =�ڹ涨��ʱ���ڣ���δ���ز������
   AV_ERR_NOT_IMPLEMENTED = 1006,//=δʵ��           =��Ӧ�Ĺ��ܻ�δ֧��
   AV_ERR_NOT_IN_MAIN_THREAD = 1007,//=�������߳�       =SDK����ӿ�Ҫ�������߳�ִ��
   AV_ERR_RESOURCE_IS_OCCUPIED = 1008,//=��Դ��ռ��       =��Ҫ�õ�ĳ����Դ��ռ����
   AV_ERR_CONTEXT_NOT_EXIST = 1101,//=AVContext����δ����CONTEXT_STATE_STARTED״̬       =��AVContext����δ����CONTEXT_STATE_STARTED״̬��ȥ������Ҫ�������״̬��������õĽӿ�ʱ���������������
   AV_ERR_CONTEXT_NOT_STOPPED = 1102,//=AVContext����δ����CONTEXT_STATE_STOPPED״̬      =��AVContext����δ����CONTEXT_STATE_STOPPED״̬��ȥ������Ҫ�������״̬��������õĽӿ�ʱ����������������粻������״̬�£�ȥ����AVContext::DestroyContextʱ���ͻ�����������
   AV_ERR_ROOM_NOT_EXIST = 1201,//=AVRoom����δ����ROOM_STATE_ENTERED״̬       =��AVRoom����δ����ROOM_STATE_ENTERED״̬��ȥ������Ҫ�������״̬��������õĽӿ�ʱ���������������
   AV_ERR_ROOM_NOT_EXITED = 1202,//=AVRoom����δ����ROOM_STATE_EXITED״̬      =��AVRoom����δ����ROOM_STATE_EXITED״̬��ȥ������Ҫ�������״̬��������õĽӿ�ʱ����������������粻������״̬�£�ȥ����AVContext::StopContextʱ���ͻ�����������   
   AV_ERR_DEVICE_NOT_EXIST = 1301,//=�豸������       =���豸�����ڻ����豸��ʼ��δ���ʱ��ȥʹ���豸���������������
   AV_ERR_ENDPOINT_NOT_EXIST = 1401,//=AVEndpoint���󲻴���       =����Աû���ڷ���������Ƶʱ��ȥ��ȡ����AVEndpoint����ʱ���Ϳ��ܲ����������
   AV_ERR_ENDPOINT_HAS_NOT_VIDEO = 1402,//=û�з���Ƶ       =����Աû���ڷ���Ƶʱ��ȥ����Ҫ��Ա����Ƶ����ز���ʱ���Ϳ��ܲ�����������統ĳ����Աû�з�����Ƶʱ��ȥ�������Ļ��棬�ͻ�����������
   AV_ERR_TINYID_TO_OPENID_FAILED = 1501,//=tinyidתidentifierʧ��         =���յ�ĳ����Ա��Ϣ���µ�����ʱ����Ҫȥ��tinyidת��identifier�����IMSDK������߼��������⡢�����������ȣ��������������
   AV_ERR_OPENID_TO_TINYID_FAILED = 1502,//=identifierתtinyidʧ��        =������StartContext�ӿ�ʱ����Ҫȥ��identifierת��tinyid�����IMSDK������߼��������⡢����������⡢û�д��ڵ�¼̬ʱ�ȣ��������������
   AV_ERR_DEVICE_TEST_NOT_EXIST = 1601,//=AVDeviceTest����δ����DEVICE_TEST_STATE_STARTED״̬(windows����)       =��AVDeviceTest����δ����DEVICE_TEST_STATE_STARTED״̬��ȥ������Ҫ�������״̬��������õĽӿ�ʱ���������������    
   AV_ERR_DEVICE_TEST_NOT_STOPPED = 1602,//=AVDeviceTest����δ����DEVICE_TEST_STATE_STOPPED״̬��windows���У�      =��AVDeviceTest����δ����DEVICE_TEST_STATE_STOPPED״̬��ȥ������Ҫ�������״̬��������õĽӿ�ʱ����������������粻������״̬�£�ȥ����AVContext::StopContextʱ���ͻ�����������   
   AV_ERR_INVITE_FAILED = 1801,//=����ʧ��         =��������ʱ������ʧ��
   AV_ERR_ACCEPT_FAILED = 1802,//=����ʧ��         =��������ʱ������ʧ��
   AV_ERR_REFUSE_FAILED = 1803,//=�ܾ�ʧ��         =�ܾ�����ʱ������ʧ��
   AV_ERR_IMSDK_FAIL = 6999,//=IMSDK�ص�֪ͨʧ�� =����ԭ����ͨ��������־ȷ��
   AV_ERR_IMSDK_TIMEOUT = 7000,//=IMSDK�ص�֪ͨ�ȴ���ʱ =����ԭ����ͨ��������־ȷ��
   AV_ERR_SERVER_FAILED = 10001,//=һ�����         =����ԭ����Ҫͨ��������־ȷ��
   AV_ERR_SERVER_INVALID_ARGUMENT = 10002,//=�������         =����Ĳ���
   AV_ERR_SERVER_NO_PERMISSION = 10003,//=û��Ȩ��         =û��Ȩ��ʹ��ĳ������
   AV_ERR_SERVER_TIMEOUT = 10004,//=�������ȡ�����ַʧ��  =����ԭ����Ҫͨ��������־ȷ��
   AV_ERR_SERVER_ALLOC_RESOURCE_FAILED = 10005,//=���������ӷ���ʧ�� =����ԭ����Ҫͨ��������־ȷ��
   AV_ERR_SERVER_ID_NOT_IN_ROOM = 10006,//=���ڷ���         =�ڲ��ڷ�����ʱ��ȥִ��ĳЩ����
   AV_ERR_SERVER_NOT_IMPLEMENT = 10007,//=δʵ��           =����SDK�ӿ�ʱ�������Ӧ�Ĺ��ܻ�δ֧��
   AV_ERR_SERVER_REPEATED_OPERATION = 10008,//=�ظ�����         =����ԭ����Ҫͨ��������־ȷ��
   AV_ERR_SERVER_ROOM_NOT_EXIST = 10009,//=���䲻����       =���䲻����ʱ��ȥִ��ĳЩ����
   AV_ERR_SERVER_ENDPOINT_NOT_EXIST = 10010,//=��Ա������       =ĳ����Ա������ʱ��ȥִ�иó�Ա��صĲ���
   AV_ERR_SERVER_INVALID_ABILITY = 10011,//=��������         =����ԭ����Ҫͨ��������־ȷ��
};

enum ILiveIMSDK_ERRCode {
   ILiveIMTimeOut = 6012,
   LogInAgainErr = 6015,//�����ýӿڵ��ÿ��ƣ���һ��login�����ص�ǰ��������login�����᷵�ظô�����

};

#endif//_H_PUBCONST_H_