#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MapTerrain.h"

#include <QMessageBox>
#include <QFile>
#include <QFileDialog>

#include <QPainter>

#include <QProgressBar>

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

    MapTerrain t(this, ui->lwSelectedFiles->item(0)->toolTip());
    if(!t.load()) {
        QMessageBox::warning(this, tr("Loading preview"), tr("<p><b>Couldn't load preivew:</b></p><p>%1</p>").arg(t.errorString()));
    }

    QPixmap pixmap(244, 244);
    pixmap.fill(Qt::black);


    float originalArray[61][61];
    float newTerrain[61][61];

    for(int i = 0; i < 61; ++i) {
        for(int j = 0; j < 61; ++j) {
            originalArray[i][j] = t.value(i * 61 + j);
        }
    }

    for (int i = 0; i < 61; ++i) {
        for (int j = 0; j < 61; ++j) {
            newTerrain[61 - 1 - j][i] = originalArray[i][j];
        }
    }

    for(int i = 0; i < 61; ++i) {
        for(int j = 0; j < 61; ++j) {
            float colorValue = newTerrain[i][j] * (1 / t.maximalValue()) * 255;

            QPainter painter(&pixmap);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(colorValue, colorValue, colorValue));
            painter.drawRect(i * 4, j * 4, 4, 4);
        }
    }

    ui->lPreview->setPixmap(pixmap);
}

void MainWindow::on_dsbMoveTerrain_valueChanged(double arg1) {
    ui->hsMoveTerrain->setValue(arg1);
}

void MainWindow::on_hsMoveTerrain_valueChanged(int value) {
    ui->dsbMoveTerrain->setValue(value);
}

void MainWindow::on_pushButton_clicked() {
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Moving terrain..."));
    QProgressBar pgBar;
    pgBar.setMaximum(ui->lwSelectedFiles->count());
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->addWidget(&pgBar);
    dlg.setLayout(layout);

    dlg.show();

    for(int i = 0; i < ui->lwSelectedFiles->count(); i++) {
        runTile(ui->lwSelectedFiles->item(i)->toolTip(), i);
        qApp->processEvents();
        pgBar.setValue(i + 1);
    }

    dlg.hide();
    QMessageBox::information(this, tr("Finished"), tr("Finished successfully!"));
}

void MainWindow::runTile(const QString &filePath, const int &index) {
    QFileInfo fi(filePath);
    MapTerrain t(this, filePath);
    if(!t.load()) {
        QMessageBox::critical(this, tr("Couldn't read terrain file"), tr("<p><b>Couldn't read terrain file %1:</b></p><p>%2</p>").arg(fi.fileName(), t.errorString()));
        ui->lwSelectedFiles->item(index)->setForeground(Qt::red);
        return;
    }

    if(ui->cbCreateBackup->isChecked()) {
        if(!t.createBackup()) {
            QMessageBox::critical(this, tr("Couldn't create backup file"), tr("<p><b>Couldn't create backup file %1:</b></p><p>%2</p>").arg(fi.fileName(), t.errorString()));
            ui->lwSelectedFiles->item(index)->setForeground(Qt::red);
            return;
        }
    }

    if(!t.save(ui->dsbMoveTerrain->value())) {
        QMessageBox::critical(this, tr("Couldn't modify file"), tr("<p><b>Couldn't create modified file %1:</b></p><p>%2</p>").arg(fi.fileName(), t.errorString()));
        ui->lwSelectedFiles->item(index)->setForeground(Qt::red);
        return;
    }

    ui->lwSelectedFiles->item(index)->setForeground(Qt::darkGreen);
}
