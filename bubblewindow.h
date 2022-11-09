#ifndef BUBBLEWINDOW_H
#define BUBBLEWINDOW_H

#include <QMainWindow>

class QPropertyAnimation;
class QTimer;

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
	void setMasks();
	void minimizeApp();
	void maximizeApp();
	void setIcons();
	void addAnimations();
	void windowMoveTimerSlot();
	void on_btnMain_clicked();
	void on_btnFunction1_clicked();

	void on_btnFunction2_clicked();

private:
	Ui::BubbleWindow *ui;
	bool isBubbleMinimized;
	QPropertyAnimation *maximizeAnimation = nullptr;
	QPropertyAnimation *minimizeAnimation = nullptr;
	int bubbleWidth;
	int bubbleHeight;
	QRect MonitorInfo;
	QRect maximizedSize, minimizedSize; // keeps on changing according to the values
	QTimer *timerForWindowPos; // Timer tracking window position

	bool eventFilter(QObject *watched, QEvent *event);
	QPixmap makeImageRounded(const QPixmap &original);

protected:
	void resizeEvent(QResizeEvent *event) override;
};
#endif // BUBBLEWINDOW_H
