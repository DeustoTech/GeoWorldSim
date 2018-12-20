#ifndef GWSINTELLIGENCE_H
#define GWSINTELLIGENCE_H

#include <QObject>
#include <QMap>
#include <QJsonArray>

class GWSIntelligence : public QObject
{
    Q_OBJECT

public:

    GWSIntelligence();
    ~GWSIntelligence();

    // INIT STRUCTURES
    void trainFromFile( QString inputs_file_path , QString outputs_file_path );
    void trainFromJSON( QJsonArray inputs_array , QJsonArray outputs_array  );

    // METHODS
    virtual void train( QList< QList< QPair< std::string , QVariant> > > input_train_dataset, QList< QList< QPair< std::string , QVariant> > >  output_train_dataset ) = 0;
    virtual QJsonObject run(  QList<QPair < std::string, QVariant> > inputs ) = 0;

protected:

    // Inputs
    QMap< std::string , int>* input_positions = Q_NULLPTR; // { "input1:value_str1" : 0 , "input1:value_str2" : 1 , "input2" : 3 }
    QMap< std::string , double >* input_maximums = Q_NULLPTR;; // { pos0 : 2.43 , pos1 : 42 ...
    QMap< std::string , double >* input_minimums = Q_NULLPTR;; // { pos0 : 2.43 , pos1 : 0 ...

    // Outputs
    QMap< std::string , int>* output_positions = Q_NULLPTR;; // { "output1:value_str1" : 0 , "output1:value_str2" : 1 , "output2" : 3 }
    QMap< std::string , double >* output_maximums = Q_NULLPTR;; // { pos0 : 2.43 , pos1 : 42 ...
    QMap< std::string , double >* output_minimums = Q_NULLPTR;; // { pos0 : 2.43 , pos1 : 0 ...

    // Methods
    void generatePositions( QList< QList< QPair< std::string , QVariant> > > data_rows ,  QMap< std::string , int>* positions , QMap< std::string , double >* maximums , QMap< std::string , double >* minimums );
    std::string getIOName( std::string key , QVariant value );
    double normalizeIO( QVariant value , std::string hash , QMap< std::string , double >* maximums , QMap< std::string , double >* minimums );
    double denormalizeIO( double normalized_value , int position );

};

#endif // GWSINTELLIGENCE_H
