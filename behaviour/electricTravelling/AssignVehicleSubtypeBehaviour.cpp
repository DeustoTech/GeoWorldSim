#include "AssignVehicleSubtypeBehaviour.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../util/random/RouletteWheel.h"

QString geoworldsim::behaviour::AssignVehicleSubtypeBehaviour::VEHICLE_TYPE = "vehicle_type";
QString geoworldsim::behaviour::AssignVehicleSubtypeBehaviour::VEHICLE_SUBTYPE = "vehicle_subtype";
QString geoworldsim::behaviour::AssignVehicleSubtypeBehaviour::DISTRIBUTION = "distribution";
QString geoworldsim::behaviour::AssignVehicleSubtypeBehaviour::NEXTS = "nexts";


geoworldsim::behaviour::AssignVehicleSubtypeBehaviour::AssignVehicleSubtypeBehaviour() : Behaviour(){

}

QPair<double, QJsonArray> geoworldsim::behaviour::AssignVehicleSubtypeBehaviour::behave(){

    QSharedPointer<Entity> agent = this->getEntity();
    QString entity_type = agent->getProperty( VEHICLE_TYPE ).toString();

    geoworldsim::random::RouletteWheel* roulete_wheel = new geoworldsim::random::RouletteWheel();

    QList< QSharedPointer<geoworldsim::Entity> > distribution_agent = environment::EntityEnvironment::globalInstance()->getByClass("DISTRIBUTION");

    foreach (QSharedPointer<geoworldsim::Entity> ds, distribution_agent) {

        if( ds->getProperty( VEHICLE_TYPE ) == entity_type){
            QJsonArray distribution = ds->getProperty( DISTRIBUTION ).toArray();

            foreach(QJsonValue d, distribution){
                double rate = d.toObject().value("rate").toDouble() / 100;
                QString subtype = d.toObject().value("subtype").toString();

                Object* obj = new Object();
                obj->setObjectName(subtype);
                roulete_wheel->addElement( obj, rate);
            }
            break;
        }
    }

    qDebug() << roulete_wheel->isEmpty() << entity_type;

    QString subtype = roulete_wheel->getElement()->objectName();
    agent->setProperty( VEHICLE_SUBTYPE, subtype );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
