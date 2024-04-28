#include "MapTerrain.h"

#include <QtCore>

#include <QFile>

MapTerrain::MapTerrain(QObject *parent, const QString &filePath) :
    QObject{parent},
    filePath(filePath)
{
    // load
}

bool MapTerrain::load() {
    QFile f(filePath);

    if(!f.open(QFile::ReadOnly)) {
        return false;
        _errorString = f.errorString();
    }

    QDataStream s(&f);
    s.setFloatingPointPrecision(QDataStream::SinglePrecision);
    s.setByteOrder(QDataStream::LittleEndian);

    _min = 0;
    _max = 0;

    for(int i = 0; i < 3722; i++) {
        if(s.atEnd()) {
            f.close();
            _errorString = tr("unexpeted end of file");
            return false;
        }

        float val;
        s >> val;
        values[i] = val;

        if(val < _min)
            _min = val;

        if(val > _max)
            _max = val;
    }

    f.close();
    return true;
}

bool MapTerrain::createBackup() {
    QFileInfo fi(filePath);
    QDir backupDir(fi.dir().path() + "/TerrainMover_Backup");

    if(!backupDir.exists()) {
        if(!backupDir.mkpath(backupDir.path())) {
            _errorString = tr("Couldn't create backup directory");
            return false;
        }
    }

    QFile f(filePath);

    QString targetPath = fi.path() + "/TerrainMover_Backup/" + fi.fileName();
    if(!f.rename(targetPath)) {
        _errorString = f.errorString();
        return false;
    }

    return true;
}

bool MapTerrain::save(const float &offset) {
    QFile f(filePath);

    if(!f.open(QFile::WriteOnly)) {
        _errorString = f.errorString();
        return false;
    }

    QDataStream s(&f);
    s.setFloatingPointPrecision(QDataStream::SinglePrecision);
    s.setByteOrder(QDataStream::LittleEndian);

    for(int i = 0; i < 3722; i++) {
        if(i == 0)
            s << values[i];
        else
            s << values[i] + offset;
    }

    f.close();

    return true;
}

float MapTerrain::value(const int &index) const {
    if(index <= 0 || index > 3721)
        return 0;

    return values[index + 1];
}

void MapTerrain::setValue(const int &index, const float &value) {
    if(index <= 0 || index > 3721)
        return;

    values[index + 1] = value;
}

QString MapTerrain::errorString() const {
    return _errorString;
}

float MapTerrain::maximalValue() const {
    return _max;
}

float MapTerrain::minimalValue() const {
    return _min;
}
