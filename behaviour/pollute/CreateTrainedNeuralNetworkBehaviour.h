#ifndef CREATETRAINEDNEURALNETWORK_H
#define CREATETRAINEDNEURALNETWORK_H

#include "doublefann.h"
#include "fann_cpp.h"
#include "fann_train.h"
#include "fann_data.h"
#include "fann_internal.h"
#include "fann_io.h"
#include "fann_error.h"
#include "../../behaviour/Behaviour.h"

class CreateTrainedNeuralNetworkBehaviour : public GWSBehaviour
{

    Q_OBJECT

public:
    Q_INVOKABLE explicit CreateTrainedNeuralNetworkBehaviour();

    // PARAMETERS:
    static QString URL;
    static QString NEXT;


private slots:
    QJsonArray behave();
};

#endif // CREATETRAINEDNEURALNETWORK_H
