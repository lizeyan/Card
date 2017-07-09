#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H
class QNetworkAccessManager;
class DataSession : public QObject
{
    Q_OBJECT
public:
    explicit DataSession(QObject *parent = nullptr);

signals:

public slots:
private slots:
private:
    QNetworkAccessManager *mManager;
};

#endif // AUTHENTICATION_H
