#include "bubblewindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BubbleWindow w;
	w.show();
	return a.exec();
}
