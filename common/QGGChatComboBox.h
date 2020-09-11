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

// ����
struct STRU_GG_COMBOBOX_ITEM
{
	long long m_llUserID;//�û�ID
	QString m_qsNickName;//�ǳ�
	QPixmap m_qsIcon;//ͼ��

	int m_RedLevel;//����ȼ�
	int m_NobilityLevel;//����ȼ�

public:
	// ����
	STRU_GG_COMBOBOX_ITEM();

	// ��������
	STRU_GG_COMBOBOX_ITEM(const STRU_GG_COMBOBOX_ITEM& obj);

	//��ֵ����
	STRU_GG_COMBOBOX_ITEM& operator = (const STRU_GG_COMBOBOX_ITEM& other);
};

// �˵���������
enum ENUM_GG_COMBOBOX_ACTION_TYPE
{
	e_Acction_type_Add_Item,
	e_Acction_type_Delete_Item,
	e_Acction_type_Update_Item,	
};

// �˵���������
enum ENUM_GG_COMBOBOX_CHAT_PROPERTY_TYPE
{
	e_Chat_Property_No_All_Users,		// û�С���ҡ�
	e_Chat_Property_All_Users,			// �С���ҡ�
};

// �˵���������
enum ENUM_GG_COMBOBOX_ORIENTATION_TYPE
{
	e_Orientation_type_top,
	e_Orientation_type_bottom,
	e_Orientation_type_left,
	e_Orientation_type_right,
};

// �˵�Ĭ�����λ��
enum ENUM_GG_COMBOBOX_ADD_POS_MODE
{
	e_Add_Pos_Mode_first,
	e_Add_Pos_Mode_end,
};

// չʾ
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

	// ��ȡ��ǰѡ��Item��Ϣ
	STRU_GG_COMBOBOX_ITEM GetCurSeleteItemInfo();

	// ����
	void UpdateItem(STRU_GG_COMBOBOX_ITEM& aItem, ENUM_GG_COMBOBOX_ACTION_TYPE aeType);	

	// ���
	void CleanAll();

	// ��������
	void SetChatProperty(ENUM_GG_COMBOBOX_CHAT_PROPERTY_TYPE aeType);

	// ���������˵���������
	void SetDropOrientation(ENUM_GG_COMBOBOX_ORIENTATION_TYPE aeType);

	// ���ò˵�Ĭ�����λ��
	void SetDropAddPosMode(ENUM_GG_COMBOBOX_ADD_POS_MODE aeMode);

	// ���������˵����
	void SetDropDownWidth(int anWidth);

	// ���������˵��߶�
	void SetDropDownHeight(int anHeight);

	// ����Item��С
	void SetItemSizeHint(QSize qSize);

	// ����Item�������
	void SetItemMaxSize(int nSize);

	// ���ñ༭�ı��Ƿ����ʡ�Ժ���ʾδ��ʾȫ�ַ�
	void SetIsEditTextOmit(bool abOmit);

	//���õ�ǰ�ı�
	void SetCurrentText(const QString &str);

protected:
	// ���
	void AddItem(const STRU_GG_COMBOBOX_ITEM& aItem);

	// ɾ��
	void DeleteItem(const STRU_GG_COMBOBOX_ITEM& aItem);

	// ����
	void UpdateItem(const STRU_GG_COMBOBOX_ITEM& aItem);

	// ���ÿ��
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

	ENUM_GG_COMBOBOX_CHAT_PROPERTY_TYPE m_eProperty;	// ����
	ENUM_GG_COMBOBOX_ORIENTATION_TYPE m_eType;			// �˵�չʾλ��
	ENUM_GG_COMBOBOX_ADD_POS_MODE m_eMode;			// �˵�Ĭ�����λ��

	int m_nItemHeight;
	int m_nMaxItemCount;

	int m_nFixHeight;
	int m_nFixWidth;

	bool m_bOmit;		//ʡ�Ժ���ʾ
	bool m_bIsEnter;
	bool m_bIsDisable;
};


// ���Ʊ���
static void DrawBG(QPainter& aPainter, const QPixmap aPixmap);

//�ü�ͼƬ
static bool ClipImge(const QPixmap& aoPixmapSrc, int aiIndex, int auiResPix, QPixmap& aoPixmapDest);


#endif // QGGChatComboBox_H
