#include "Intelligence.h"

#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QDebug>
#include <QHash>

GWSIntelligence::GWSIntelligence() : QObject() {

}

GWSIntelligence::~GWSIntelligence(){
}


/**********************************************************************
  METHODS
**********************************************************************/

void GWSIntelligence::trainFromFile(QString inputs_file_path, QString outputs_file_path) {

    QList< QMap< QString , QVariant> > inputs;
    {
        QFile file( inputs_file_path );

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning() << "NO VALID INPUT FILE";
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

                QMap< QString, QVariant > row;
                QStringList columns = line.split(';');
                for(int i = 0; i < columns.size() ; i++ ){

                    QString string_value = columns.at(i);
                    // Remove surrounding quotes
                    string_value.replace( "\"" , "" );

                    bool ok;
                    QVariant value = string_value.toDouble( &ok );
                    if( !ok ){ value = string_value; }

                    row.insert( headers.key( i ) , value );
                }

                inputs.insert( 0 , row );
            }
        }

        file.close();
    }


    QList< QMap< QString , QVariant> > outputs;
    {
        QFile file( outputs_file_path );

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning() << "NO VALID OUTPUT FILE";
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

                QMap< QString, QVariant > row;
                QStringList columns = line.split(';');
                for(int i = 0; i < columns.size() ; i++ ){

                    QString string_value = columns.at(i);
                    // Remove surrounding quotes
                    string_value.replace( "\"" , "" );

                    bool ok;
                    QVariant value = string_value.toDouble( &ok );
                    if( !ok ){ value = string_value; }

                    row.insert( headers.key( i ) , value );
                }

                outputs.insert( 0 , row );
            }
        }

        file.close();
    }

    this->generatePositions( inputs  , this->input_positions , this->input_maximums , this->input_minimums );
    this->generatePositions( outputs , this->output_positions , this->output_maximums , this->output_minimums );

    this->train( inputs , outputs );
}


void GWSIntelligence::trainFromJSON( QJsonArray input_train_dataset , QJsonArray output_train_dataset ){

    QList< QMap< QString , QVariant> > inputs;
    foreach( QJsonValue r , input_train_dataset ){
        QJsonObject row = r.toObject();
        QMap< QString , QVariant > qpair_list;
        foreach( QString key , row.keys() ){
            qpair_list.insert( key , row.value( key ).toVariant() );
        }
        inputs.insert( 0 , qpair_list );
    }

    QList< QMap< QString , QVariant> >  outputs;
    foreach( QJsonValue r , output_train_dataset ){
        QJsonObject row = r.toObject();
        QMap< QString , QVariant> qpair_list;
        foreach( QString key , row.keys() ){
            qpair_list.insert( key , row.value( key ).toVariant() );
        }
        outputs.insert( 0 , qpair_list );
    }

    this->train( inputs , outputs );

}

void GWSIntelligence::saveTrained(QString model_file_path, QString ios_file_path){

    // Save Input / output positions
    QFile out( ios_file_path );
       if( out.open(QIODevice::ReadWrite) ) {
           QTextStream stream(&out);

           // Input positions
           {
               QString str;
               for(int i = 0 ; i < this->input_positions.values().size() ; i++){
                   if( !str.isEmpty() ){ str += ";"; }
                   str += this->input_positions.key( i );
               }
               stream << str << endl;
           }

           // Input maximums
           {
               QString str;
               for(int i = 0 ; i < this->input_positions.values().size() ; i++){
                   if( !str.isEmpty() ){ str += ";"; }
                   str += QString::number( this->input_maximums.value( this->input_positions.key( i ) ) );
               }
               stream << str << endl;
           }

           // Input minimums
           {
               QString str;
               for(int i = 0 ; i < this->input_positions.values().size() ; i++){
                   if( !str.isEmpty() ){ str += ";"; }
                   str += QString::number( this->input_minimums.value( this->input_positions.key( i ) ) );
               }
               stream << str << endl;
           }

           // Output positions
           {
               QString str;
               for(int i = 0 ; i < this->output_positions.values().size() ; i++){
                   if( !str.isEmpty() ){ str += ";"; }
                   str += this->output_positions.key( i );
               }
               stream << str << endl;
           }

           // Output maximums
           {
               QString str;
               for(int i = 0 ; i < this->output_positions.values().size() ; i++){
                   if( !str.isEmpty() ){ str += ";"; }
                   str += QString::number( this->output_maximums.value( this->output_positions.key( i ) ) );
               }
               stream << str << endl;
           }

           // Output minimums
           {
               QString str;
               for(int i = 0 ; i < this->output_positions.values().size() ; i++){
                   if( !str.isEmpty() ){ str += ";"; }
                   str += QString::number( this->output_minimums.value( this->output_positions.key( i ) ) );
               }
               stream << str << endl;
           }
       }


    // Call specific library
    this->saveModel( model_file_path );
}




/* Load trained network */
void GWSIntelligence::loadTrained( QString model_file_path, QString ios_file_path ){

    // Load trained:
    this->loadModel( model_file_path );

    QFile in( ios_file_path );
    if( in.open(QIODevice::ReadWrite) ) {

        QTextStream stream(&in);

        // First line = input_positions
        {
            QString line = stream.readLine();
            foreach( QString key , line.split(";") ){
                this->input_positions.insert( key , this->input_positions.size() );
            }

        }

        // Second line = input_maximums
        {
            QString line = stream.readLine();
            QStringList values = line.split(";");
            for( int i = 0 ; i < values.size() ; i++ ){
                QString key = this->input_positions.key( i );
                this->input_maximums.insert( key , values.at( i ).toDouble() );
            }
        }

        // Third line = input_minimums
        {
            QString line = stream.readLine();
            QStringList values = line.split(";");
            for( int i = 0 ; i < values.size() ; i++ ){
                QString key = this->input_positions.key( i );
                this->input_minimums.insert( key , values.at( i ).toDouble() );
            }
        }

        // Fourth line = output_positions
        {
            QString line = stream.readLine();
            foreach( QString key , line.split(";") ){
                this->output_positions.insert( key , this->output_positions.size() );
            }
        }

        // Fifth line = output_maximums
        {
            QString line = stream.readLine();
            QStringList values = line.split(";");
            for( int i = 0 ; i < values.size() ; i++ ){
                QString key = this->output_positions.key( i );
                this->output_maximums.insert( key , values.at( i ).toDouble() );
            }
        }

        // Sixth line = output_minimums
        {
            QString line = stream.readLine();
            QStringList values = line.split(";");
            for( int i = 0 ; i < values.size() ; i++ ){
                QString key = this->output_positions.key( i );
                this->output_minimums.insert( key , values.at( i ).toDouble() );
            }
        }
    }
}






/**********************************************************************
  PROTECTED
**********************************************************************/

/*  Allocate positions to input fields and get maximum and minimum values
    for eventual normalization (FANN data formatting requirement)        */

void GWSIntelligence::generatePositions(QList<QMap<QString, QVariant> > data_rows, QMap<QString, int> &positions, QMap<QString, double> &maximums, QMap<QString, double> &minimums){

    // Give positions to the outputs
    for( int i = 0 ; i < data_rows.size() ; i ++ ) {

        QMap< QString , QVariant> row = data_rows.at(i);

        foreach( QString key , row.keys() ){

            QString hash = key;
            QVariant value_variant = row.value( key );
            hash = this->getIOName( hash , value_variant );
            double value_double = 0;

            if( value_variant.type() == QVariant::String ){
                value_double = 1;
            } else {
                value_double = value_variant.toDouble();
            }

            // Initialize position
            if( !positions.keys().contains( hash ) ){
                positions.insert( hash , positions.keys().size() );
            }

            // Initialize maximum
            if( !maximums.keys().contains( hash ) ){
                maximums.insert( hash , -9999999999999 );
            }
            maximums.insert( hash , qMax( maximums.value( hash ) , value_double ) );

            // Initialize minimum
            if( !minimums.keys().contains( hash ) ){
                minimums.insert( hash , 9999999999999 );
            }
            minimums.insert( hash , qMin( minimums.value( hash ) , value_double ) );
        }
    }

}

QString GWSIntelligence::getIOName( QString key , QVariant value){

    if( value.type() == QVariant::String ){
        return QString("%1:%2").arg( key ).arg( value.toString() );
    }

    return QString("%1").arg( key );
}

double GWSIntelligence::normalizeIO(QVariant value, QString hash, QMap<QString, double> maximums, QMap<QString, double> minimums){

    double value_double = value.toDouble();

    if( value.type() == QVariant::String ){
        value_double = 1;
    } else {

        double min = minimums.value( hash );
        double max = maximums.value( hash );

        // To normalize in [ 0 , 1 ] range
        value_double = ( value_double - min ) / ( max - min );

        // To normalize in [ -1 , 1 ] range
        //value_double = ((value_double - min )/(max - min) - 0.5 ) *2;
    }

    return value_double;
}


double GWSIntelligence::denormalizeIO( double normalized_value , int position ){

    double min = this->output_minimums.value( this->output_positions.key( position ) );
    double max = this->output_maximums.value( this->output_positions.key( position ) );

    // Denormalize from [ 0 , 1 ] range:
    normalized_value = normalized_value * ( max - min ) + min;

    // Denormalize from [ -1 , 1 ] range:
    //qDebug() << normalized_value << min << max << this->output_positions.keys();
    //normalized_value = ( normalized_value / 2.0 + 0.5) * ( max - min ) + min;
    qDebug() << normalized_value;
    return normalized_value;

}

