#include "UnitStorage.h"

const char* GWSUnitStorage::STORED_PROP = "stored";
const char* GWSUnitStorage::CAPACITY = "capacity";

GWSUnitStorage::GWSUnitStorage(GWSObject *parent) : GWSObject( parent ){
    this->setProperty( GWSUnitStorage::STORED_PROP , GWSMassUnit( 0 ) );
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSUnit GWSUnitStorage::getAmount() const{
    return this->property( GWSUnitStorage::STORED_PROP ).value<GWSUnit>();
}

GWSUnit GWSUnitStorage::getCapacity() const{
    return this->property( GWSUnitStorage::CAPACITY ).value<GWSUnit>();
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSUnitStorage::setCapacity(GWSUnit capacity){
    this->setProperty( GWSUnitStorage::CAPACITY , capacity );
}

void GWSUnitStorage::add(GWSUnit amount){
    this->mutex.lock();
    this->setProperty( GWSUnitStorage::STORED_PROP , this->getAmount() + amount );
    this->mutex.unlock();
}

GWSUnit GWSUnitStorage::remove(GWSUnit amount){
    this->mutex.lock();
    GWSUnit stored = this->getAmount();
    this->setProperty( GWSUnitStorage::STORED_PROP , stored - amount );
    this->mutex.unlock();
    return amount;
}

GWSUnit GWSUnitStorage::empty(){
    this->mutex.lock();
    GWSUnit stored = this->getAmount();
    this->setProperty( GWSUnitStorage::STORED_PROP , GWSUnit( 0 , stored.unit ) );
    this->mutex.unlock();
    return stored;
}
