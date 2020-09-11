#include "stdafx.h"
#include "QGGChatComboBox.h"

#include <QBitmap>
#include <QPalette>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>

Q_DECLARE_METATYPE(STRU_GG_COMBOBOX_ITEM)


void DrawBG(QPainter& aPainter, const QPixmap aPixmap, const QSize aSize)
{
	// ������
	int lnShadowWidth = 6;
	if(!aPixmap.isNull())
	{
		QRect loWindowRect( 0, 0, aSize.width(), aSize.height() );
		// �����Ͻ�
		{
			QRect loDestTop( 0, 0, lnShadowWidth, lnShadowWidth );
			QRect loSrcTop( 0, 0, lnShadowWidth, lnShadowWidth );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}

		// ���ϱ�
		{
			QRect loDestTop( lnShadowWidth, 0, aSize.width() - lnShadowWidth*2, lnShadowWidth );
			QRect loSrcTop( lnShadowWidth, 0, aPixmap.width() - lnShadowWidth*2, lnShadowWidth );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}

		// �����Ͻ�
		{
			QRect loDestTop( aSize.width() - lnShadowWidth, 0, lnShadowWidth, lnShadowWidth );
			QRect loSrcTop( aPixmap.width() - lnShadowWidth, 0, lnShadowWidth, lnShadowWidth );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}

		// �����
		{
			QRect loDestTop( 0, lnShadowWidth, lnShadowWidth, aSize.height() - lnShadowWidth*2 );
			QRect loSrcTop( 0, lnShadowWidth, lnShadowWidth, aPixmap.height() - lnShadowWidth*2 );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}

		// ���ұ�
		{
			QRect loDestTop( aSize.width() - lnShadowWidth, lnShadowWidth, lnShadowWidth, aSize.height() - lnShadowWidth*2 );
			QRect loSrcTop( aPixmap.width() - lnShadowWidth, lnShadowWidth, lnShadowWidth, aPixmap.height() - lnShadowWidth*2 );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}

		// �����½�
		{
			QRect loDestTop( 0, aSize.height() - lnShadowWidth, lnShadowWidth, lnShadowWidth );
			QRect loSrcTop( 0, aPixmap.height() - lnShadowWidth, lnShadowWidth, lnShadowWidth );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}

		// ���±�
		{
			QRect loDestTop( lnShadowWidth, aSize.height() - lnShadowWidth, aSize.width() - lnShadowWidth*2, lnShadowWidth );
			QRect loSrcTop( lnShadowWidth, aPixmap.height() - lnShadowWidth, aPixmap.width() - lnShadowWidth*2, lnShadowWidth );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}

		// �����½�
		{
			QRect loDestTop( aSize.width() - lnShadowWidth, aSize.height() - lnShadowWidth, lnShadowWidth, lnShadowWidth );
			QRect loSrcTop( aPixmap.width() - lnShadowWidth, aPixmap.height() - lnShadowWidth, lnShadowWidth, lnShadowWidth );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}

		// ���м���
		{
			QRect loDestTop( lnShadowWidth, lnShadowWidth, aSize.width() - lnShadowWidth*2, aSize.height() - lnShadowWidth*2 );
			QRect loSrcTop( lnShadowWidth, lnShadowWidth, aPixmap.width() - lnShadowWidth*2, aPixmap.height() - lnShadowWidth*2 );
			aPainter.drawPixmap( loDestTop, aPixmap, loSrcTop );
		}
	}
}

//�ü�ͼƬ
bool ClipImge(const QPixmap& aoPixmapSrc, int aiIndex, int auiResPix, QPixmap& aoPixmapDest)
{
	if(aoPixmapSrc.isNull())
	{
		return false;
	}

	aoPixmapDest = aoPixmapSrc.copy(aiIndex * auiResPix, 0, auiResPix, auiResPix);
	aoPixmapDest.setMask(aoPixmapDest.createMaskFromColor(QColor(255, 0, 255)));
	if(aoPixmapDest.isNull())
	{
		return false;
	}

	return true;
}


//==================================================================//
STRU_GG_COMBOBOX_ITEM::STRU_GG_COMBOBOX_ITEM()
{
	m_llUserID = 0;
	m_RedLevel = -1;
	m_NobilityLevel = -1;
}

// ��������
STRU_GG_COMBOBOX_ITEM::STRU_GG_COMBOBOX_ITEM(const STRU_GG_COMBOBOX_ITEM& obj)
{
	m_llUserID = obj.m_llUserID;
	m_qsNickName = obj.m_qsNickName;
	m_qsIcon = obj.m_qsIcon;
	m_RedLevel = obj.m_RedLevel;
	m_NobilityLevel = obj.m_NobilityLevel;
}

//��ֵ����
STRU_GG_COMBOBOX_ITEM& STRU_GG_COMBOBOX_ITEM::operator =(const STRU_GG_COMBOBOX_ITEM& other)
{
	m_llUserID = other.m_llUserID;
	m_qsNickName = other.m_qsNickName;
	m_qsIcon = other.m_qsIcon;
	m_RedLevel = other.m_RedLevel;
	m_NobilityLevel = other.m_NobilityLevel;

	return *this;
}

//==================================================================//

QGGChatComboBoxItem::QGGChatComboBoxItem(QWidget *parent)
	:QWidget(parent)
	, m_bIsHover(false)
{
	setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

	m_oNickName.setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
	m_oNickName.setTextFormat(Qt::PlainText);

	m_oIcon.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_oIcon.setFixedWidth(16);
	m_oIcon.setFixedHeight(16);

	m_oMainLayout.addWidget(&m_oIcon);
	m_oMainLayout.addWidget(&m_oNickName);

	m_oMainLayout.setContentsMargins(6, 0, 6, 0);
	m_oMainLayout.setSpacing(5);

	setLayout(&m_oMainLayout);	
	
}

QGGChatComboBoxItem::~QGGChatComboBoxItem()
{

}

void QGGChatComboBoxItem::SetItemInfo(const STRU_GG_COMBOBOX_ITEM& aItem)
{
	m_oItem = aItem;

	m_oIcon.setPixmap(aItem.m_qsIcon);
	
	// ��Ҳ���¼ID
	if (0 == aItem.m_llUserID)
	{
		QString lqsNickName = QString("%1").arg(aItem.m_qsNickName);
		m_oNickName.setText(lqsNickName);
	}
	else
	{
		QString lqsNickName = QString("%1 (%2)").arg(aItem.m_qsNickName).arg(QString::number(aItem.m_llUserID));
		m_oNickName.setText(lqsNickName);
	}
}

void QGGChatComboBoxItem::paintEvent(QPaintEvent * event)
{
	__super::paintEvent(event);
	QPainter lPainter(this);
	lPainter.save();

	QColor qColor;
	if (m_bIsHover)
	{
		qColor = QColor(90, 173, 245);		
	}
	else
	{
		qColor = QColor(255, 255, 255);
	}

	lPainter.setPen(qColor);
	lPainter.setBrush(QBrush(qColor));
	lPainter.drawRect(rect());	

	lPainter.restore();
}

void QGGChatComboBoxItem::enterEvent(QEvent *)
{
	m_bIsHover = true;

	update();
}

void QGGChatComboBoxItem::leaveEvent(QEvent *)
{
	m_bIsHover = false;

	update();
}

//============================== �˵� ====================================//

QGGPopWidget::QGGPopWidget(QWidget *parent)
	:QWidget(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);

	setWindowFlags(Qt::Popup | Qt::MSWindowsOwnDC | Qt::FramelessWindowHint);
}

QGGPopWidget::~QGGPopWidget()
{

}

void QGGPopWidget::paintEvent(QPaintEvent * event)
{
	__super::paintEvent(event);
	QPainter lPainter(this);
	lPainter.save();

	if (!m_qPixmapBG.load(":/skin/window/float_window_and_drop_list.png"))
	{
		return;
	}

	QSize qSize(this->width(), this->height());
	DrawBG(lPainter, m_qPixmapBG, qSize);

	lPainter.restore();
}

//============================== �༭�� ====================================//

QGGChatComboBox::QGGChatComboBox(QWidget *parent)
	: QWidget(parent)
	, m_bIsEnter(false)
	, m_bIsDisable(false)
	, m_oMainLayout(NULL)
	, m_oEdtLineEdit(NULL)
	, m_oBtnDown(NULL)
	, m_oPopWidget(NULL)
	, m_oPopLayout(NULL)
	, m_oListWidget(NULL)
{
	// �༭
	m_oEdtLineEdit.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_oEdtLineEdit.setReadOnly(true);
	m_oEdtLineEdit.setContextMenuPolicy(Qt::NoContextMenu);
	m_oEdtLineEdit.setCursor(Qt::PointingHandCursor);

	m_oEdtLineEdit.installEventFilter(this);

	m_oBtnDown.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_oBtnDown.setFixedWidth(16);
	m_oBtnDown.setFixedHeight(16);
	m_oBtnDown.setStyleSheet("QPushButton{outline:none;border-image: url(:/skin/btn/down.png);}");

	m_oMainLayout.addWidget(&m_oEdtLineEdit);
	m_oMainLayout.addWidget(&m_oBtnDown);
	m_oMainLayout.setContentsMargins(5, 2, 2, 2);
	setLayout(&m_oMainLayout);

	// �˵�
	m_oListWidget.setFrameShape(QFrame::NoFrame);
	m_oListWidget.setViewMode(QListView::ListMode);
	m_oListWidget.setMovement(QListView::Static);
	m_oListWidget.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_oListWidget.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	// ȥ���������Ҽ��˵�
	QScrollBar* pBar = NULL;
	pBar = m_oListWidget.verticalScrollBar();
	if (NULL != pBar)
	{
		pBar->setContextMenuPolicy(Qt::NoContextMenu);
	}

	pBar = m_oListWidget.horizontalScrollBar();
	if (NULL != pBar)
	{
		pBar->setContextMenuPolicy(Qt::NoContextMenu);
	}

	// �˵�����
	m_oPopLayout.addWidget(&m_oListWidget);
	m_oPopLayout.setContentsMargins(10, 10, 10, 10);

	m_oPopWidget.setLayout(&m_oPopLayout);
	m_oPopWidget.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// ��ť
	connect(&m_oBtnDown, SIGNAL(clicked()), this, SLOT(onShowGGComboBoxList()));

	// ��
	connect(&m_oListWidget, SIGNAL(itemPressed(QListWidgetItem*)), SLOT(onListItemPressed(QListWidgetItem*)));

	// Ĭ���и�
	m_nItemHeight = 24;

	// Ĭ�ϲ���������
	m_nMaxItemCount = 0;
	m_nFixHeight = 0;
	m_nFixWidth = 0;

	m_bOmit = true;

	// �˵����ԣ�Ĭ��Ϊ������"���"��
	m_eProperty = e_Chat_Property_No_All_Users;

	// �˵���ʾ��ʽ��Ĭ��������ʾ��
	m_eType = e_Orientation_type_bottom;

	// �˵�Ĭ�������λ��
	m_eMode = e_Add_Pos_Mode_first;

	// Ĭ��̬
	if (!m_qPixmapBG_Default.load(":/skin/edt/input_Default.png"))
	{
		return;
	}

	// ����̬
	if (!m_qPixmapBG_Focus.load(":/skin/edt/input_Focus.png"))
	{
		return;
	}

	// ����̬
	if (!m_qPixmapBG_Disable.load(":/skin/edt/input_Default.png"))
	{
		return;
	}

	m_oEdtLineEdit.setStyleSheet("QLineEdit{border: none; border-image:none; background-color:transparent;}");

	m_imgAll.load(":/icon/res/all.png");
	m_imgFace16.load(":/icon/res/def_face_16.png");
	m_imgRedDiamand.load(":/icon/res/red_diamand.png");

	// ��ʿ
	m_imgNobility[0].load(":/icon/res/Nobility7.png");
	m_imgNobility[1].load(":/icon/res/Nobility7.png");
	m_imgNobility[2].load(":/icon/res/Nobility7.png");
	// �о�
	m_imgNobility[3].load(":/icon/res/Nobility6.png");
	m_imgNobility[4].load(":/icon/res/Nobility6.png");
	m_imgNobility[5].load(":/icon/res/Nobility6.png");
	//�Ӿ�
	m_imgNobility[6].load(":/icon/res/Nobility5.png");
	m_imgNobility[7].load(":/icon/res/Nobility5.png");
	m_imgNobility[8].load(":/icon/res/Nobility5.png");
	//����
	m_imgNobility[9].load(":/icon/res/Nobility4.png");
	m_imgNobility[10].load(":/icon/res/Nobility4.png");
	m_imgNobility[11].load(":/icon/res/Nobility4.png");
	//���
	m_imgNobility[12].load(":/icon/res/Nobility3.png");
	m_imgNobility[13].load(":/icon/res/Nobility3.png");
	m_imgNobility[14].load(":/icon/res/Nobility3.png");
	//����
	m_imgNobility[15].load(":/icon/res/Nobility2.png");
	m_imgNobility[16].load(":/icon/res/Nobility2.png");
	//����
	m_imgNobility[17].load(":/icon/res/Nobility1.png");
	m_imgNobility[18].load(":/icon/res/Nobility0.png");
}

QGGChatComboBox::~QGGChatComboBox()
{
	CleanAll();
}

bool QGGChatComboBox::eventFilter(QObject *pWatched, QEvent *pEvent)
{
	if (pWatched == &m_oEdtLineEdit && pEvent->type() == QEvent::MouseButtonPress)
	{
		onShowGGComboBoxList();
	}

	return QWidget::eventFilter(pWatched,pEvent);     // ����¼������ϲ�Ի���
}

void QGGChatComboBox::paintEvent(QPaintEvent * event)
{
	__super::paintEvent(event);
	QPainter lPainter(this);
	lPainter.save();

	QSize qSize(this->width(), this->height());
	if (m_bIsDisable)
	{
		DrawBG(lPainter, m_qPixmapBG_Disable, qSize);
	}
	else if (m_bIsEnter)
	{
		DrawBG(lPainter, m_qPixmapBG_Focus, qSize);
	}
	else
	{
		DrawBG(lPainter, m_qPixmapBG_Default, qSize);	
	}

	lPainter.restore();
}

void QGGChatComboBox::enterEvent(QEvent *pEvent)
{
	m_bIsEnter = true;
	update();

	QWidget::enterEvent(pEvent);
}

void QGGChatComboBox::leaveEvent(QEvent *pEvent)
{
	m_bIsEnter = false;
	update();

	QWidget::leaveEvent(pEvent);
}

// ��ȡ��ǰѡ��Item��Ϣ
STRU_GG_COMBOBOX_ITEM QGGChatComboBox::GetCurSeleteItemInfo()
{
	return m_CurSelete;
}

// ����
void QGGChatComboBox::UpdateItem(STRU_GG_COMBOBOX_ITEM& aItem, ENUM_GG_COMBOBOX_ACTION_TYPE aeType)
{
	switch(aeType)
	{
	case e_Acction_type_Add_Item:
		{
			// �����Ҫ��ӵ��û��͵�ǰѡ���û���ͬ���Ҳ�Ϊ��ң���ֱ�ӷ���
			if (aItem.m_llUserID == m_CurSelete.m_llUserID && aItem.m_llUserID != 0)
			{
				return;
			}

			DeleteItem(aItem);

			if (0 == aItem.m_llUserID)
			{
				aItem.m_qsIcon = m_imgAll;
			}
			else
			{
				if (aItem.m_NobilityLevel > -1)
				{
					QPixmap qIcon;
					if( aItem.m_NobilityLevel >= 0 &&  aItem.m_NobilityLevel <= 18 )
					{
						aItem.m_qsIcon = m_imgNobility[aItem.m_NobilityLevel].copy(0,0,16,16);
					}
					else
					{
						aItem.m_qsIcon = m_imgFace16;
					}
				}
				else if (aItem.m_RedLevel > -1)
				{
					QPixmap qIcon;
					if(ClipImge(m_imgRedDiamand, aItem.m_RedLevel, 16, qIcon))
					{
						aItem.m_qsIcon = qIcon;
					}
					else
					{
						aItem.m_qsIcon = m_imgFace16;
					}
				}
				else
				{
					aItem.m_qsIcon = m_imgFace16;
				}
			}
			
			AddItem(aItem);

			int fontSize = m_oEdtLineEdit.fontMetrics().width(aItem.m_qsNickName);
			if( m_bOmit && fontSize >= m_oEdtLineEdit.width() )	
			{
				QString str = m_oEdtLineEdit.fontMetrics().elidedText(aItem.m_qsNickName, Qt::ElideRight, m_oEdtLineEdit.width());
				m_oEdtLineEdit.setText(str);
			}
			else
			{
				m_oEdtLineEdit.setText(aItem.m_qsNickName);
			}
		}

		break;

	case e_Acction_type_Delete_Item:
		{
			DeleteItem(aItem);
		}

		break;

	case e_Acction_type_Update_Item:
		{

			if (0 == aItem.m_llUserID)
			{
				aItem.m_qsIcon = m_imgAll;
			}
			else
			{
				if (aItem.m_NobilityLevel > -1)
				{
					QPixmap qIcon;
					if( aItem.m_NobilityLevel >= 0 &&  aItem.m_NobilityLevel <= 18 )
					{
						aItem.m_qsIcon = m_imgNobility[aItem.m_NobilityLevel].copy(0,0,16,16);
					}
					else
					{
						aItem.m_qsIcon = m_imgFace16;
					}
				}
				else if (aItem.m_RedLevel > -1)
				{
					QPixmap qIcon;
					if(ClipImge(m_imgRedDiamand, aItem.m_RedLevel, 16, qIcon))
					{
						aItem.m_qsIcon = qIcon;
					}
					else
					{
						aItem.m_qsIcon = m_imgFace16;
					}
				}
				else
				{
					aItem.m_qsIcon = m_imgFace16;
				}
			}

			UpdateItem(aItem);
		}
		break;
	}

	int nCount = m_oListWidget.count();
	if (0 >= nCount)
	{
		m_oPopWidget.setFixedWidth(0);
		m_oPopWidget.setFixedHeight(0);
		return;
	}

	int nHeight = m_nItemHeight * nCount + 20;
	if (m_nFixHeight != 0 && m_nFixHeight < nHeight)
	{
		m_oPopWidget.setFixedHeight(m_nFixHeight);
	}
	else
	{
		m_oPopWidget.setFixedHeight(nHeight);
	}

	// �������
	UpdateWidgetWidth();
}

// ���
void QGGChatComboBox::AddItem(const STRU_GG_COMBOBOX_ITEM& aItem)
{
	QListWidgetItem* pListItem = new QListWidgetItem;
	pListItem->setSizeHint(m_qSize);

	QVariant lqValue;
	lqValue.setValue(aItem);
	pListItem->setData(Qt::UserRole, lqValue);

	QGGChatComboBoxItem* pItem = new QGGChatComboBoxItem;
	pItem->SetItemInfo(aItem);	

	int nCount = m_oListWidget.count();
	if (m_eMode == e_Add_Pos_Mode_first)
	{
		if (e_Chat_Property_All_Users == m_eProperty)
		{
			// ������Ҵӵڶ�λ��ʼ����
			m_oListWidget.insertItem(1, pListItem);
		}
		else
		{
			// ��������Ҵӵ�һλ��ʼ����
			m_oListWidget.insertItem(0, pListItem);
		}
	}
	else if (m_eMode == e_Add_Pos_Mode_end)
	{
		// �嵽���һλ
		m_oListWidget.insertItem(nCount, pListItem);
	}
	else	// Ĭ��Ϊ:e_Add_Pos_Mode_first
	{
		if (e_Chat_Property_All_Users == m_eProperty)
		{
			// ������Ҵӵڶ�λ��ʼ����
			m_oListWidget.insertItem(1, pListItem);
		}
		else
		{
			// ��������Ҵӵ�һλ��ʼ����
			m_oListWidget.insertItem(0, pListItem);
		}	
	}
	m_oListWidget.setItemWidget(pListItem, pItem);

	m_CurSelete = aItem;
	if (0 != m_nMaxItemCount && nCount > m_nMaxItemCount - 1)
	{
		QListWidgetItem* pListItem = NULL;
		if (m_eMode == e_Add_Pos_Mode_first)
		{
			pListItem = m_oListWidget.takeItem(nCount -1);
		}
		else if (m_eMode == e_Add_Pos_Mode_end)
		{
			if (e_Chat_Property_All_Users == m_eProperty)
			{
				pListItem = m_oListWidget.takeItem(1);
			}
			else
			{
				pListItem = m_oListWidget.takeItem(0);
			}
		}
		else
		{
			pListItem = m_oListWidget.takeItem(nCount -1);
		}

		if (NULL != pListItem)
		{
			QGGChatComboBoxItem *pItem = (QGGChatComboBoxItem*)(m_oListWidget.itemWidget(pListItem));
			if (NULL != pItem)
			{
				delete pItem;
				pItem = NULL;
			}

			delete pListItem;
			pListItem = NULL;
		}
	}
}

// ɾ��
void QGGChatComboBox::DeleteItem(const STRU_GG_COMBOBOX_ITEM& aItem)
{
	int nCount = m_oListWidget.count();
	for (int i = 0; i < nCount; i++)
	{
		QListWidgetItem* pListItem = m_oListWidget.item(i);

		QVariant lqValue = pListItem->data(Qt::UserRole);

		STRU_GG_COMBOBOX_ITEM loData;
		if (lqValue.canConvert<STRU_GG_COMBOBOX_ITEM>())
		{
			 loData = lqValue.value<STRU_GG_COMBOBOX_ITEM>();
		}

		if (loData.m_llUserID != aItem.m_llUserID)
		{
			continue;
		}

		QGGChatComboBoxItem *pItem = (QGGChatComboBoxItem*)(m_oListWidget.itemWidget(pListItem));
		delete pItem;
		pItem = NULL;

		QListWidgetItem * lpItemTaked = m_oListWidget.takeItem(i);
		if( NULL != lpItemTaked )
		{
			delete lpItemTaked;
			lpItemTaked = NULL;
		}
	
		// ��ǰѡ���뽫Ҫɾ������ͬ
		if (m_CurSelete.m_llUserID == aItem.m_llUserID)
		{
			if (e_Chat_Property_All_Users == m_eProperty)
			{
				m_oEdtLineEdit.setText("���");

				STRU_GG_COMBOBOX_ITEM loDataAll;
				loDataAll.m_llUserID = 0;
				loDataAll.m_qsNickName = "���";
				loDataAll.m_qsIcon = QPixmap();
				loDataAll.m_NobilityLevel = 0;
				loDataAll.m_RedLevel = 0;
				m_CurSelete = loDataAll;
			}
			else
			{
				m_oEdtLineEdit.setText("");

				STRU_GG_COMBOBOX_ITEM loDataAll;
				loDataAll.m_llUserID = 0;
				loDataAll.m_qsNickName = "";
				loDataAll.m_qsIcon = QPixmap();
				loDataAll.m_NobilityLevel = 0;
				loDataAll.m_RedLevel = 0;
				m_CurSelete = loDataAll;
			}

			// ֪ͨѡ������仯
			emit onSelectItemChange();
		}
		break;
	}
}

// ����
void QGGChatComboBox::UpdateItem(const STRU_GG_COMBOBOX_ITEM& aItem)
{
	int nCount = m_oListWidget.count();
	for (int i = 0; i < nCount; i++)
	{
		QListWidgetItem* pListItem = m_oListWidget.item(i);

		QVariant lqValue = pListItem->data(Qt::UserRole);

		STRU_GG_COMBOBOX_ITEM loData;
		if (lqValue.canConvert<STRU_GG_COMBOBOX_ITEM>())
		{
			loData = lqValue.value<STRU_GG_COMBOBOX_ITEM>();
		}

		if (loData.m_llUserID != aItem.m_llUserID)
		{
			continue;
		}

		lqValue.setValue(aItem);
		pListItem->setData(Qt::UserRole, lqValue);

		QGGChatComboBoxItem *pItem = (QGGChatComboBoxItem*)(m_oListWidget.itemWidget(pListItem));
		if (NULL != pItem)
		{
			pItem->SetItemInfo(aItem);
		}
		
		break;
	}
}

// ���
void QGGChatComboBox::CleanAll()
{
	while(0 != m_oListWidget.count())
	{
		QListWidgetItem* pListItem = m_oListWidget.item(0);

		if (NULL != pListItem)
		{
			QGGChatComboBoxItem *pItem = (QGGChatComboBoxItem*)(m_oListWidget.itemWidget(pListItem));

			if (NULL != pItem)
			{
				delete pItem;
				pItem = NULL;
			}

			delete pListItem;
			pListItem = NULL;
		}

		m_oListWidget.takeItem(0);
	}

	m_oListWidget.clear();
}

//==================================================================//

// ��������
void QGGChatComboBox::SetChatProperty(ENUM_GG_COMBOBOX_CHAT_PROPERTY_TYPE aeType)
{
	m_eProperty = aeType;
}

// ���������˵���������
void QGGChatComboBox::SetDropOrientation(ENUM_GG_COMBOBOX_ORIENTATION_TYPE aeType)
{
	m_eType = aeType;
}

// ���ò˵�Ĭ�����λ��
void QGGChatComboBox::SetDropAddPosMode(ENUM_GG_COMBOBOX_ADD_POS_MODE aeMode)
{
	m_eMode = aeMode;
}

// ���������˵����
void QGGChatComboBox::SetDropDownWidth(int anWidth)
{
	m_oPopWidget.setMaximumWidth(anWidth);
	m_oPopWidget.setMinimumWidth(anWidth);
	m_oPopWidget.setFixedWidth(anWidth);

	m_nFixWidth = anWidth;
}

// ���������˵��߶�
void QGGChatComboBox::SetDropDownHeight(int anHeight)
{
	m_oPopWidget.setMaximumHeight(anHeight);
	m_oPopWidget.setMinimumHeight(anHeight);
	m_oPopWidget.setFixedHeight(anHeight);

	m_nFixHeight = anHeight;
}

// ����Item��С
void QGGChatComboBox::SetItemSizeHint(QSize qSize)
{
	m_qSize = qSize;
}

// ����Item�������
void QGGChatComboBox::SetItemMaxSize(int nSize)
{
	m_nMaxItemCount = nSize;
}

// ���ñ༭�ı��Ƿ����ʡ�Ժ���ʾδ��ʾȫ�ַ�
void QGGChatComboBox::SetIsEditTextOmit(bool abOmit)
{
	m_bOmit = abOmit;
}

void QGGChatComboBox::onShowGGComboBoxList()
{
	// ����ֱ�ӷ���
	int nCount = m_oListWidget.count();
	if (0 >= nCount)
	{
		return;
	}

	int lnDesktopWidth = QApplication::desktop()->width();

	switch(m_eType)
	{
	case e_Orientation_type_top:
		{
			QPoint qPoint = m_oEdtLineEdit.mapToGlobal(QPoint(0, 0));

			int lnX = qPoint.x() - 5;
			int lnRightPos = lnX+m_oPopWidget.width();
			if( lnRightPos > lnDesktopWidth )
				lnX -= (lnRightPos - lnDesktopWidth);

			m_oPopWidget.move( lnX, qPoint.y() - m_oPopWidget.height());
		}
		break;
	case e_Orientation_type_bottom:
		{
			QPoint qPoint = m_oEdtLineEdit.mapToGlobal(QPoint(0, m_oEdtLineEdit.height()));

			int lnX = qPoint.x() - 5;
			int lnRightPos = lnX+m_oPopWidget.width();
			if( lnRightPos > lnDesktopWidth )
				lnX -= (lnRightPos - lnDesktopWidth);

			m_oPopWidget.move( lnX, qPoint.y());
		}
		break;
	case e_Orientation_type_left:
		{
			QPoint qPoint = m_oEdtLineEdit.mapToGlobal(QPoint(0, 0));

			int lnX = qPoint.x() - m_oPopWidget.width();
			int lnRightPos = lnX+m_oPopWidget.width();
			if( lnRightPos > lnDesktopWidth )
				lnX -= (lnRightPos - lnDesktopWidth);

			m_oPopWidget.move( lnX, qPoint.y() );
		}
		break;
	case e_Orientation_type_right:
		{
			QPoint qPoint = m_oBtnDown.mapToGlobal(QPoint(m_oBtnDown.width(), 0));

			int lnX = qPoint.x();
			int lnRightPos = lnX+m_oPopWidget.width();
			if( lnRightPos > lnDesktopWidth )
				lnX -= (lnRightPos - lnDesktopWidth);

			m_oPopWidget.move( lnX, qPoint.y() );
		}
		break;
	}

	m_oPopWidget.show();

	int nCurStep = 0;
	QScrollBar *pScrollBar = m_oListWidget.verticalScrollBar();
	if (m_eMode == e_Add_Pos_Mode_first)
	{
		nCurStep = 0;
	}
	else if (m_eMode == e_Add_Pos_Mode_end)
	{
		nCurStep = pScrollBar->maximum();
	}
	else
	{
		nCurStep = 0;
	}
	pScrollBar->setSliderPosition(nCurStep);	
}

void QGGChatComboBox::onListItemPressed(QListWidgetItem* pitem)
{
	if(NULL == pitem)
	{
		return;
	}

	QVariant lqValue = pitem->data(Qt::UserRole);

	if (lqValue.canConvert<STRU_GG_COMBOBOX_ITEM>())
	{
		STRU_GG_COMBOBOX_ITEM data;
		data = lqValue.value<STRU_GG_COMBOBOX_ITEM>();

		// ѡ����Ǵ�һ��ߵ�ǰ�û��������κδ���
		if (0 != data.m_llUserID && m_CurSelete.m_llUserID != data.m_llUserID)
		{
			// ���µ���λ�ã���ɾ��������ӣ�		
			DeleteItem(data);
			AddItem(data);
		}

		// �����ı�����ʾ��Ϣ
		int fontSize = m_oEdtLineEdit.fontMetrics().width(data.m_qsNickName);
		if(m_bOmit && fontSize >= m_oEdtLineEdit.width())	
		{
			QString str = m_oEdtLineEdit.fontMetrics().elidedText(data.m_qsNickName, Qt::ElideRight, m_oEdtLineEdit.width());
			m_oEdtLineEdit.setText(str);
		}
		else
		{
			m_oEdtLineEdit.setText(data.m_qsNickName);
		}

		m_oEdtLineEdit.setFocus();
		m_oPopWidget.hide();

		m_CurSelete = data;
		emit onSelectItemChange();
	}
}

// ���ÿ��
void QGGChatComboBox::UpdateWidgetWidth()
{
	if (0 != m_nFixWidth)
	{
		m_oPopWidget.setFixedWidth(m_nFixWidth);
	}
	else
	{
		int lnMaxWidth = this->width() + 8;
		int lnCount = m_oListWidget.count();
		for (int i = 0; i < lnCount; i++)
		{
			QListWidgetItem* pListItem = m_oListWidget.item(i);

			QVariant lqValue = pListItem->data(Qt::UserRole);

			if (!lqValue.canConvert<STRU_GG_COMBOBOX_ITEM>())
			{
				continue;
			}

			STRU_GG_COMBOBOX_ITEM data;
			data = lqValue.value<STRU_GG_COMBOBOX_ITEM>();

			QFontMetrics fm(pListItem->font());

			QString lqsNickName = QString("%1 (%2)").arg(data.m_qsNickName).arg(QString::number(data.m_llUserID));

			int lnWidth = fm.width(lqsNickName) + 60;

			if (lnWidth > lnMaxWidth)
			{
				lnMaxWidth = lnWidth;
			}
		}

		m_oPopWidget.setFixedWidth(lnMaxWidth);
	}
}

void QGGChatComboBox::SetCurrentText(const QString &str)
{
	m_oEdtLineEdit.setText(str);

}