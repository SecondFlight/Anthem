#ifndef MODELITEM_H
#define MODELITEM_H

#include <QSharedPointer>
#include <QObject>

#include "communicator.h"

#include "Include/rapidjson/document.h"

class ModelItem : public Communicator
{
    Q_OBJECT
private:
    QString key;
public:
    ModelItem(Communicator* parent, QString jsonKey);

    void patch(QString operation, QString from, QString path, rapidjson::Value& value);

    rapidjson::Value* jsonNode;
    Communicator* parent;

    virtual ~ModelItem();
};

#endif // MODELITEM_H
