#ifndef BUBBLEWINDOW_H
#define BUBBLEWINDOW_H

#include <QMainWindow>
#include <QPair>
#include <QRegularExpressionMatch>

class QPropertyAnimation;
class QAudioRecorder;
class SpeechToText;
class QThread;

QT_BEGIN_NAMESPACE
namespace Ui { class BubbleWindow; }
QT_END_NAMESPACE


class BubbleWindow : public QMainWindow
{
	Q_OBJECT

public:
	BubbleWindow(QWidget *parent = nullptr);
	QSize sizeHint() const override;
	~BubbleWindow();

private slots:
    void initApp();
	void minimizeApp();
	void maximizeApp();
	void setIcons();
    void addAnimations();
    void takeScreenshot();
    void askToQuit();

	void on_btnMain_clicked();
	void on_btnFunction1_clicked();
	void on_btnFunction2_clicked();
    void on_btnFunction3_clicked();
    void on_btnFunction4_clicked();
    void on_btnFunction5_clicked();

    void startSpeechToText();
    void finishedSpeechToText();
    void stopRecording();

signals:
    void stopTranscribingAudio();

private:
    bool isInAdminMode = false;
	Ui::BubbleWindow *ui;
	bool isBubbleMinimized;
	QPropertyAnimation *maximizeAnimation = nullptr;
	QPropertyAnimation *minimizeAnimation = nullptr;
	int bubbleWidth;
	int bubbleHeight;
	QRect MonitorInfo;
    QRect maximizedSize, minimizedSize; // keeps on changing according to the values
    QString pathToCameraBusDevice = "";
    QSize iconSize;
    int bubbleGap;
    bool isCameraDeviceBlocked = false;
    bool isMicVolumeZero = false;
    bool isSpeakerVolumeZero = false;
    int curSpeakerValue = 100;
    QRegularExpressionMatch match;
    QTimer *timer;
    QAudioRecorder *audioRecorder;
    QMetaObject::Connection recordingConnection;
    SpeechToText *speechToTextWorker;
    QThread *threadSpeechToText;
    bool isInRecording;

    bool eventFilter(QObject *watched, QEvent *event);
#ifdef Q_OS_LINUX
    bool toggleCameraConfigValue();
#endif
};
#endif // BUBBLEWINDOW_H
