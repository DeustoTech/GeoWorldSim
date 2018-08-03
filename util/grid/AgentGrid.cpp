#include "AgentGrid.h"

#include <QJsonArray>
#include <QDebug>

GWSAgentGrid::GWSAgentGrid(GWSAgent *agent) : GWSGrid( agent ){

}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSAgentGrid::deserialize(QJsonObject json){
    GWSGrid::deserialize( json ); // Creates the empty grid
    if( json.value( GWSGrid::GRID_VALUES_PROP ).isArray() ){

        QJsonArray a1 = json.value( GWSGrid::GRID_VALUES_PROP ).toArray();
        for( int i = 0 ; i < a1.size() ; i++){
            QJsonArray a2 = a1.at( i ).toArray();
            for( int j = 0 ; j < a2.size() ; j++){
                //this->values[i][j] = a2.at( j ).toDouble();
            }
        }
    }
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSAgentGrid::serialize() const{
    QJsonObject json = GWSGrid::serialize();
    QJsonArray a1;
    for(int i = 0; i < this->values.size() ; i++){
        QJsonArray a2;
        for(int j = 0; j < this->values[i].size() ; j++){
            //a2.append( this->getGridCellValue( i , j ) );
        }
        a1.append( a2 );
    }
    json.insert( GWSGrid::GRID_VALUES_PROP , a1 );
    return json;
}

/**********************************************************************
 GETTERS
**********************************************************************/

QList<GWSAgent*> GWSAgentGrid::getGridCellValue(unsigned int grid_x, unsigned int grid_y){
    if( grid_x < 0 || grid_x > this->getGridXSize() || grid_y < 0 || grid_y > this->getGridYSize() ){
        return QList<GWSAgent*>();
    }
    qDebug() << grid_x << grid_y << this->getGridXSize() << this->getGridYSize();
    return this->values.value(grid_x).value(grid_y);
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSAgentGrid::setGridSize(unsigned int x_size, unsigned int y_size){
    GWSGrid::setGridSize( x_size , y_size );
    this->values.clear();
    this->values = QVector< QVector< QList<GWSAgent*> > >( x_size );
    for( int i = 0 ; i < this->values.size() ; i++){
        this->values[i] = QVector< QList<GWSAgent*> >( y_size );
    }
}

void GWSAgentGrid::addGridCellValue(unsigned int grid_x, unsigned int grid_y, GWSAgent *v){
    if( grid_x < 0 || grid_x >= this->values.size() || grid_y < 0 || grid_y >= this->values.value(0).size() ){
        return;
    }
    this->mutex.lock();
    this->values[grid_x][grid_y].append( v );
    this->mutex.unlock();
}

void GWSAgentGrid::removeGridCellValue(unsigned int grid_x, unsigned int grid_y, GWSAgent *v){
    if( grid_x < 0 || grid_x >= this->values.size() || grid_y < 0 || grid_y >= this->values[0].size() ){
        return;
    }
    this->mutex.lock();
    this->values[grid_x][grid_y].removeAll( v );
    this->mutex.unlock();
}
