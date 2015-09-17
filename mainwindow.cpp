#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <QProcess>
#include <QDesktopServices>
#include <QTimer>

QTextEdit * MainWindow::s_textEdit = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    s_textEdit = ui->debugLogTextEdit;

    ui->mainToolBar->hide();
    ui->menuBar->hide();
    ui->desktopButton->hide();// ShellExecute 'adbcontrol.jar' failed (error 2).
    ui->helpTextBrowser->setOpenExternalLinks(true);

    readSettings();

    m_process = 0;

    if(qApp->applicationDirPath().contains(" "))
        qDebug() << "The path to this program contains a space.  Please reinstall to a different folder without a space.";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSettings()
{
//    qDebug() << Q_FUNC_INFO;

    QSettings s;
    ui->embeddedButton->setChecked(s.value("embedded_process", true).toBool());
    ui->detachedButton->setChecked(s.value("detached_process", false).toBool());
    ui->desktopButton->setChecked(s.value("desktop_services", false).toBool());


    ui->tabWidget->setCurrentIndex(s.value("last_tab", 1).toInt());

    QFile configFile("config.properties");
    if(!configFile.open(QFile::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&configFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList strs = line.split(" = ");
        if(strs.size() > 1)
        {
            QString key = strs.at(0);
            QString value = strs.at(1);
            if(key == "adbCommand")
                ui->adbPathLineEdit->setText(value);
            else if(key == "screenshotDelay")
                ui->delaySpinBox->setValue(value.toInt());
            else if(key == "localImageFilePath")
                ui->hostImagePathLineEdit->setText(value);
            else if(key == "phoneImageFilePath")
                ui->deviceImagePathLineEdit->setText(value);
            else
                qDebug() << "unknown key" << key << " = " << value;
        }
    }
}

void MainWindow::writeSettings()
{
//    qDebug() << Q_FUNC_INFO;
    QSettings s;
    s.setValue("detached_process", ui->detachedButton->isChecked());
    s.setValue("embedded_process", ui->embeddedButton->isChecked());
    s.setValue("desktop_services", ui->desktopButton->isChecked());

    s.setValue("last_tab", ui->tabWidget->currentIndex());
}

void MainWindow::on_getImagePath_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(0,"Image path",QString(),".png");
    if(fileName.isEmpty())
        return;
    if(!fileName.endsWith(".png"))
        fileName.append(".png");

    ui->hostImagePathLineEdit->setText(fileName);

    generateConfigFile();
}

void MainWindow::on_getAdbPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(0,"Path to adb.exe",QString(),"adb.exe");
    if(fileName.isEmpty())
        return;

    ui->adbPathLineEdit->setText(fileName);
    generateConfigFile();
}

void MainWindow::on_delaySpinBox_valueChanged(int delay_ms)
{
    Q_UNUSED(delay_ms)
    generateConfigFile();
}

void MainWindow::on_deviceImagePathLineEdit_textChanged(const QString &imagePathDevice)
{
    Q_UNUSED(imagePathDevice)
    generateConfigFile();
}

void MainWindow::generateConfigFile()
{
    QString out;
    out = "adbCommand = " + ui->adbPathLineEdit->text() + "\n"
        + "screenshotDelay = " + QString::number(ui->delaySpinBox->value()) + "\n"
        + "localImageFilePath = " + ui->hostImagePathLineEdit->text() + "\n"
        + "phoneImageFilePath = " + ui->deviceImagePathLineEdit->text() + "\n";
    ui->configTxtEdit->setPlainText(out);

}

void MainWindow::on_launchAdbButton_clicked()
{
    if(ui->detachedButton->isChecked())
    {

        bool started = QProcess::startDetached("java -jar adbcontrol.jar");
        if(started)
            this->statusBar()->showMessage("Launched!");
        else
            this->statusBar()->showMessage("Error with java call!");
    }
    else if(ui->desktopButton->isChecked())
    {
        QDesktopServices::openUrl(qApp->applicationDirPath() + "/adbcontrol.jar");
    }
    else if(ui->embeddedButton)
    {
        if(m_process)
        {
            m_process->close();
            m_process->kill();
            delete m_process;
            m_process = 0;
        }
        m_process = new QProcess(this);
        QObject::connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(on_standardError()));
        QObject::connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(on_standardOutput()));
        m_process->start("java -jar adbcontrol.jar");
    }
}

void MainWindow::on_saveButton_clicked()
{
    QFile configFile("config.properties");
    if(!configFile.open(QFile::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&configFile);
    out << ui->configTxtEdit->toPlainText();
    configFile.close();
}

void MainWindow::on_standardOutput()
{
    QString output = m_process->readAllStandardOutput();
    output.replace("\r","");

    s_textEdit->moveCursor (QTextCursor::End);
    s_textEdit->insertPlainText (output);
    s_textEdit->moveCursor (QTextCursor::End);
}

void MainWindow::on_standardError()
{
    QString output = m_process->readAllStandardError();
    output.replace("\r","");

    s_textEdit->moveCursor (QTextCursor::End);
    s_textEdit->insertPlainText (output);
    s_textEdit->moveCursor (QTextCursor::End);
}

void MainWindow::closeEvent(QCloseEvent *ce)
{
    Q_UNUSED(ce)
    writeSettings();
}

