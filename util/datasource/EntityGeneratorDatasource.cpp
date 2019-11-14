#include "EntityGeneratorDatasource.h"

#include "../../entity/Entity.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"

#include <QObject>
#include <QTimer>

geoworldsim::datasource::EntityGeneratorDatasource::EntityGeneratorDatasource(QJsonObject configuration){

    // CREATE POPULATION
    QList< DatasourceReader* >* pending_readers = Q_NULLPTR;

    QJsonObject json_population = configuration.value("population").toObject();
    QString user_id = configuration.value("user_id").toString();

    foreach( QString key , json_population.keys() ) {

        // Population type:
        QJsonObject population = json_population[ key ].toObject();

        if ( !population.value( "template" ).isUndefined() && !population.value( "datasources" ).isUndefined() ){

            QJsonArray datasources = population.value( "datasources" ).toArray();
            QJsonObject entity_template = population.value( "template" ).toObject();

            for ( int i = 0; i <  datasources.size() ; ++i ){

                QJsonObject datasource = datasources.at( i ).toObject();
                QString scenario_id = datasource.value("scenario_id").toString();
                int limit = datasource.value("limit").toInt(-1);
                QString entity_type = datasource.value("entity_type").toString();
                QString entity_filter = datasource.value("entity_filter").toString();

                if( scenario_id.isEmpty() || entity_type.isEmpty() ){
                    qWarning() << "Asked to download from scenario without ID or entity_type";
                }

                DatasourceReader* reader = this->generateEntities( entity_template , user_id , scenario_id,  entity_type , entity_filter , limit );

                if( !reader ){
                    continue;
                }

                if( !pending_readers ){
                    pending_readers = new QList< DatasourceReader* >();
                }
                pending_readers->append( reader );

                reader->connect( reader , &DatasourceReader::dataReadingFinishedSignal , [ this , reader , pending_readers ](){

                    if( pending_readers ){
                        pending_readers->removeAll( reader );
                    }
                    reader->deleteLater();

                    if( pending_readers && pending_readers->isEmpty() ){
                        delete pending_readers;
                        emit this->dataReadingFinishedSignal();
                    }
                });

            }
        }

        if ( !population.value("template").isUndefined() && !population.value("amount").isUndefined() ){
            for ( int i = 0; i < population.value("amount").toInt() ; i++){
                // Use template to generate amount entities
                QSharedPointer<geoworldsim::Object> entity = ObjectFactory::globalInstance()->fromJSON( population.value("template").toObject() );
            }
        }

       qInfo() << QString("Creating population %1").arg( key );
    }

    if( !pending_readers ){
        emit this->dataReadingFinishedSignal();
    }

}

geoworldsim::datasource::EntityGeneratorDatasource::EntityGeneratorDatasource( QJsonObject entity_template , QString user_id , QString scenario_id , QString entity_type , QString entity_filter , int amount ) : QObject (){
    DatasourceReader* reader = this->generateEntities( entity_template , user_id , scenario_id , entity_type , entity_filter , amount );


    if( !reader ){
        emit this->dataReadingFinishedSignal();
        return;
    }

    reader->connect( reader , &DatasourceReader::dataReadingFinishedSignal , [ reader , this ](){
        emit this->dataReadingFinishedSignal();
        reader->deleteLater();
    });
}

geoworldsim::datasource::EntityGeneratorDatasource::~EntityGeneratorDatasource(){
}

QJsonObject geoworldsim::datasource::EntityGeneratorDatasource::joinJSON( QJsonObject json_template , QJsonObject json_data ){

    foreach ( QString key , json_data.keys() ) {

        QJsonValue value = json_data.value( key );

        // If value from template must prevail, skip data joining
        if( json_template.value( key ).toObject().value( "override" ).toBool() ){
            value = json_template.value( key );
        }

        // Extract THE_REAL_VALUE from { type : "string|number|..." , metadata : {} , value : THE_REAL_VALUE }
        while( value.toObject().keys().contains( "value" ) ){
            value = value.toObject().value("value");
        }

        // IF Value is a complex object, recursively call joinJSON
        if( value.isObject() ){
            json_template.insert( key , this->joinJSON( json_template.value( key ).toObject() , value.toObject() ) );
        } else {
            json_template.insert( key , value );
        }

    }

    return json_template;
}

geoworldsim::datasource::DatasourceReader* geoworldsim::datasource::EntityGeneratorDatasource::generateEntities(QJsonObject entity_template, QString user_id, QString scenario_id, QString entity_type, QString entity_filter, int amount){

    if( entity_template.isEmpty() ){
        qCritical() << "Empty JSON template to join with the datasource";
        return Q_NULLPTR;
    }

    DatasourceReader* reader = new DatasourceReader( user_id , scenario_id , entity_type , entity_filter , amount > 0 ? amount : 999999999999999 );
    reader->connect( reader , &DatasourceReader::dataValueReadSignal , [this , entity_template]( QJsonObject data ){

        // FIXME
        if( data.value( "location" ).isObject() ){
            data.insert( "geometry" , data.value( "location" ).toObject() );
        }

        QJsonObject template_to_be_constructed = this->joinJSON( entity_template , data );
        if ( template_to_be_constructed.isEmpty() ){
            return;
        }

        // Check if agent already exists
        QString new_entity_id = template_to_be_constructed.value( Object::GWS_UID_PROP ).toString();
        QSharedPointer< Entity > entity = environment::EntityEnvironment::globalInstance()->getByUID( new_entity_id );
        if( entity ){
            //qWarning() << QString("Skipping duplicate (Duplicate UID %1 found)").arg( new_entity_id );
            return;
        } else {
            entity = ObjectFactory::globalInstance()->fromJSON( template_to_be_constructed ).dynamicCast< Entity >();
        }

    });

    reader->startReading();
    return reader;
}
