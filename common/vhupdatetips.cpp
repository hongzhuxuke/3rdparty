#include "vhupdatetips.h"
#include "ui_vhupdatetips.h"
#include "title_button.h"

#include <QPainter>
#include <QContextMenuEvent>
#include <QTextBlock>
#include <QTextDocument>
#include <QScrollBar>

#include "ConfigSetting.h"
#include "pathmanager.h"

VHUpdateTips::VHUpdateTips(QWidget *parent, bool bForceUpdata ) :
    CBaseDlg(parent),
	m_bForceUpdate(bForceUpdata),
    ui(new Ui::VHUpdateTips)
{
    ui->setupUi(this);
    ui->label_version->adjustSize();
    this->setWindowFlags(Qt::FramelessWindowHint |
    Qt::WindowMinimizeButtonHint |
    Qt::Dialog|
    /*Qt::Tool |*/
    Qt::WindowStaysOnTopHint); 
    //setWindowOpacity(0.5);
    //setAttribute(Qt::WA_TranslucentBackground);
    ui->textEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    setAutoFillBackground(false);

    QString versionBackImage = CPathManager::GetVersionBackImagePath();
    this->pixmap = QPixmap(versionBackImage);
    this->setFixedSize(pixmap.size());

    this->installEventFilter(this);
    ui->textEdit->installEventFilter(this);
    if (m_bForceUpdate){
	    ui->btnCloseButton->hide();
    }
    connect(ui->btnCloseButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(accept()));

    ui->textEdit->adjustSize();
    ui->textEdit->setGeometry(QRect(328, 240, 329, 27 * 15));  //四倍行距
    //ui->textEdit->setWordWrap(true);
    //ui->textEdit->setAlignment(Qt::AlignTop);
    ui->textEdit->verticalScrollBar()->hide();
    QString confPath = CPathManager::GetAppDataPath() + QString::fromStdWString(VHALL_TOOL_CONFIG);
    QString vhallHelper = ConfigSetting::ReadString(confPath, GROUP_DEFAULT, KEY_VHALL_LIVE, VHALL_LIVE_TEXT);
    setWindowTitle(vhallHelper);
}

VHUpdateTips::~VHUpdateTips()
{
    delete ui;
}

bool VHUpdateTips::eventFilter(QObject *o, QEvent *e)
{
	if (o == this || o == ui->textEdit)
	{
		if (e->type() == QEvent::MouseButtonPress) {
			this->pressPoint = this->cursor().pos();
			this->startPoint = this->pos();
		}
		else if (e->type() == QEvent::MouseMove) {
			int dx = this->cursor().pos().x() - this->pressPoint.x();
			int dy = this->cursor().pos().y() - this->pressPoint.y();
			this->move(this->startPoint.x() + dx, this->startPoint.y() + dy);
		}
	}
    return QWidget::eventFilter(o,e);
}
void VHUpdateTips::SetVersion(QString version){
	ui->label_version->setText(QStringLiteral("最新版本V") + version + QStringLiteral("已经上线了！"));
}
void VHUpdateTips::SetTip(QString str){
   	ui->textEdit->setText(str);
	
	
	QTextBlockFormat fmt;
	fmt.setLineHeight(100, QTextBlockFormat::SingleHeight);
	QTextCursor cur = ui->textEdit->textCursor();
	cur.setBlockFormat(fmt);
	ui->textEdit->setTextCursor(cur);
}

void VHUpdateTips::contextMenuEvent(QContextMenuEvent *e){
	if (e){
		e->ignore();
	}
	return;
}

void VHUpdateTips::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	//if (!m_bSplashHide) {
		painter.drawPixmap(rect(), this->pixmap);
	//}
}
