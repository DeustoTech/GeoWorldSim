#include "Intelligence.h"

#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QDebug>

GWSIntelligence::GWSIntelligence() : QObject() {

}

GWSIntelligence::~GWSIntelligence(){
    if( this->input_positions ){ delete this->input_positions; }
    if( this->input_minimums ){ delete this->input_minimums; }
    if( this->input_maximums ){ delete this->input_maximums; }
    if( this->output_positions ){ delete this->output_positions; }
    if( this->output_minimums ){ delete this->output_minimums; }
    if( this->output_maximums ){ delete this->output_maximums; }
}


/**********************************************************************
  METHODS
**********************************************************************/

void GWSIntelligence::trainFromFile(QString inputs_file_path, QString outputs_file_path) {

    QList< QList< QPair< QString , QVariant> > > inputs;
    {
        QFile file( inputs_file_path );

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning() << "NO FILE";
            return;
        }

        QTextStream in(&file);
        QMap<QString , int> headers;

        while( !in.atEnd() ) {

            QString line = in.readLine();

            // First read headers
            if( headers.isEmpty() ){
                QStringList columns = line.split(';');
                foreach( QString c , columns ){
                    // Remove surrounding quotes
                    //c.replace( "\"" , "" );
                    headers.insert( c.replace( "\"" , "" ) , columns.indexOf( c ) );
                }

                // Next reads are data
            } else {

                QList< QPair< QString, QVariant > > row;
                QStringList columns = line.split(';');
                for(int i = 0; i < columns.size() ; i++ ){

                    QString string_value = columns.at(i);
                    // Remove surrounding quotes
                    string_value.replace( "\"" , "" );

                    bool ok;
                    QVariant value = string_value.toDouble( &ok );
                    if( !ok ){ value = string_value; }

                    row.append( QPair< QString, QVariant>( headers.key( i ) , value ) );
                }

                inputs.insert( 0 , row );
            }
        }

        file.close();
    }


    QList< QList< QPair< QString , QVariant> > > outputs;
    {
        QFile file( outputs_file_path );

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning() << "NO FILE";
            return;
        }

        QTextStream in(&file);
        QMap<QString , int> headers;

        while( !in.atEnd() ) {

            QString line = in.readLine();

            // First read headers
            if( headers.isEmpty() ){
                QStringList columns = line.split(';');
                foreach( QString c , columns ){
                    headers.insert( c.replace( "\"" , "" ) , columns.indexOf( c ) );
                }

                // Next reads are data
            } else {

                QList< QPair< QString, QVariant > > row;
                QStringList columns = line.split(';');
                for(int i = 0; i < columns.size() ; i++ ){

                    QString string_value = columns.at(i);
                    // Remove surrounding quotes
                    string_value.replace( "\"" , "" );

                    bool ok;
                    QVariant value = string_value.toDouble( &ok );
                    if( !ok ){ value = string_value; }

                    row.append( QPair< QString, QVariant>( headers.key( i ) , value ) );
                }

                outputs.insert( 0 , row );
            }
        }

        file.close();
    }

    this->train( inputs , outputs );
}


void GWSIntelligence::trainFromJSON( QJsonArray input_train_dataset , QJsonArray output_train_dataset ){

    QList< QList< QPair< QString , QVariant> > > inputs;
    foreach( QJsonValue r , input_train_dataset ){
        QJsonObject row = r.toObject();
        QList< QPair< QString , QVariant> > qpair_list;
        foreach( QString key , row.keys() ){
            qpair_list.append( QPair<QString , QVariant>( key , row.value( key ).toVariant() ) );
        }
        inputs.insert( 0 , qpair_list );
    }

    QList< QList< QPair< QString , QVariant> > >  outputs;
    foreach( QJsonValue r , output_train_dataset ){
        QJsonObject row = r.toObject();
        QList< QPair< QString , QVariant> > qpair_list;
        foreach( QString key , row.keys() ){
            qpair_list.append( QPair<QString , QVariant>( key , row.value( key ).toVariant() ) );
        }
        outputs.insert( 0 , qpair_list );
    }

    this->train( inputs , outputs );

}


/**********************************************************************
  PROTECTED
**********************************************************************/

/*  Allocate positions to input fields and get maximum and minimum values
    for eventual normalization (FANN data formatting requirement)        */

void GWSIntelligence::generatePositions(QList< QList< QPair< QString , QVariant> > > data_rows, QMap<QString, int> *positions, QMap<QString, double> *maximums, QMap<QString, double> *minimums){

    // Give positions to the outputs
    for( int i = 0 ; i < data_rows.size() ; i ++ ) {

        QList< QPair< QString , QVariant> > row = data_rows.at(i);

        for( int j = 0 ; j < row.size() ; j ++ ){

            QPair< QString , QVariant > pair = row.at( j );
            QString hash = pair.first;
            QVariant value_variant = pair.second;
            hash = this->getIOName( hash , value_variant );
            double value_double = 0;

            if( value_variant.type() == QVariant::String ){
                value_double = 1;
            } else {
                value_double = value_variant.toDouble();
            }

            // Initialize position
            if( !positions->keys().contains( hash ) ){
                positions->insert( hash , positions->keys().size() );
            }

            // Initialize maximum
            if( !maximums->keys().contains( hash ) ){
                maximums->insert( hash , -9999999999999 );
            }
            maximums->insert( hash , qMax( maximums->value( hash ) , value_double ) );

            // Initialize minimum
            if( !minimums->keys().contains( hash ) ){
                minimums->insert( hash , 9999999999999 );
            }
            minimums->insert( hash , qMin( minimums->value( hash ) , value_double ) );
        }
    }

}

QString GWSIntelligence::getIOName( QString key, QVariant value){

    if( value.type() == QVariant::String ){
        return key + ":" + value.toString();
    }
    return key;
}

double GWSIntelligence::normalizeIO(QVariant value, QString hash , QMap< QString , double >* maximums , QMap< QString , double >* minimums){

    double value_double = 0;

    if( value.type() == QVariant::String ){
        value_double = 1;
    } else if ( value != 0.0 ){
        value_double = value.toDouble();
        double min = minimums->value( hash );
        double max = maximums->value( hash );

        // To normalize in [ 0 , 1 ] range
        value_double = ( value_double - min ) / ( max - min );

        // To normalize in [ -1 , 1 ] range
        //value_double = ((value_double - min )/(max - min) - 0.5 ) *2;
        //value_double = 2.0 * ( ( value_double - min ) / ( max - min ) ) - 1.0;
    }

    return value_double;
}


double GWSIntelligence::denormalizeIO( double normalized_value , int position ){

    double denormalized_value;

    double min = this->output_minimums->value( this->output_positions->key( position ) );
    double max = this->output_maximums->value( this->output_positions->key( position ) );

    // Denormalize from [ 0 , 1 ] range:
    denormalized_value = normalized_value * ( max - min) + min;

    // Denormalize from [ -1 , 1 ] range:
    //denormalized_value = ( normalized_value / 2.0 + 0.5) * ( max - min ) + min;
    return normalized_value;

}

