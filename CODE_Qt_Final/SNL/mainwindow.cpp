#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lexeranalyse.h"
#include "recursiveanalyse.h"
#include "semanticanalyse.h"
#include "ll1analyse.h"
#include "predict.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>  // 选择文件对话??

MainWindow::MainWindow(QWidget *parent) :        // 触发列表
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/logo/logo.png"));

    // 进去textEdit就显示欢迎文??
    QFile file1("D:/CODE_Qt/txt/UI/backwords.txt");
    if(!file1.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file1.readAll();
    ui->textEdit->setText(a);  //将读取到欢迎信息放入textEdit??
    file1.close();

//    QString strTemp= ui->textEdit->document()->toPlainText();////文本框中的内??
//    QStringList strList= strTemp.split( "/n" );////利用内容中的回车换行符截取，来获知共有几??
//    cout<<strList;
    // 右部textEdit_2显示使用教程
    QFile file2("D:/CODE_Qt/txt/UI/backwords_2.txt");
    if(!file2.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray b = file2.readAll();
    ui->textEdit_2->setText(b);  //将读取到使用教程放入textEdit_2??
    file2.close();

    // 还要把那几个输出结果的文件清??否则就会输出上一次运行的结果
    // 其中 tokens.txt ??debug_info.txt 每次运行词法分析就会覆盖, 主要是清??语法树和语义表文??
    QFile file3("D:/CODE_Qt/txt/ll1SyntaxTree.txt");
    file3.open(QFile::WriteOnly|QFile::Truncate);
    file3.close();

    QFile file4("D:/CODE_Qt/txt/recursiveSyntaxTree.txt");
    file4.open(QFile::WriteOnly|QFile::Truncate);
    file4.close();

    QFile file5("D:/CODE_Qt/txt/semanticSymbleTable.txt");
    file5.open(QFile::WriteOnly|QFile::Truncate);
    file5.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//------------------- 动作触发执行的函?? 槽函数们 -----------------------
void MainWindow::on_action_ctrl_Q_triggered()  // 退??
{
    // 怎么写啊, 猜一??  这个应该是自动connect()??信号和槽 ?? 所以只需要写槽函数就?? yes!!! 猜对??
    this->close();  // 关闭窗口
}

void MainWindow::on_actionopen_triggered()  // 新建文件
{
   // 我应?? 新建文件的话, 我需要调出来一??敲代码的窗口,应该要自定义  把敲代码窗口设为 enable
   // 我需要在这个函数里把 那个 textEdit 对象设为 enable
    ui->textEdit->setEnabled(true);
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);  // 加横向滑动条

    // 还要把那个欢迎文档清??
    ui->textEdit->clear();
}

void MainWindow::on_actionnew_triggered()  // 打开文件
{
    //选择文件对话??
    QFileDialog* f = new QFileDialog(this);
    f->setWindowTitle("选择数据文件*.txt");
    f->setNameFilter("*.txt");
    f->setViewMode(QFileDialog::Detail);

    QString filePath;
    if(f->exec() == QDialog::Accepted)
        filePath = f->selectedFiles()[0];

    //创建 QFile 对象，同时指定要操作的文??
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit->setText(a);  //将读取到的token数据放入textEdit_2??
    file.close();

    ui->textEdit->setEnabled(true);
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);  // 加横向滑动条
}


void MainWindow::on_actionsave_triggered()  // 保存文件
{
    // ??textEdit 里的内容保存到一个文件里
    //将文本框数据取出并按行排??
    QFile file("D:/CODE_Qt/txt/sourceFile.txt");  //文件路径名和命名
    if (!file.open(QFile::WriteOnly | QFile::Text))		//检测文件是否打开, 只写打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QTextStream out(&file);		//分行写入文件
    out << ui->textEdit->toPlainText();
    file.close();  // 输入??textEdit 再关闭文??

    ui->pushButton_2->setEnabled(true);  // 将词法分析的按钮设为 enable
}

void MainWindow::on_pushButton_2_clicked()  // 词法分析按钮
{
    // 应该执行自己的词法分析cpp代码, 再把 token 输出??textEdit_2 中展??
    getTokenList();

    //创建 QFile 对象，同时指定要操作的文??
    QFile file("D:/CODE_Qt/txt/tokens.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //将读取到的token数据放入textEdit_2??
    file.close();

    // textEdit_2 enable
    ui->textEdit_2->setEnabled(true);

    // 语法分析的按??enable
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
}

void MainWindow::on_pushButton_4_clicked()  // 递归下降语法分析 按钮
{
    // 执行递归下降语法分析程序, 语法树文件输出到 textEdit_2 ??
    getParseTree();

    //创建 QFile 对象，同时指定要操作的文??
    QFile file("D:/CODE_Qt/txt/recursiveSyntaxTree.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //将读取到的parseTree数据放入textEdit_2??
    file.close();

    // 语义分析的按??enable
    ui->pushButton->setEnabled(true);
}


void MainWindow::on_pushButton_clicked() // 语义分析 按钮
{
    // 执行语义分析部分代码, 读取 符号表文????textEdit_2 ??
    semanticAnalyse();

    //创建 QFile 对象，同时指定要操作的文??
    QFile file("D:/CODE_Qt/txt/semanticSymbleTable.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //将读取到的parseTree数据放入textEdit_2??
    file.close();
}

void MainWindow::on_pushButton_5_clicked()  // debug info 的按??
{
    // 需要把控制台输出的信息输出??textEdit_2 ??, 把控制台输出的信息转到文件中??
    //创建 QFile 对象，同时指定要操作的文??
    QFile file("D:/CODE_Qt/txt/debug_info.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //将读取到的parseTree数据放入textEdit_2??
    file.close();
}

void MainWindow::on_pushButton_3_clicked()  // LL(1) 语法分析的按??
{
    // 输出语法树到 textEdit_2
    getLL1Tree();
    //创建 QFile 对象，同时指定要操作的文??
    QFile file("D:/CODE_Qt/txt/ll1SyntaxTree.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //将读取到的parseTree数据放入textEdit_2??
    file.close();

    // 语义分析的按??enable
    ui->pushButton->setEnabled(true);
}



// --------------------- predict ??-----------------------

void MainWindow::on_action_triggered()  // 文法相关
{
        QFile file("D:/CODE_Qt/txt/UI/grammarfinal.txt");
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
        {
            QMessageBox::information(this, "Error Message", "Please Select a Text File!");
            return;
        }
        QByteArray a = file.readAll();
        ui->textEdit_2->setText(a);  //将读取到的grammar数据放入textEdit_2??
        file.close();
        // textEdit_2 enable
        ui->textEdit_2->setEnabled(true);
}

void MainWindow::on_actionPredict_triggered()
{
        getAllPredict();
        QFile file("D:/CODE_Qt/txt/UI/predicts.txt");
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // 只读打开
        {
            QMessageBox::information(this, "Error Message", "Please Select a Text File!");
            return;
        }
        QByteArray a = file.readAll();
        ui->textEdit_2->setText(a);  //将读取到的predict数据放入textEdit_2??
        file.close();
        // textEdit_2 enable
        ui->textEdit_2->setEnabled(true);
}
