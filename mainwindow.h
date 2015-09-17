#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QShowEvent>
#include <QProcess>
#include <QTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static QTextEdit * s_textEdit;

private slots:
    void on_getImagePath_clicked();

    void on_getAdbPath_clicked();

    void on_delaySpinBox_valueChanged(int arg1);

    void on_deviceImagePathLineEdit_textChanged(const QString &arg1);

    void on_launchAdbButton_clicked();

    void on_saveButton_clicked();

    void on_standardOutput();
    void on_standardError();

protected:
    void closeEvent(QCloseEvent * ce);

private:
    Ui::MainWindow *ui;
    void generateConfigFile();
    void readSettings();
    void writeSettings();

    QProcess * m_process;
};

#endif // MAINWINDOW_H
