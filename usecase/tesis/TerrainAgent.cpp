#include "TerrainAgent.h"

#include "../../util/grid/Grid.h"

TerrainAgent::TerrainAgent() : GWSAgent() {
    //this->setProperty( "grid" , GWSGrid() );
}

void TerrainAgent::behave(){

    /*for( int i = 0 ; i < this->getXSize() ; i ++ ){
        for( int j = 0 ; j < this->getYSize() ; j++ ){
            this->setCellValue(i , j , this->getCellValue(i,j) + 0.1 );
        }
    }*/

    this->incrementInternalTime( 5 );
}
