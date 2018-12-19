#ifndef GWSSVM_H
#define GWSSVM_H

#include "../../../GeoWorldSim/util/ai/Intelligence.h"

#include <libsvm/svm.h>

class GWSSvm : public GWSIntelligence
{
    Q_OBJECT

public:
    explicit GWSSvm();
    ~GWSSvm();

    // METHODS
    virtual void train( QList< QList< QPair< QString , QVariant> > > input_train_dataset, QList< QList< QPair< QString , QVariant> > >  output_train_dataset ) = 0;
    virtual QJsonObject run(  QList<QPair <QString, QVariant> > inputs ) = 0;




private:
};

#endif // GWSSVM_H
