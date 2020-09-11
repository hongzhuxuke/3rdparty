#include "vhallstreamremux.h"
#include <QDebug>
#include <QEventLoop>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "DebugTrace.h"
#include "httpnetwork.h"
#define STREAM_TAIL "_helper"

QString GetTententSign(QString str) {
   QCryptographicHash md(QCryptographicHash::Md5);
   md.addData(str.toUtf8());
   return md.result().toHex();
}

VhallStreamRemix::VhallStreamRemix(QObject *parent)
: QObject(parent) {
   mHttpNetwork = new HttpNetWork(this);
   if (mHttpNetwork) {
      connect(mHttpNetwork, SIGNAL(HttpNetworkGetFinished(QByteArray, int)), this, SLOT(slot_OnHttpPostNetWorkfinished(QByteArray, int)));
   }
   connect(&mMixStreamTimer, SIGNAL(timeout()), this, SLOT(slot_OnMixStream()));
   mMixStreamTimer.start(1500);   
}

VhallStreamRemix::~VhallStreamRemix() {
   mMixStreamTimer.stop();
   if (mHttpNetwork) {
      delete mHttpNetwork;
      mHttpNetwork = NULL;
   }
}

void VhallStreamRemix::CancelRemix() {
   qDebug() << "VhallStreamRemix::CancelRemix()";
   emit this->SigFadout(QString::fromWCharArray(L"摄像头全部关闭后，观众将无法观看直播"));
}
void VhallStreamRemix::Init(int appid) {
   m_appid = appid;
}

void VhallStreamRemix::PostBody(QJsonObject &para, bool bCancelMix/* = false*/) {
   MixParam param;
   param.obj = para;
   param.cancelMix = bCancelMix;
   QMutexLocker l(&mixParamMutex);
   mMixStreamParamList.push_back(param);
}

void VhallStreamRemix::RemixStream(QString &desktopId, QSet<QString> &cameras, QSet<QString> &usersMic) {
   TRACE6("%s desktopId:%s: cameras count:%d usersMic:%d\n", __FUNCTION__, desktopId.toStdString().c_str(), cameras.count(), usersMic.count());
   QJsonObject para;
   QJsonArray input_stream_list;
   QList<QString> cameraLists = cameras.toList();
   QList<QString> micLists = usersMic.toList();
   for (int i = 0; i < cameraLists.size(); i++) {
      TRACE6("%s cameras[%d]:%s\n", __FUNCTION__, i, cameraLists[i].toStdString().c_str());
   }
   for (int i = 0; i < micLists.size(); i++) {
      TRACE6("%s micLists[%d]:%s\n", __FUNCTION__, i, micLists[i].toStdString().c_str());
   }
   if (desktopId == "") {
      int nLayer = 1;
      if (cameras.count() + micLists.count() == 1) {
         QString userStreamID;
         if (cameras.count() == 1) {
            userStreamID = cameraLists[0];
         }
         else {
            userStreamID = micLists[0];
         }
         QJsonObject person;
         person["input_stream_id"] = userStreamID;
         QJsonObject layout_params;
         layout_params["image_layer"] = nLayer++;
         person["layout_params"] = layout_params;
         input_stream_list.append(person);

         person = QJsonObject();
         person["input_stream_id"] = userStreamID;
         layout_params = QJsonObject();
         layout_params["image_layer"] = nLayer++;//,               # 图层标识号
         layout_params["image_width"] = 4;//,             # 小主播画面宽度
         layout_params["image_height"] = 4;//,            # 小主播画面高度
         layout_params["location_x"] = 0;//,              # x偏移：相对于大主播背景画面左上角的横向偏移
         layout_params["location_y"] = 0;//               # y偏移：相对于大主播背景画面左上角的纵向偏移
         person["layout_params"] = layout_params;
         input_stream_list.append(person);
      }
      else if (cameras.count() + micLists.count() == 2) {
         QJsonObject person;
         QJsonObject layout_params;
         layout_params["input_type"] = 3;
         layout_params["color"] = "0x000000";
         layout_params["image_width"] = 1280;
         layout_params["image_height"] = 720;
         layout_params["image_layer"] = nLayer++;
         person["layout_params"] = layout_params;
         input_stream_list.append(person);
         InsertUserLayOut(input_stream_list, nLayer, cameraLists);
         InsertUserLayOut(input_stream_list, nLayer, micLists);
         para["mix_stream_template_id"] = 390;
      }
      else if (cameras.count() + micLists.count() == 3 || cameras.count() + micLists.count() == 4) {
         QJsonObject person;
         QJsonObject layout_params;
         layout_params["input_type"] = 3;
         layout_params["color"] = "0x000000";
         layout_params["image_width"] = 1280;
         layout_params["image_height"] = 720;
         layout_params["image_layer"] = nLayer++;
         person["layout_params"] = layout_params;
         input_stream_list.append(person);
         InsertUserLayOut(input_stream_list, nLayer, cameraLists);
         InsertUserLayOut(input_stream_list, nLayer, micLists);
         if (cameras.count() + micLists.count() == 3) {
            person = QJsonObject();
            person["input_stream_id"] = cameraLists.count() > 0 ? cameraLists[0] : micLists[0];
            layout_params = QJsonObject();
            layout_params["image_layer"] = nLayer++;
            layout_params["input_type"] = 3;
            layout_params["color"] = "0x000000";
            layout_params["image_width"] = 640;
            layout_params["image_height"] = 360;
            layout_params["image_layer"] = nLayer++;
            person["layout_params"] = layout_params;
            input_stream_list.append(person);
         }
         para["mix_stream_template_id"] = 590;
      }
   } else {
      int nLayOut = 1;
      QJsonObject person;
      person["input_stream_id"] = desktopId;
      QJsonObject layout_params;
      layout_params["image_layer"] = nLayOut++;
      person["layout_params"] = layout_params;
      input_stream_list.append(person);

      person = QJsonObject();
      person["input_stream_id"] = desktopId;
      layout_params = QJsonObject();
      layout_params["image_layer"] = nLayOut++;//,               # 图层标识号
      layout_params["image_width"] = 4;//,             # 小主播画面宽度
      layout_params["image_height"] = 4;//,            # 小主播画面高度
      layout_params["location_x"] = 0;//,              # x偏移：相对于大主播背景画面左上角的横向偏移
      layout_params["location_y"] = 0;//               # y偏移：相对于大主播背景画面左上角的纵向偏移
      person["layout_params"] = layout_params;
      input_stream_list.append(person);

      if (cameras.count() == 0) {
         InsertUserLayOut(input_stream_list, nLayOut, micLists,true);
      } else if (cameras.count() > 0) {
         InsertUserLayOut(input_stream_list, nLayOut, cameraLists,true);
         InsertUserLayOut(input_stream_list, nLayOut, micLists,true);
         //para["mix_stream_template_id"] = 10;
      }
   }
   para["input_stream_list"] = input_stream_list;
   PostBody(para);
}

void VhallStreamRemix::StopMixStream(const QString& streamID) {
   QJsonArray input_stream_list;
   QJsonObject para;
   QJsonObject person;
   person["input_stream_id"] = streamID;
   QJsonObject layout_params;
   layout_params["image_layer"] = 1;
   person["layout_params"] = layout_params;
   input_stream_list.append(person);
   para["input_stream_list"] = input_stream_list;
   PostBody(para, true);
}

void VhallStreamRemix::InsertUserLayOut(QJsonArray &input_stream_list, int &startLayout, const QList<QString> &userId, bool audioType/* = false*/) {
   for (int i = 0; i < userId.count(); i++) {
      QJsonObject micUserperson = QJsonObject();
      micUserperson["input_stream_id"] = userId[i];
      QJsonObject micUserLayoutParams = QJsonObject();
      micUserLayoutParams["image_layer"] = startLayout++;
      if (audioType) {
         micUserLayoutParams["image_width"] = 4;//,             # 小主播画面宽度
         micUserLayoutParams["image_height"] = 4;//,            # 小主播画面高度
         micUserLayoutParams["location_x"] = 0;//,              # x偏移：相对于大主播背景画面左上角的横向偏移
         micUserLayoutParams["location_y"] = 0;//               # y偏移：相对于大主播背景画面左上角的纵向偏移
      }
      micUserperson["layout_params"] = micUserLayoutParams;
      input_stream_list.append(micUserperson);
   }
}

void VhallStreamRemix::slot_OnHttpPostNetWorkfinished(QByteArray data, int networkCode) {
   QByteArray reply = data;
   if (reply.toStdString().compare("time expired") == 0) {
      emit SigTimeExpired();
      return;
   }

   if (reply.isEmpty()) {
      TRACE6("%s  reply is empty\n", __FUNCTION__);
      emit SigRemixEnd(false);
   } else {
      QJsonDocument doc = QJsonDocument::fromJson(reply);
      QJsonObject obj = doc.object();
      if (obj.contains("code")) {
         int code = obj["code"].toInt();
         if (code != 0) {
            TRACE6("%s  code is %d\n", __FUNCTION__, code);
         }
         TRACE6("%s reply:%s\n", __FUNCTION__, reply.toStdString().c_str());
         emit SigRemixEnd(code == 0);
      }
   }
}

void VhallStreamRemix::slot_OnMixStream() {
   bool bCancelMix;
   MixParam param;
   QJsonObject para;
   {
      QMutexLocker l(&mixParamMutex);
      if (mMixStreamParamList.size() > 0) {
         param = mMixStreamParamList.front();
         para = param.obj;
         bCancelMix = param.cancelMix;
         mMixStreamParamList.pop_front();
      }
      else  {
         return;
      }
   }

   int appid = m_appid;
   QString interfaceName = "Mix_StreamV2";
   uint t = QDateTime::currentDateTime().toTime_t() + 60;
   QString apiKey = "941f6fc4c80f614eca51f2677b103c70";
   QString sign = GetTententSign(apiKey + QString::number(t));
   QString url = "http://fcgi.video.qcloud.com/common_access?appid=%1&interface=%2&t=%3&sign=%4";
   url = url.arg(appid).arg(interfaceName).arg(t).arg(sign);

   QJsonObject body;
   body["timestamp"] = (int)t;
   body["eventId"] = (int)t;
   QJsonObject jsonObj;
   jsonObj["interfaceName"] = "Mix_StreamV2";
   para["app_id"] = appid;
   para["interface"] = "mix_streamv2.start_mix_stream_advanced";
   para["output_stream_id"] = mStreamId + STREAM_TAIL;
   if (!bCancelMix) {
      para["output_stream_type"] = 1;
   }
   para["mix_stream_session_id"] = mStreamId;

   jsonObj["para"] = para;
   body["interface"] = jsonObj;
   QJsonDocument doc;
   doc.setObject(body);

   QByteArray bodyByteArray = doc.toJson();
   if (mHttpNetwork) {
      mHttpNetwork->HttpNetworkPost(url, bodyByteArray);
   }
   TRACE6("%s mix:%s\n", __FUNCTION__, bodyByteArray.toStdString().c_str());
}

