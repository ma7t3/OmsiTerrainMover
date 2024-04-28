#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbSelectFiles_clicked();
    void on_pbRemoveSelectedFiles_clicked();
    void on_pbRemoveAllFiles_clicked();

    void refreshPreview();

    void on_dsbMoveTerrain_valueChanged(double arg1);
    void on_hsMoveTerrain_valueChanged(int value);
    void on_pushButton_clicked();

    void runTile(const QString &filePath, const int &index);

private:
    Ui::MainWindow *ui;
    QSettings settings;
};
#endif // MAINWINDOW_H
