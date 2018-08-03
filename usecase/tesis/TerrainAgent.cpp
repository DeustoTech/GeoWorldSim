#include "TerrainAgent.h"
#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

TerrainAgent::TerrainAgent( QObject *parent ) : GWSAgent( parent ) , GWSAgentGrid( this ) {
qInfo() << "PLAYGROUND";
    //QVariant grid_variant = QVariant::fromValue<GWSGrid>( GWSGrid() );
    //this->setProperty( "grid" , grid_variant );
}


void TerrainAgent::deserialize(QJsonObject json){
    GWSAgent::deserialize( json );
    GWSAgentGrid::deserialize( json );
}

void TerrainAgent::behave(){

    /*for( int i = 0 ; i < this->getXSize() ; i ++ ){
        for( int j = 0 ; j < this->getYSize() ; j++ ){
            this->setCellValue(i , j , this->getCellValue(i,j) + 0.1 );
        }
    }*/

    //this->incrementInternalTime( 5 );
}



/* QUESTION:

   Eventually should not we add the 'greenness' of each cell grid as a property to
   enable or prevent sheep from feeding? This would undoubtedly control their life-span.

   Otherwise one could add a GrassAgent that has the greenness as a parameter to each of
   the grid cell elements, and the sheep would be able to access that property and modify it.
   This seems similar to the behaviour of trash containers, with whom humans can interact*/
