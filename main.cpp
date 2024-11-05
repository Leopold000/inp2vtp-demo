#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
    // 设置控制台编码为 UTF-8
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
