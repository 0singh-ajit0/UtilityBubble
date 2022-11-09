#include "bubblewindow.h"
#include "ui_bubblewindow.h"

#include <QtGui>
#include <QDebug>
#include <QRegion>
#include <QLayout>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QMoveEvent>
#include <QTimer>

BubbleWindow::BubbleWindow(QWidget *parent)
	: QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::NoDropShadowWindowHint)
	, ui(new Ui::BubbleWindow)
{
	ui->setupUi(this);

	initApp();
	addAnimations();
	setIcons();
	setMasks();
}

QSize BubbleWindow::sizeHint() const
{
	return QSize(100, 100);
}

BubbleWindow::~BubbleWindow()
{
	delete ui;
}

void BubbleWindow::initApp()
{
	isBubbleMinimized = true;
	setAttribute(Qt::WA_TranslucentBackground);
	bubbleWidth = 100;
	bubbleHeight = 100;

	QRect MonitorInfo = QApplication::desktop()->screenGeometry();
	setGeometry(MonitorInfo.width() - bubbleWidth, MonitorInfo.height() / 2 - bubbleHeight / 2, 100, 100);

	minimizedSize = QRect(this->geometry().x(), this->geometry().y(), 100, 100);
	maximizedSize = QRect(this->geometry().x() - 300, this->geometry().y() - 150, 400, 400);

	ui->btnMain->setGeometry(0, 0, 100, 100);
	ui->btnFunction1->hide();
	ui->btnFunction2->hide();
	ui->btnFunction3->hide();
	qApp->installEventFilter(this);

	timerForWindowPos = new QTimer(this);
	connect(timerForWindowPos, &QTimer::timeout, this, QOverload<>::of(&BubbleWindow::windowMoveTimerSlot));
	timerForWindowPos->start(2000);
}

void BubbleWindow::setMasks()
{
	QRegion maskRegion(0, 0, bubbleWidth, bubbleHeight, QRegion::Ellipse);
	ui->btnMain->setMask(maskRegion);
	ui->btnFunction1->setMask(maskRegion);
	ui->btnFunction2->setMask(maskRegion);
	ui->btnFunction3->setMask(maskRegion);
}

void BubbleWindow::on_btnMain_clicked()
{
	qDebug() << "Main Button clicked";

	if (isBubbleMinimized) {
		maximizeApp();
	} else {
		minimizeApp();
	}
}

void BubbleWindow::windowMoveTimerSlot()
{

}

bool BubbleWindow::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == this && (event->type() == QEvent::MouseButtonPress)) {
		qDebug() << "Button pressed";
	} else if (watched == this && (event->type() == QEvent::MouseButtonRelease)) {
		qDebug() << "Button released";
		minimizeApp();
	} else if (watched == this && (event->type() == QEvent::Move)) {
		QMoveEvent *moveEvent = static_cast<QMoveEvent*>(event);
		qDebug() << "Window moved from " << moveEvent->oldPos();
		qDebug() << "Window moved to " << moveEvent->pos();
	}
	return false;
}

QPixmap BubbleWindow::makeImageRounded(const QPixmap &original)
{
	// getting size if the original picture is not square
	int size = qMax(original.width(), original.height());

	// creating a new transparent pixmap with equal sides
	QPixmap rounded = QPixmap(size, size);
	rounded.fill(Qt::transparent);

	// creating circle clip area
	QPainterPath path;
	path.addEllipse(rounded.rect());

	QPainter painter(&rounded);
	painter.setClipPath(path);

	// filling rounded area if needed
	painter.fillRect(rounded.rect(), Qt::black);

	// getting offsets if the originalinal picture is not square
	int x = qAbs(original.width() - size) / 2;
	int y = qAbs(original.height() - size) / 2;
	painter.drawPixmap(x, y, original.width(), original.height(), original);

	return rounded;
}

void BubbleWindow::minimizeApp()
{
	minimizeAnimation->setStartValue(maximizedSize);
	minimizeAnimation->setEndValue(minimizedSize);
	minimizeAnimation->start();

	ui->btnFunction1->hide();
	ui->btnFunction2->hide();
	ui->btnFunction3->hide();
	ui->btnMain->setGeometry(0, 0, 100, 100);

	isBubbleMinimized = true;
}

void BubbleWindow::maximizeApp()
{
//	setFixedSize(QSize(400, 400));
	maximizeAnimation->setStartValue(minimizedSize);
	maximizeAnimation->setEndValue(maximizedSize);
	maximizeAnimation->start();

//	setMinimumSize(QSize(400, 400));
//	setMaximumSize(QSize(400, 400));

	ui->btnFunction1->show();
	ui->btnFunction2->show();
	ui->btnFunction3->show();
	ui->btnMain->setGeometry(300, 150, 100, 100);

	isBubbleMinimized = false;
}

void BubbleWindow::setIcons()
{
//	QPixmap orig("/home/ajit/Pictures/icons/icons8-microphone-100.png");
//	QPixmap rounded = makeImageRounded(orig);
//	ui->btnFunction1->setIconSize(QSize(100, 100));
//	ui->btnFunction1->setIcon(QIcon(orig));

	ui->btnMain->setIcon(QIcon(":/Icons/resources/icons/close-99-99.png"));
//	ui->btnFunction1->setIcon(QIcon(":/Icons/resources/icons/close-99-99.png"));
//	QPixmap pixmap("/home/ajit/Pictures/icons/mic icon.png");
//	QIcon icn(pixmap);
//	ui->btnFunction1->setIcon(QIcon("/home/ajit/Pictures/icons/mic icon.png"));
//	ui->btnFunction1->setIcon(icn);
	ui->btnFunction2->setIcon(QIcon(":/Icons/resources/icons/close-99-99.png"));
	ui->btnFunction3->setIcon(QIcon(":/Icons/resources/icons/close-99-99.png"));
}

void BubbleWindow::addAnimations()
{
	maximizeAnimation = new QPropertyAnimation((QWidget *)this, "geometry");
	maximizeAnimation->setDuration(200);

	minimizeAnimation = new QPropertyAnimation((QWidget *)this, "geometry");
	minimizeAnimation->setDuration(200);
}

void BubbleWindow::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	QRegion maskedRegion(0, 0, width(), height(), QRegion::Ellipse);
	setMask(maskedRegion);
}


/*!
 * \brief This function toggles the default microphone
 */
void BubbleWindow::on_btnFunction1_clicked()
{
	QProcess process;
#ifdef Q_OS_LINUX
	// Code for muting microphone on linux
	process.start("amixer", QStringList() << "set" << "Capture" << "toggle");
	if (!process.waitForFinished()) {
		qDebug() << "Process ended abruptly";
	}
#elif Q_OS_WIN32
	// Code for muting microphone on windows
#endif
}


/*!
 * \brief This function takes screenshot of current Screen
 */
void BubbleWindow::on_btnFunction2_clicked()
{

}

