#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT     // 宏, 允许类中使用信号和槽机制

public:
    explicit MainWindow(QWidget *parent = 0);  // 构造函数
    ~MainWindow();

private slots:
    void on_actionopen_triggered();  // 这个是动作函数, 后来加上的

    void on_action_ctrl_Q_triggered();

    void on_actionsave_triggered();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_actionnew_triggered();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_action_triggered();

    void on_actionPredict_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
