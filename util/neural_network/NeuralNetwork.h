#ifndef GWSNEURALNETWORK_H
#define GWSNEURALNETWORK_H

#include "../../util/ai/Intelligence.h"
#include <QMap>
#include <QJsonValue>
#include <QJsonArray>

#include "fann.h"
#include "fann_cpp.h"
#include "fann_train.h"
#include "fann_data.h"

class GWSNeuralNetwork : public GWSIntelligence{
    Q_OBJECT

public:
    explicit GWSNeuralNetwork();
    ~GWSNeuralNetwork();

    // METHODS
    void setParameters(double learning_rate,  double desired_error, int max_iterations, int iterations_between_reports);
    void train( QList< QList< QPair< QString , QVariant> > > input_train_dataset, QList< QList< QPair< QString , QVariant> > >  output_train_dataset );
    void save( const QString fann_trained_network_filename , const QString gws_neural_network_filename );
    void load( const QString fann_trained_network_filename , const QString gws_neural_network_filename );
    QJsonObject run(  QList<QPair <QString, QVariant> > inputs );
    QJsonObject randomLine( QString inputs_file_path );

private:

    // NN Parameters
    float learning_rate;
    unsigned int num_hidden;

    float desired_error;
    unsigned int max_iterations;
    unsigned int iterations_between_reports;

    FANN::training_data train_data;
    FANN::neural_net net;

};

#endif // GWSNEURALNETWORK_H
