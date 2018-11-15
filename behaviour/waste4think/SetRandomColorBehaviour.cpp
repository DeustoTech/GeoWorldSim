#include "SetRandomColorBehaviour.h"


QString SetRandomColorBehaviour::NEXTS = "nexts";


SetRandomColorBehaviour::SetRandomColorBehaviour() : GWSBehaviour(){

}



QStringList SetRandomColorBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Generate random color
    QString random_color = QColor::colorNames().at( qrand() % QColor::colorNames().size() ) ;
    agent->setProperty( "color" , random_color);

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;
}
