#ifndef QGGChatComboBox_H
#define QGGChatComboBox_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QPainter>
#include <QPaintEvent>

// 数据
struct STRU_GG_COMBOBOX_ITEM
{
	long long m_llUserID;//用户ID
	QString m_qsNickName;//昵称
	QPixmap m_qsIcon;//图标

	int m_RedLevel;//红钻等级
	int m_NobilityLevel;//贵族等级

public:
	// 构造
	STRU_GG_COMBOBOX_ITEM();

	// 拷贝构造
	STRU_GG_COMBOBOX_ITEM(const STRU_GG_COMBOBOX_ITEM& obj);

	//赋值构造
	STRU_GG_COMBOBOX_ITEM& operator = (const STRU_GG_COMBOBOX_ITEM& other);
};

// 菜单操作类型
enum ENUM_GG_COMBOBOX_ACTION_TYPE
{
	e_Acction_type_Add_Item,
	e_Acction_type_Delete_Item,
	e_Acction_type_Update_Item,	
};

// 菜单对象属性
enum ENUM_GG_COMBOBOX_CHAT_PROPERTY_TYPE
{
	e_Chat_Property_No_All_Users,		// 没有“大家”
	e_Chat_Property_All_Users,			// 有“大家”
};

// 菜单弹出方向
enum ENUM_GG_COMBOBOX_ORIENTATION_TYPE
{
	e_Orientation_type_top,
	e_Orientation_type_bottom,
	e_Orientation_type_left,
	e_Orientation_type_right,
};

// 菜单默认添加位置
enum ENUM_GG_COMBOBOX_ADD_POS_MODE
{
	e_Add_Pos_Mode_first,
	e_Add_Pos_Mode_end,
};

// 展示
class QGGChatComboBoxItem : public QWidget
{
	Q_OBJECT

public:
	QGGChatComboBoxItem(QWidget *parent = NULL);
	~QGGChatComboBoxItem();

public:
	void SetItemInfo(const STRU_GG_COMBOBOX_ITEM& aItem);

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);

private:
	STRU_GG_COMBOBOX_ITEM m_oItem;

	QLabel m_oIcon;
	QLabel m_oNickName;
	QHBoxLayout m_oMainLayout;

	bool m_bIsHover;
};

class QGGPopWidget : public QWidget
{
	Q_OBJECT

public:
	QGGPopWidget(QWidget *parent = NULL);
	~QGGPopWidget();

protected:

	virtual void paintEvent(QPaintEvent *);

private:
	QPixmap m_qPixmapBG;
};

class QGGChatComboBox : public QWidget
{
	Q_OBJECT

public:
	QGGChatComboBox(QWidget *parent);
	~QGGChatComboBox();

	// 获取当前选中Item信息
	STRU_GG_COMBOBOX_ITEM GetCurSeleteItemInfo();

	// 更新
	void UpdateItem(STRU_GG_COMBOBOX_ITEM& aItem, ENUM_GG_COMBOBOX_ACTION_TYPE aeType);	

	// 清空
	void CleanAll();

	// 设置属性
	void SetChatProperty(ENUM_GG_COMBOBOX_CHAT_PROPERTY_TYPE aeType);

	// 设置下拉菜单弹出方向
	void SetDropOrientation(ENUM_GG_COMBOBOX_ORIENTATION_TYPE aeType);

	// 设置菜单默认添加位置
	void SetDropAddPosMode(ENUM_GG_COMBOBOX_ADD_POS_MODE aeMode);

	// 设置下拉菜单宽度
	void SetDropDownWidth(int anWidth);

	// 设置下拉菜单高度
	void SetDropDownHeight(int anHeight);

	// 设置Item大小
	void SetItemSizeHint(QSize qSize);

	// 设置Item最大数量
	void SetItemMaxSize(int nSize);

	// 设置编辑文本是否采用省略号显示未显示全字符
	void SetIsEditTextOmit(bool abOmit);

	//设置当前文本
	void SetCurrentText(const QString &str);

protected:
	// 添加
	void AddItem(const STRU_GG_COMBOBOX_ITEM& aItem);

	// 删除
	void DeleteItem(const STRU_GG_COMBOBOX_ITEM& aItem);

	// 更新
	void UpdateItem(const STRU_GG_COMBOBOX_ITEM& aItem);

	// 设置宽度
	void UpdateWidgetWidth();

	virtual bool eventFilter(QObject *, QEvent *);

	virtual void paintEvent(QPaintEvent *);
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);

protected slots:
	void onShowGGComboBoxList();
	void onListItemPressed(QListWidgetItem*);

signals:
	void onSelectItemChange();

private:
	QHBoxLayout m_oMainLayout;
	QLineEdit m_oEdtLineEdit;
	QPushButton m_oBtnDown;

	QGGPopWidget m_oPopWidget;
	QVBoxLayout m_oPopLayout;
	QListWidget m_oListWidget;

	QPixmap m_qPixmapBG_Default;
	QPixmap m_qPixmapBG_Focus;
	QPixmap m_qPixmapBG_Disable;
	QSize m_qSize;

	mutable QPixmap  m_imgAll;
	mutable QPixmap  m_imgFace16;
	mutable QPixmap  m_imgNobility[19];
	mutable QPixmap  m_imgRedDiamand;

	STRU_GG_COMBOBOX_ITEM m_CurSelete;

	ENUM_GG_COMBOBOX_CHAT_PROPERTY_TYPE m_eProperty;	// 属性
	ENUM_GG_COMBOBOX_ORIENTATION_TYPE m_eType;			// 菜单展示位置
	ENUM_GG_COMBOBOX_ADD_POS_MODE m_eMode;			// 菜单默认添加位置

	int m_nItemHeight;
	int m_nMaxItemCount;

	int m_nFixHeight;
	int m_nFixWidth;

	bool m_bOmit;		//省略号显示
	bool m_bIsEnter;
	bool m_bIsDisable;
};


// 绘制背景
static void DrawBG(QPainter& aPainter, const QPixmap aPixmap);

//裁减图片
static bool ClipImge(const QPixmap& aoPixmapSrc, int aiIndex, int auiResPix, QPixmap& aoPixmapDest);


#endif // QGGChatComboBox_H
