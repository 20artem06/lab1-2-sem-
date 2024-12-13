
#include "mainwindow.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QDir>
//#include <QtCore/QTextStream>




MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , btnCreateShared(new QPushButton("������� shared_ptr", this))
    , btnCreateUnique(new QPushButton("������� unique_ptr", this))
    , btnCreateWeak(new QPushButton("������� weak_ptr", this))
    , btnRunFunctionalTests(new QPushButton("��������� �������������� �����", this))
    , btnRunStressTests(new QPushButton("��������� ����������� �����", this))
    , textEditLog(new QTextEdit(this))
    , functionalTestProcess(new QProcess(this))
    , stressTestProcess(new QProcess(this))
{
    // ��������� UI
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // ������ ������ ������
    QHBoxLayout* layout1 = new QHBoxLayout();
    layout1->addWidget(btnCreateShared);
    layout1->addWidget(btnCreateUnique);
    layout1->addWidget(btnCreateWeak);
    mainLayout->addLayout(layout1);

    // ������ ������ ������
    QHBoxLayout* layout2 = new QHBoxLayout();
    layout2->addWidget(btnRunFunctionalTests);
    layout2->addWidget(btnRunStressTests);
    mainLayout->addLayout(layout2);

    // ��� ���������� ���������
    textEditLog->setReadOnly(true);
    mainLayout->addWidget(textEditLog);

    // ����������� �������� � ������
    connect(btnCreateShared, &QPushButton::clicked, this, &MainWindow::on_btnCreateShared_clicked);
    connect(btnCreateUnique, &QPushButton::clicked, this, &MainWindow::on_btnCreateUnique_clicked);
    connect(btnCreateWeak, &QPushButton::clicked, this, &MainWindow::on_btnCreateWeak_clicked);
    connect(btnRunFunctionalTests, &QPushButton::clicked, this, &MainWindow::on_btnRunFunctionalTests_clicked);
    connect(btnRunStressTests, &QPushButton::clicked, this, &MainWindow::on_btnRunStressTests_clicked);

    // ����������� �������� ���������� ��������� � ������
    connect(functionalTestProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainWindow::functionalTestsFinished);
    connect(stressTestProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainWindow::stressTestsFinished);

    // ����������� �������� ���������� ������
    connect(functionalTestProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        QString output = functionalTestProcess->readAllStandardOutput();
        logMessage(output);
        });
    connect(functionalTestProcess, &QProcess::readyReadStandardError, this, [this]() {
        QString output = functionalTestProcess->readAllStandardError();
        logMessage(output);
        });

    connect(stressTestProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        QString output = stressTestProcess->readAllStandardOutput();
        logMessage(output);
        });
    connect(stressTestProcess, &QProcess::readyReadStandardError, this, [this]() {
        QString output = stressTestProcess->readAllStandardError();
        logMessage(output);
        });
}

MainWindow::~MainWindow()
{

}

void MainWindow::logMessage(const QString& message)
{
    textEditLog->append(message);
}

void MainWindow::on_btnCreateShared_clicked()
{
    sp = shared_ptr<TestObject>(new TestObject(100));
    QString msg = QString("������ shared_ptr � use_count = %1").arg(sp.use_count());
    logMessage(msg);
}

void MainWindow::on_btnCreateUnique_clicked()
{
    up = unique_ptr<TestObject>(new TestObject(200));
    if (up) {
        QString msg = QString("������ unique_ptr � �������� ID = %1").arg(up->id);
        logMessage(msg);
    }
}

void MainWindow::on_btnCreateWeak_clicked()
{
    wp = weak_ptr<TestObject>(sp);
    if (!wp.expired()) {
        shared_ptr<TestObject> sp_locked = wp.lock();
        QString msg = QString("������ weak_ptr. use_count = %1").arg(sp_locked.use_count());
        logMessage(msg);
    }
    else {
        logMessage("weak_ptr �����. ������ ���������.");
    }
}

void MainWindow::on_btnRunFunctionalTests_clicked()
{
    if (functionalTestProcess->state() != QProcess::NotRunning) {
        QMessageBox::information(this, "����������", "�������������� ����� ��� �����������.");
        return;
    }

    QString testPath = QDir::currentPath() + "/functional_tests";

#ifdef Q_OS_WIN
    testPath += ".exe";
#endif

    if (!QFile::exists(testPath)) {
        QMessageBox::warning(this, "������", "�� ������ ����������� ���� functional_tests.");
        return;
    }

    logMessage("������ �������������� ������...");
    functionalTestProcess->start(testPath);
}

void MainWindow::on_btnRunStressTests_clicked()
{
    if (stressTestProcess->state() != QProcess::NotRunning) {
        QMessageBox::information(this, "����������", "����������� ����� ��� �����������.");
        return;
    }

    QString testPath = QDir::currentPath() + "/stress_tests";

#ifdef Q_OS_WIN
    testPath += ".exe";
#endif

    if (!QFile::exists(testPath)) {
        QMessageBox::warning(this, "������", "�� ������ ����������� ���� stress_tests.");
        return;
    }

    logMessage("������ ����������� ������...");
    stressTestProcess->start(testPath);
}

void MainWindow::functionalTestsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        logMessage("�������������� ����� ��������� �������.");
    }
    else {
        logMessage("�������������� ����� ����������� � �������.");
    }
}

void MainWindow::stressTestsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        logMessage("����������� ����� ��������� �������.");
    }
    else {
        logMessage("����������� ����� ����������� � �������.");
    }
}