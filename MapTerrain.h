#ifndef MAPTERRAIN_H
#define MAPTERRAIN_H

#include <QObject>

class MapTerrain : public QObject {
    Q_OBJECT
public:
    explicit MapTerrain(QObject *parent = nullptr, const QString &filePath = "");

    bool load();
    bool createBackup();
    bool save(const float &offset);

    float value(const int &index) const;
    void setValue(const int &index, const float &value);
    QString errorString() const;

    float maximalValue() const;
    float minimalValue() const;

private:
    QString filePath;
    float values[3722];
    QString _errorString;
    float _max, _min;

signals:
};

#endif // MAPTERRAIN_H
