#include "mainwindow.h"
#include <QApplication>

// Main程序入口  argc命令行变量的数量, argv命令行变量的数组
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);  // a 是应用程序对象,在Qt中有且仅有一个

    MainWindow w;  // 窗口对象, 默认不会显示, 要调用 show 方法才能显示
    w.show();

    return a.exec();  // 进入消息循环机制,出来的窗口一直等着你点击按钮做出相应动作, 代码阻塞到这里
}
