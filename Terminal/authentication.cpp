#include "authentication.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
DataSession::DataSession(QObject *parent) : QObject(parent)
  , mManager(new QNetworkAccessManager(this))
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://www.lvxin.com"));
    QNetworkReply *reply = mManager->get(request);
    connect(reply, SIGNAL(readyRead()), this, SLOT(getToken()));
}

void DataSession::getToken()
{

}
