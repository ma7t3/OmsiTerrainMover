#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("ma7t3", "OmsiTerrainMover") {
    ui->setupUi(this);

    refreshPreview();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pbSelectFiles_clicked() {
    QString lastDir = settings.value("lastDir", "C:/Program Files (x86)/Steam/steamapps/common/OMSI 2/maps").toString();
    QStringList files = QFileDialog::getOpenFileNames(this, "", lastDir, tr("Omsi Terrain File (*.terrain)"));

    if(files.isEmpty())
        return;

    QFileInfo firstFi(files[0]);
    settings.setValue("lastDir", firstFi.dir().path());

    QStringList existingFiles;
    for(int i = 0; i < ui->lwSelectedFiles->count(); i++)
        existingFiles << ui->lwSelectedFiles->item(i)->toolTip();

    foreach(QString str, files) {
        if(existingFiles.contains(str))
            continue;

        QFileInfo fi(str);
        QListWidgetItem *itm = new QListWidgetItem(ui->lwSelectedFiles);
        itm->setText(fi.fileName());
        itm->setToolTip(str);
    }

    refreshPreview();
}

void MainWindow::on_pbRemoveSelectedFiles_clicked() {
    if(!ui->lwSelectedFiles->currentItem())
        return;

    delete ui->lwSelectedFiles->currentItem();
    refreshPreview();
}

void MainWindow::on_pbRemoveAllFiles_clicked() {
    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Remove all files"), tr("Do you really want to remove all files from the list?"), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    ui->lwSelectedFiles->clear();
    refreshPreview();
}

void MainWindow::refreshPreview() {
    if(ui->lwSelectedFiles->count() != 1) {
        ui->lPreview->setPixmap(QPixmap());
        return;
    }

    QPixmap pixmap(244, 244);
    pixmap.fill(Qt::black);

    // draw preview here

    ui->lPreview->setPixmap(pixmap);
}

void MainWindow::on_dsbMoveTerrain_valueChanged(double arg1) {
    ui->hsMoveTerrain->setValue(arg1);
}

void MainWindow::on_hsMoveTerrain_valueChanged(int value) {
    ui->dsbMoveTerrain->setValue(value);
}

