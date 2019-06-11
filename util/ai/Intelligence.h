#ifndef GWSINTELLIGENCE_H
#define GWSINTELLIGENCE_H

#include "../../object/Object.h"
#include <QMap>
#include <QMap>
#include <QJsonArray>
#include <QMutex>

class GWSIntelligence : public QObject
{
    Q_OBJECT

public:

    GWSIntelligence();
    ~GWSIntelligence();

    // INIT STRUCTURES
    void trainFromFile( QString inputs_file_path , QString outputs_file_path );
    void trainFromJSON( QJsonArray inputs_array , QJsonArray outputs_array  );
    void saveTrained( QString model_file_path , QString ios_file_path );
    void loadTrained( QString model_file_path , QString ios_file_path );

    // METHODS
    virtual void train( const QList< QMap< QString , QVariant> > &input_train_dataset, const QList< QMap< QString , QVariant> >  &output_train_dataset ) = 0;
    virtual QJsonObject run(  QMap< QString, QVariant> inputs ) = 0;

protected:

    // Inputs
    QMap< QString , int> input_positions; // { "input1:value_str1" : 0 , "input1:value_str2" : 1 , "input2" : 3 }
    QMap< QString , double > input_maximums; // { pos0 : 2.43 , pos1 : 42 ...
    QMap< QString , double > input_minimums; // { pos0 : 2.43 , pos1 : 0 ...
    QMap< QString , double > input_means; // { pos0 : 2.43 , pos1 : 42 ...
    QMap< QString , double > input_stdevs; // { pos0 : 2.43 , pos1 : 0 ...

    // Outputs
    QMap< QString , int> output_positions; // { "output1:value_str1" : 0 , "output1:value_str2" : 1 , "output2" : 3 }
    QMap< QString , double > output_maximums; // { pos0 : 2.43 , pos1 : 42 ...
    QMap< QString , double > output_minimums; // { pos0 : 2.43 , pos1 : 0 ...
    QMap< QString , double > output_means; // { pos0 : 2.43 , pos1 : 42 ...
    QMap< QString , double > output_stdevs; // { pos0 : 2.43 , pos1 : 0 ...

    // Methods
    void generatePositions( QList< QMap< QString , QVariant> > data_rows ,  QMap< QString , int> &positions , QMap< QString , double > &maximums , QMap< QString , double > &minimums , QMap< QString , double > &means , QMap< QString , double > &stdevs );
    QString getIOName( QString key , QVariant value );
    double normalizeIOMinMax( QVariant value , QString hash , QMap< QString , double > maximums , QMap< QString , double > minimums );
    double denormalizeIOMinMax( double normalized_value , int position );
    double normalizeIOMeanStdev( QVariant value , QString hash , QMap< QString , double > means , QMap< QString , double > stdevs );
    double denormalizeIOMeanStdev( double normalized_value , int position );
    virtual void saveModel( QString model_file_path ) = 0;
    virtual void loadModel( QString model_file_path ) = 0;

    // Unique mutex for ALL EXISTING GWSIntelligence classes, for avoiding concurrency
    static QMutex mutex;
};

#endif // GWSINTELLIGENCE_H
