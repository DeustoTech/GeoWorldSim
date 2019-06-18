#include "EntityGeneratorDatasource.h"

#include "../../app/App.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../entity/Entity.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include <QObject>
#include <QTimer>

GWSEntityGeneratorDatasource::GWSEntityGeneratorDatasource(QJsonObject json, QString scenario_id , QString entity_type , QString entity_filter , int amount ) : QObject ()
{

    if( json.isEmpty() ){
        qCritical() << "Empty JSON template to join with the datasource";
        return;
    }

    GWSDatasourceReader* reader = new GWSDatasourceReader( scenario_id , entity_type , entity_filter , amount );
    reader->connect( reader , &GWSDatasourceReader::dataReadingFinishedSignal , [ reader , this ](){
        emit this->dataReadingFinishedSignal();
        reader->deleteLater();
    });
    reader->connect( reader , &GWSDatasourceReader::dataValueReadSignal , [this , json]( QJsonObject data ){

        QJsonObject template_to_be_constructed = this->joinJSON( json , data );
        if ( template_to_be_constructed.isEmpty() ){
            return;
        }

        // Check if agent already exists
        QString new_entity_id = template_to_be_constructed.value( GWSObject::GWS_UID_PROP ).toString();
        QSharedPointer<GWSEntity> entity = GWSEntityEnvironment::globalInstance()->getByUID( new_entity_id );
        if( entity ){
            //qWarning() << QString("Skipping duplicate (Duplicate UID %1 found)").arg( new_entity_id );
            return;
        } else {
            entity = GWSObjectFactory::globalInstance()->fromJSON( template_to_be_constructed ).dynamicCast<GWSEntity>();
        }

    });

    reader->startReading();

}

QJsonObject GWSEntityGeneratorDatasource::joinJSON(QJsonObject json_template, QJsonObject json_data){

    foreach ( QString key , json_data.keys() ) {

        QJsonValue value = json_data.value( key );

        // If value from template must prevail, skip data joining
        if( json_template.value( key ).toObject().value( "override" ).toBool() ){
            value = json_template.value( key );
        }

        // Extract THE_REAL_VALUE from { type : "string|number|..." , metadata : {} , value : THE_REAL_VALUE }
        if( value.toObject().keys().contains("value") ){ value = value.toObject().value("value"); }

        // IF Value is a complex object, recursively call joinJSON
        if( value.isObject() ){
            json_template.insert( key , this->joinJSON( json_template.value( key ).toObject() , value.toObject() ) );
        } else {
            json_template.insert( key , value );
        }

    }

    return json_template;
}
