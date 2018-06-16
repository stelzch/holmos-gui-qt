#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QObject>
#include <QException>
#include <QString>

class ImageRetrivalException : public QException
{
public:
    ImageRetrivalException(QString msg="");
    void raise() const;
    ImageRetrivalException *clone() const;
    QString msg;
};

#endif // EXCEPTIONS_H
