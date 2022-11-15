#include "bubblewindow.h"
#include "ui_bubblewindow.h"

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#define BUBBLE_WIDTH    64
#define BUBBLE_HEIGHT   64
#define BUBBLE_GAP      20

#include <QtGui>
#include <QDebug>
#include <QRegion>
#include <QLayout>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QMoveEvent>
#include <QTimer>
#include <QScreen>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>

BubbleWindow::BubbleWindow(QWidget *parent)
	: QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::NoDropShadowWindowHint)
	, ui(new Ui::BubbleWindow)
{
	ui->setupUi(this);

	initApp();
	addAnimations();
    setIcons();
}

QSize BubbleWindow::sizeHint() const
{
    return QSize(BUBBLE_WIDTH, BUBBLE_HEIGHT);
}

BubbleWindow::~BubbleWindow()
{
	delete ui;
}

void BubbleWindow::initApp()
{
	isBubbleMinimized = true;
	setAttribute(Qt::WA_TranslucentBackground);
    bubbleWidth = BUBBLE_WIDTH;
    bubbleHeight = BUBBLE_HEIGHT;
    bubbleGap = BUBBLE_GAP;
    iconSize = QSize(bubbleWidth, bubbleHeight);

	QRect MonitorInfo = QApplication::desktop()->screenGeometry();
    setGeometry(MonitorInfo.width() - bubbleWidth, MonitorInfo.height() / 2 - bubbleHeight / 2, bubbleWidth, bubbleHeight);

    minimizedSize = QRect(this->geometry().x(),
                          this->geometry().y(),
                          bubbleWidth,
                          bubbleHeight);

    maximizedSize = QRect(
                this->geometry().x() - (bubbleWidth + bubbleGap),
                this->geometry().y() - (bubbleHeight + bubbleGap),
                (2*bubbleWidth) + bubbleGap,
                (3*bubbleHeight) + (2*bubbleGap)
                );

    ui->btnMain->setGeometry(0, 0, bubbleWidth, bubbleHeight);
    ui->btnMain->setStyleSheet("QPushButton {"
                                    "background-color: rgba(0, 0, 0, 0.6);"
                                    "border-radius: " + QString::number(bubbleWidth/2) + "px;"
                                    "}"
                                    "QPushButton:hover:!pressed {"
                                    "background-color: rgba(0, 0, 0, 0.8);"
                                    "}");
    ui->btnFunction1->setGeometry(0, 0, bubbleWidth, bubbleHeight);
    ui->btnFunction1->setStyleSheet("QPushButton {"
                                    "background-color: rgba(0, 0, 0, 0.6);"
                                    "border-radius: " + QString::number(bubbleWidth/2) + "px;"
                                    "}"
                                    "QPushButton:hover:!pressed {"
                                    "background-color: rgba(0, 0, 0, 0.8);"
                                    "}");
    ui->btnFunction1->hide();
    ui->btnFunction2->setGeometry(0, bubbleHeight + bubbleGap, bubbleWidth, bubbleHeight);
    ui->btnFunction2->setStyleSheet("QPushButton {"
                                    "background-color: rgba(0, 0, 0, 0.6);"
                                    "border-radius: " + QString::number(bubbleWidth/2) + "px;"
                                    "}"
                                    "QPushButton:hover:!pressed {"
                                    "background-color: rgba(0, 0, 0, 0.8);"
                                    "}");
    ui->btnFunction2->hide();
    ui->btnFunction3->setGeometry(0, 2*bubbleHeight + 2*bubbleGap, bubbleWidth, bubbleHeight);
    ui->btnFunction3->setStyleSheet("QPushButton {"
                                    "background-color: rgba(0, 0, 0, 0.6);"
                                    "border-radius: " + QString::number(bubbleWidth/2) + "px;"
                                    "}"
                                    "QPushButton:hover:!pressed {"
                                    "background-color: rgba(0, 0, 0, 0.8);"
                                    "}");
    ui->btnFunction3->hide();
	qApp->installEventFilter(this);
}

void BubbleWindow::on_btnMain_clicked()
{
	if (isBubbleMinimized) {
		maximizeApp();
	} else {
		minimizeApp();
	}
}

bool BubbleWindow::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == this && (event->type() == QEvent::MouseButtonPress)) {
		qDebug() << "Button pressed";
	} else if (watched == this && (event->type() == QEvent::MouseButtonRelease)) {
		qDebug() << "Button released";
		minimizeApp();
    }
	return false;
}

void BubbleWindow::minimizeApp()
{
    isBubbleMinimized = true;
	minimizeAnimation->setStartValue(maximizedSize);
	minimizeAnimation->setEndValue(minimizedSize);
	minimizeAnimation->start();

    ui->btnFunction1->hide();
    ui->btnFunction2->hide();
    ui->btnFunction3->hide();
    ui->btnMain->setGeometry(0, 0, bubbleWidth, bubbleHeight);
    ui->btnMain->setIcon(QIcon(":/Icons/resources/icons/circle.png"));
}

void BubbleWindow::maximizeApp()
{
    isBubbleMinimized = false;
	maximizeAnimation->setStartValue(minimizedSize);
	maximizeAnimation->setEndValue(maximizedSize);
	maximizeAnimation->start();

    ui->btnFunction1->show();
    ui->btnFunction2->show();
    ui->btnFunction3->show();
    ui->btnMain->setGeometry(bubbleWidth + bubbleGap, bubbleHeight + bubbleGap, bubbleWidth, bubbleHeight);
    ui->btnMain->setIcon(QIcon(":/Icons/resources/icons/close.png"));
}

void BubbleWindow::setIcons()
{
    ui->btnMain->setIconSize(iconSize);
    ui->btnFunction1->setIconSize(iconSize);
    ui->btnFunction2->setIconSize(iconSize);
    ui->btnFunction3->setIconSize(iconSize);

    ui->btnMain->setIcon(QIcon(":/Icons/resources/icons/circle.png"));
    ui->btnFunction1->setIcon(QIcon(":/Icons/resources/icons/microphone-on.png"));
    ui->btnFunction2->setIcon(QIcon(":/Icons/resources/icons/screenshot.png"));
    ui->btnFunction3->setIcon(QIcon(":/Icons/resources/icons/camera-on.png"));
}

void BubbleWindow::addAnimations()
{
	maximizeAnimation = new QPropertyAnimation((QWidget *)this, "geometry");
	maximizeAnimation->setDuration(200);

	minimizeAnimation = new QPropertyAnimation((QWidget *)this, "geometry");
	minimizeAnimation->setDuration(200);
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
        return;
	}
    isMicVolumeZero = !isMicVolumeZero;
    if (isMicVolumeZero) {
        ui->btnFunction1->setIcon(QIcon(":/Icons/resources/icons/microphone-off.png"));
    } else {
        ui->btnFunction1->setIcon(QIcon(":/Icons/resources/icons/microphone-on.png"));
    }
#elif defined(Q_OS_WIN32)
	// Code for muting microphone on windows
#endif
}


/*!
 * \brief This function takes screenshot of current Screen
 */
void BubbleWindow::on_btnFunction2_clicked()
{
    this->hide();
    QTimer::singleShot(1000, this, &BubbleWindow::takeScreenshot);
}


/*!
 * \brief This function enables/disables the camera
 */
void BubbleWindow::on_btnFunction3_clicked()
{
#ifdef Q_OS_LINUX
    // Code for muting microphone on linux
    if (pathToCameraBusDevice != "") {
        if (!toggleCameraConfigValue()) {
            qDebug() << "Not able to toggle camera config value";
            return;
        }
    }

    QProcess process;
    process.start("udevadm", QStringList() << "info" << "--query=all" << "/dev/video1");
    if (!process.waitForFinished()) {
        qDebug() << "Process1 ended abruptly";
    }
    QString res = process.readAllStandardOutput();
    QString vendorID, productID;

    QRegularExpression rgx;
    rgx.setPattern("E: ID_VENDOR_ID=.*");
    QRegularExpressionMatch match = rgx.match(res);
    if (match.hasMatch()) {
        QString matchedString = match.captured(0);
        vendorID = matchedString.split("=").at(1);
    }
    rgx.setPattern("E: ID_MODEL_ID=.*");
    match = rgx.match(res);
    if (match.hasMatch()) {
        QString matchedString = match.captured(0);
        productID = matchedString.split("=").at(1);
    }
    qDebug() << vendorID << ":" << productID;

    vendorID = vendorID.trimmed();
    productID = productID.trimmed();

    QDirIterator dir_itr("/sys/bus/usb/devices", QDir::Dirs | QDir::NoDotAndDotDot);
    while (dir_itr.hasNext()) {
        QString dir = dir_itr.next();

        // Check Vendor ID and Product ID in files idVendor and idProduct respectively
        QFile vendorIdFile(dir + "/idVendor");
        if (!vendorIdFile.exists()) {
            continue;
        }
        if (!vendorIdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Cannot open idVendor file in read mode";
            continue;
        }
        QString deviceVendorID = vendorIdFile.readAll().trimmed();
        vendorIdFile.close();
        if (deviceVendorID != vendorID) {
            continue;
        }

        QFile productIdFile(dir + "/idProduct");
        if (!productIdFile.exists()) {
            continue;
        }
        if (!productIdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Cannot open idProduct file in write mode";
            continue;
        }
        QString deviceProductID = productIdFile.readAll().trimmed();
        productIdFile.close();
        if (deviceProductID != productID) {
            continue;
        }
        pathToCameraBusDevice = dir;
        if (toggleCameraConfigValue()) {
            break;
        }
    }



#elif defined(Q_OS_WIN32)
    // Code for muting microphone on windows
#endif
}

void BubbleWindow::takeScreenshot()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen == nullptr) {
        qDebug() << "Can't take screenshot";
        return;
    }
    const QWindow *window = windowHandle();
    screen = window->screen();
    QPixmap screenPixmap = screen->grabWindow(0);
    screenPixmap.save(QDir::homePath() + "/Pictures/screenshot.png");
    QApplication::beep();
    this->show();
}

#ifdef Q_OS_LINUX
bool BubbleWindow::toggleCameraConfigValue()
{
    QFile file(pathToCameraBusDevice + "/bConfigurationValue");
    if (!file.exists()) {
        return false;
    }

    QString newConfigurationValue;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open bConfigurationValue in read mode";
        return false;
    }
    QString fileText;
    QTextStream in(&file);
    fileText = in.readAll();
    file.close();

    if (fileText == "" || fileText[0] == "0") {
        newConfigurationValue = "1";
    } else {
        newConfigurationValue = "0";
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file bConfigurationValue in write mode";
        return false;
    }
    QTextStream out(&file);
    out << newConfigurationValue;
    file.close();
    isCameraDeviceBlocked = !isCameraDeviceBlocked;
    if (isCameraDeviceBlocked) {
        ui->btnFunction1->setIcon(QIcon(":/Icons/resources/icons/camera-off.png"));
    } else {
        ui->btnFunction1->setIcon(QIcon(":/Icons/resources/icons/camera-on.png"));
    }
    return true;
}
#endif
