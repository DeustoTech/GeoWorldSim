#include "TerrainAgent.h"

#include "../../environment/grid_environment/GridEnvironment.h"

TerrainAgent::TerrainAgent() : GWSAgent() , GWSGrid( 0 , 100 , 100 , 0 ){

}


void TerrainAgent::behave(){

    for( int i = 0 ; i < this->getXSize() ; i ++ ){
        for( int j = 0 ; j < this->getYSize() ; j++ ){
            this->setCellValue(i , j , this->getCellValue(i,j) + 0.1 );
        }
    }

    this->incrementInternalTime( 5 );
}
