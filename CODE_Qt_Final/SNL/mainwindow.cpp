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
#include <QFileDialog>  // ѡ���ļ��Ի�??

MainWindow::MainWindow(QWidget *parent) :        // �����б�
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/logo/logo.png"));

    // ��ȥtextEdit����ʾ��ӭ��??
    QFile file1("D:/CODE_Qt/txt/UI/backwords.txt");
    if(!file1.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file1.readAll();
    ui->textEdit->setText(a);  //����ȡ����ӭ��Ϣ����textEdit??
    file1.close();

//    QString strTemp= ui->textEdit->document()->toPlainText();////�ı����е���??
//    QStringList strList= strTemp.split( "/n" );////���������еĻس����з���ȡ������֪���м�??
//    cout<<strList;
    // �Ҳ�textEdit_2��ʾʹ�ý̳�
    QFile file2("D:/CODE_Qt/txt/UI/backwords_2.txt");
    if(!file2.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray b = file2.readAll();
    ui->textEdit_2->setText(b);  //����ȡ��ʹ�ý̷̳���textEdit_2??
    file2.close();

    // ��Ҫ���Ǽ������������ļ���??����ͻ������һ�����еĽ��
    // ���� tokens.txt ??debug_info.txt ÿ�����дʷ������ͻḲ��, ��Ҫ����??�﷨�����������??
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


//------------------- ��������ִ�еĺ�?? �ۺ����� -----------------------
void MainWindow::on_action_ctrl_Q_triggered()  // ��??
{
    // ��ôд��, ��һ??  ���Ӧ�����Զ�connect()??�źźͲ� ?? ����ֻ��Ҫд�ۺ�����?? yes!!! �¶�??
    this->close();  // �رմ���
}

void MainWindow::on_actionopen_triggered()  // �½��ļ�
{
   // ��Ӧ?? �½��ļ��Ļ�, ����Ҫ������һ??�ô���Ĵ���,Ӧ��Ҫ�Զ���  ���ô��봰����Ϊ enable
   // ����Ҫ������������ �Ǹ� textEdit ������Ϊ enable
    ui->textEdit->setEnabled(true);
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);  // �Ӻ��򻬶���

    // ��Ҫ���Ǹ���ӭ�ĵ���??
    ui->textEdit->clear();
}

void MainWindow::on_actionnew_triggered()  // ���ļ�
{
    //ѡ���ļ��Ի�??
    QFileDialog* f = new QFileDialog(this);
    f->setWindowTitle("ѡ�������ļ�*.txt");
    f->setNameFilter("*.txt");
    f->setViewMode(QFileDialog::Detail);

    QString filePath;
    if(f->exec() == QDialog::Accepted)
        filePath = f->selectedFiles()[0];

    //���� QFile ����ͬʱָ��Ҫ��������??
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit->setText(a);  //����ȡ����token���ݷ���textEdit_2??
    file.close();

    ui->textEdit->setEnabled(true);
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);  // �Ӻ��򻬶���
}


void MainWindow::on_actionsave_triggered()  // �����ļ�
{
    // ??textEdit ������ݱ��浽һ���ļ���
    //���ı�������ȡ����������??
    QFile file("D:/CODE_Qt/txt/sourceFile.txt");  //�ļ�·����������
    if (!file.open(QFile::WriteOnly | QFile::Text))		//����ļ��Ƿ��, ֻд��
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QTextStream out(&file);		//����д���ļ�
    out << ui->textEdit->toPlainText();
    file.close();  // ����??textEdit �ٹر���??

    ui->pushButton_2->setEnabled(true);  // ���ʷ������İ�ť��Ϊ enable
}

void MainWindow::on_pushButton_2_clicked()  // �ʷ�������ť
{
    // Ӧ��ִ���Լ��Ĵʷ�����cpp����, �ٰ� token ���??textEdit_2 ��չ??
    getTokenList();

    //���� QFile ����ͬʱָ��Ҫ��������??
    QFile file("D:/CODE_Qt/txt/tokens.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //����ȡ����token���ݷ���textEdit_2??
    file.close();

    // textEdit_2 enable
    ui->textEdit_2->setEnabled(true);

    // �﷨�����İ�??enable
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
}

void MainWindow::on_pushButton_4_clicked()  // �ݹ��½��﷨���� ��ť
{
    // ִ�еݹ��½��﷨��������, �﷨���ļ������ textEdit_2 ??
    getParseTree();

    //���� QFile ����ͬʱָ��Ҫ��������??
    QFile file("D:/CODE_Qt/txt/recursiveSyntaxTree.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //����ȡ����parseTree���ݷ���textEdit_2??
    file.close();

    // ��������İ�??enable
    ui->pushButton->setEnabled(true);
}


void MainWindow::on_pushButton_clicked() // ������� ��ť
{
    // ִ������������ִ���, ��ȡ ���ű���????textEdit_2 ??
    semanticAnalyse();

    //���� QFile ����ͬʱָ��Ҫ��������??
    QFile file("D:/CODE_Qt/txt/semanticSymbleTable.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //����ȡ����parseTree���ݷ���textEdit_2??
    file.close();
}

void MainWindow::on_pushButton_5_clicked()  // debug info �İ�??
{
    // ��Ҫ�ѿ���̨�������Ϣ���??textEdit_2 ??, �ѿ���̨�������Ϣת���ļ���??
    //���� QFile ����ͬʱָ��Ҫ��������??
    QFile file("D:/CODE_Qt/txt/debug_info.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //����ȡ����parseTree���ݷ���textEdit_2??
    file.close();
}

void MainWindow::on_pushButton_3_clicked()  // LL(1) �﷨�����İ�??
{
    // ����﷨���� textEdit_2
    getLL1Tree();
    //���� QFile ����ͬʱָ��Ҫ��������??
    QFile file("D:/CODE_Qt/txt/ll1SyntaxTree.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QByteArray a = file.readAll();
    ui->textEdit_2->setText(a);  //����ȡ����parseTree���ݷ���textEdit_2??
    file.close();

    // ��������İ�??enable
    ui->pushButton->setEnabled(true);
}



// --------------------- predict ??-----------------------

void MainWindow::on_action_triggered()  // �ķ����
{
        QFile file("D:/CODE_Qt/txt/UI/grammarfinal.txt");
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
        {
            QMessageBox::information(this, "Error Message", "Please Select a Text File!");
            return;
        }
        QByteArray a = file.readAll();
        ui->textEdit_2->setText(a);  //����ȡ����grammar���ݷ���textEdit_2??
        file.close();
        // textEdit_2 enable
        ui->textEdit_2->setEnabled(true);
}

void MainWindow::on_actionPredict_triggered()
{
        getAllPredict();
        QFile file("D:/CODE_Qt/txt/UI/predicts.txt");
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text))      // ֻ����
        {
            QMessageBox::information(this, "Error Message", "Please Select a Text File!");
            return;
        }
        QByteArray a = file.readAll();
        ui->textEdit_2->setText(a);  //����ȡ����predict���ݷ���textEdit_2??
        file.close();
        // textEdit_2 enable
        ui->textEdit_2->setEnabled(true);
}
