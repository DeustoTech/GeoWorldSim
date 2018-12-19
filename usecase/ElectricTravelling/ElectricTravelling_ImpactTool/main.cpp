

#include <ios>
#include <iostream>
#include <iomanip>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>


// Utils
#include "../../../util/neural_network/NeuralNetwork.h"



int main(int argc, char **argv)
{
    try
    {
        QCoreApplication app( argc , argv );

        // Check if trained network file exists:
        QString save_file = argv[3];        
        QString GWSoutputs_file = argv[4];

        GWSNeuralNetwork* neural_network = new GWSNeuralNetwork();

        if ( !QFile( save_file ).exists() ){

            QString inputs_file = argv[1];
            QString outputs_file = argv[2];


            neural_network->setParameters( 0.7, 0.001, 300000, 1000 );
            neural_network->trainFromFile( inputs_file , outputs_file );
            //neural_network->save( save_file  , GWSoutputs_file );

        }

        else {

            neural_network->load( save_file , GWSoutputs_file );

        }


        QList< QPair <QString, QVariant> > test;
       /* test = { QPair<QString , QVariant>("Component" , "HC") , QPair<QString , QVariant>("TrafficSit" , "RUR/MW/80/Satur."),
               QPair<QString , QVariant>("Gradient" , 0) , QPair<QString , QVariant>("Subsegment" , "MC 4S >750cc Euro-1") ,
               QPair<QString , QVariant>("V" , 71.9621505737) , QPair<QString , QVariant>("V_0" , 0) , QPair<QString , QVariant>("V_100" , 0)};*/

        test = { QPair<QString , QVariant>("Component" , "HC") , QPair<QString , QVariant>("TrafficSit" , "RUR/MW/80/St+Go"),
                       QPair<QString , QVariant>("Gradient" , 0) , QPair<QString , QVariant>("Subsegment" , "MC 4S 251-750cc Euro-5") ,
                       QPair<QString , QVariant>("V" , 28.1581172943) , QPair<QString , QVariant>("V_0" , 0) , QPair<QString , QVariant>("V_100" , 0)};

        neural_network->run( test );
        qDebug() << neural_network->run( test );
        /*qDebug() << -1 << neural_network->run( { QPair<QString , QVariant>("val1" , 1 ), QPair<QString , QVariant>("val2" , 1 ) } );
        qDebug() << -1 << neural_network->run( { QPair<QString , QVariant>("val1" , -1 ), QPair<QString , QVariant>("val2" , -1 ) } );
        qDebug() << 1 << neural_network->run( { QPair<QString , QVariant>("val1" , -1 ), QPair<QString , QVariant>("val2" , 1 ) } );
*/
    }
    catch (const std::exception e )
    {
        qWarning() << endl << "Abnormal exception." << e.what() << endl;
    }
    return 0;
}

/******************************************************************************/

