#ifndef MAIN_H
#define MAIN_H
#include <QObject>
class Signal : public QObject
{
    Q_OBJECT

public:
    Signal(QObject *parent = nullptr);

signals:
    void publisherListChanged();
};
#endif // MAIN_H
