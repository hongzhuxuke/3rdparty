#include "vhalluserinfomanager.h"

VhallAudienceUserInfo_st::VhallAudienceUserInfo_st()
{
	userId = "";
	userName = "";
	role = "";
	miUserCount = 0;
}

VhallAudienceUserInfo_st::VhallAudienceUserInfo_st(const UserInfo &uInfo)
{
	userId = QString::fromWCharArray(uInfo.m_szUserID);
	userName = QString::fromWCharArray(uInfo.m_szUserName);
	role = QString::fromWCharArray(uInfo.m_szRole);
	miUserCount = 0;
}

VhallAudienceUserInfo_st::VhallAudienceUserInfo_st(const VhallAudienceUserInfo_st &uInfo)
{
	userId = uInfo.userId;
	userName = uInfo.userName;
	role = uInfo.role;
	gagType = uInfo.gagType;
	kickType = uInfo.kickType;
	miUserCount = uInfo.miUserCount;
}

bool VhallAudienceUserInfo_st::operator<(const VhallAudienceUserInfo_st other)
{
	if (userId == other.userId) {
		return false;
	}

	if (other.role == "user") {
		return false;
	}
	else if (role == "host") {
		return false;
	}
	else if (role == "guest") {
		if (other.role == "assistant") {
			return false;
		}
	}

	return true;
}

bool VhallAudienceUserInfo_st::operator == (const VhallAudienceUserInfo_st other)
{
	return userId == other.userId;
}

bool VhallAudienceUserInfoList_st::Has(QString qid)
{
	return mUserIdList.find(qid) != mUserIdList.end();
}

bool VhallAudienceUserInfoList_st::Has(wchar_t *id)
{
	QString qid = QString::fromWCharArray(id);
	return mUserIdList.find(qid) != mUserIdList.end();
}

bool VhallAudienceUserInfoList_st::GetUserInfo(const UserInfo &userInfo, VhallAudienceUserInfo &uInfo)
{
	VhallAudienceUserInfo inputUserInfo = VhallAudienceUserInfo(userInfo);
	for (int i = 0; i < mInfoList.count(); i++) {
		if (inputUserInfo == mInfoList[i]) {
			uInfo = mInfoList[i];
			return true;
		}
	}
	return false;
}

bool VhallAudienceUserInfoList_st::pushBack(const VhallAudienceUserInfo &info)
{
	if (mUserIdList.find(info.userId) != mUserIdList.end()) {
		return false;
	}

	mInfoList.append(info);
	mUserIdList.insert(info.userId);
	return true;
}

bool VhallAudienceUserInfoList_st::append(const VhallAudienceUserInfo &info)
{
	if (mUserIdList.find(info.userId) != mUserIdList.end()) {
		return false;
	}

	for (int i = 0; i < mInfoList.count(); i++) {
		if (mInfoList[i] < info) {
			mInfoList.append(info);
			mUserIdList.insert(info.userId);
			return true;
		}
	}
	mInfoList.append(info);
	mUserIdList.insert(info.userId);
	return true;
}

bool VhallAudienceUserInfoList_st::Remove(const VhallAudienceUserInfo &info)
{
	auto itor = mUserIdList.find(info.userId);
	if (itor == mUserIdList.end()) {
		qDebug() << "Not Find Such id " << info.userId;
		return false;
	}
	mUserIdList.erase(itor);

	for (int i = 0; i < mInfoList.count(); i++) {
		if (mInfoList[i] == info) {
			mInfoList.removeAt(i);

			qDebug() << "Remove:" << info.userId;
			break;
		}
	}
	return true;
}

VhallAudienceUserInfo & VhallAudienceUserInfoList_st::operator [](int index)
{
	if (index >= mInfoList.count()) {
		return mEmpty;
	}
	return mInfoList[index];
}


void VhallAudienceUserInfoList_st::clear()
{
	mInfoList.clear();
	mUserIdList.clear();
}
int VhallAudienceUserInfoList_st::count() {
	return mInfoList.count();
}

void VhallAudienceUserInfoList_st::Init(UserList &userList, QString defaultRole, VhallShowType gagType, VhallShowType kickType) {
	clear();
	for (int i = 0; i < userList.size(); i++) {
		UserInfo uInfo = userList[i];
		VhallAudienceUserInfo userInfo = VhallAudienceUserInfo(uInfo);
		if (defaultRole == "") {
			userInfo.role = userInfo.role;
		}

		if (userInfo.role == USER_HOST) {
			userInfo.gagType = VhallShowType_Hide;
			userInfo.kickType = VhallShowType_Hide;
		}
		else {
			userInfo.gagType = gagType;
			userInfo.kickType = kickType;
		}

		pushBack(userInfo);
	}
}
void VhallAudienceUserInfoList_st::Debug() {
	return;
	qDebug() << "mUserIdList:" << mUserIdList;
	for (int i = 0; i < mInfoList.count(); i++) {
		qDebug() << "mInfoList:" << i << mInfoList[i].userId << mInfoList[i].userName << mInfoList[i].role;
	}
}