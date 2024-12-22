
#include "mainwindow.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QDir>
//#include <QtCore/QTextStream>




MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , btnCreateShared(new QPushButton("Создать shared_ptr", this))
    , btnCreateUnique(new QPushButton("Создать unique_ptr", this))
    , btnCreateWeak(new QPushButton("Создать weak_ptr", this))
    , btnRunFunctionalTests(new QPushButton("Запустить Функциональные Тесты", this))
    , btnRunStressTests(new QPushButton("Запустить Нагрузочные Тесты", this))
    , textEditLog(new QTextEdit(this))
    , functionalTestProcess(new QProcess(this))
    , stressTestProcess(new QProcess(this))
{
    // Настройка UI
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Первая строка кнопок
    QHBoxLayout* layout1 = new QHBoxLayout();
    layout1->addWidget(btnCreateShared);
    layout1->addWidget(btnCreateUnique);
    layout1->addWidget(btnCreateWeak);

    btnCreateShared->setStyleSheet("background-color: #FFCCCC; color: black;"); // Цвет кнопки
    btnCreateUnique->setStyleSheet("background-color: #CCFFCC; color: black;"); // Цвет кнопки
    btnCreateWeak->setStyleSheet("background-color: #CCCCFF; color: black;"); // Цвет кнопки

    btnCreateShared->setFixedSize(150, 50);
    btnCreateUnique->setFixedSize(150, 50);
    btnCreateWeak->setFixedSize(150, 50);

    mainLayout->addLayout(layout1);

    // Вторая строка кнопок
    QHBoxLayout* layout2 = new QHBoxLayout();
    layout2->addWidget(btnRunFunctionalTests);
    layout2->addWidget(btnRunStressTests);
    mainLayout->addLayout(layout2);

    // Лог текстового редактора
    textEditLog->setReadOnly(true);
    mainLayout->addWidget(textEditLog);

    // Подключение сигналов к слотам
    connect(btnCreateShared, &QPushButton::clicked, this, &MainWindow::on_btnCreateShared_clicked);
    connect(btnCreateUnique, &QPushButton::clicked, this, &MainWindow::on_btnCreateUnique_clicked);
    connect(btnCreateWeak, &QPushButton::clicked, this, &MainWindow::on_btnCreateWeak_clicked);
    connect(btnRunFunctionalTests, &QPushButton::clicked, this, &MainWindow::on_btnRunFunctionalTests_clicked);
    connect(btnRunStressTests, &QPushButton::clicked, this, &MainWindow::on_btnRunStressTests_clicked);

    // Подключение сигналов завершения процессов к слотам
    connect(functionalTestProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::functionalTestsFinished);
    connect(stressTestProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::stressTestsFinished);

    // Подключение сигналов готовности данных
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
    QString msg = QString("Создан shared_ptr с use_count = %1").arg(sp.use_count());
    logMessage(msg);
}

void MainWindow::on_btnCreateUnique_clicked()
{
    up = unique_ptr<TestObject>(new TestObject(200));
    if (up) {
        QString msg = QString("Создан unique_ptr с объектом ID = %1").arg(up->id);
        logMessage(msg);
    }
}

void MainWindow::on_btnCreateWeak_clicked()
{
    wp = weak_ptr<TestObject>(sp);
    if (!wp.expired()) {
        shared_ptr<TestObject> sp_locked = wp.lock();
        QString msg = QString("Создан weak_ptr. use_count = %1").arg(sp_locked.use_count());
        logMessage(msg);
    }
    else {
        logMessage("weak_ptr истек. Объект уничтожен.");
    }
}

void MainWindow::on_btnRunFunctionalTests_clicked()
{
    if (functionalTestProcess->state() != QProcess::NotRunning) {
        QMessageBox::information(this, "Информация", "Функциональные тесты уже запускаются.");
        return;
    }

    QString testPath = QDir::currentPath() + "/functional_tests";

#ifdef Q_OS_WIN
    testPath += ".exe";
#endif

    if (!QFile::exists(testPath)) {
        QMessageBox::warning(this, "Ошибка", "Не найден исполняемый файл functional_tests.");
        return;
    }

    logMessage("Запуск функциональных тестов...");
    functionalTestProcess->start(testPath);
}

void MainWindow::on_btnRunStressTests_clicked()
{
    if (stressTestProcess->state() != QProcess::NotRunning) {
        QMessageBox::information(this, "Информация", "Нагрузочные тесты уже запускаются.");
        return;
    }

    QString testPath = QDir::currentPath() + "/stress_tests";

#ifdef Q_OS_WIN
    testPath += ".exe";
#endif

    if (!QFile::exists(testPath)) {
        QMessageBox::warning(this, "Ошибка", "Не найден исполняемый файл stress_tests.");
        return;
    }

    logMessage("Запуск нагрузочных тестов...");
    stressTestProcess->start(testPath);
}

void MainWindow::functionalTestsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        logMessage("Функциональные тесты завершены успешно.");
    }
    else {
        logMessage("Функциональные тесты завершились с ошибкой.");
    }
}

void MainWindow::stressTestsFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        logMessage("Нагрузочные тесты завершены успешно.");
    }
    else {
        logMessage("Нагрузочные тесты завершились с ошибкой.");
    }
}
