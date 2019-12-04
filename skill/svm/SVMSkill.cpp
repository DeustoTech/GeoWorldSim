#include "SVMSkill.h"

geoworldsim::skill::SVMSkill::SVMSkill() : Skill( ){

}

geoworldsim::skill::SVMSkill::~SVMSkill(){
    if( this->svms->getAmount() > 0 ){
        this->svms->deleteAllObjects();
    }
}


void geoworldsim::skill::SVMSkill::loadSVM( QString name , QString model_file_path, QString intputs_outputs_file_path ){

    QSharedPointer< intelligence::Svm > svm = QSharedPointer< intelligence::Svm >( new intelligence::Svm() );
    svm->setObjectName( name );
    svm->loadTrained( model_file_path , intputs_outputs_file_path );

    this->svms->addObject( svm );
}

QSharedPointer< geoworldsim::intelligence::Svm > geoworldsim::skill::SVMSkill::getSVM( QString name ) const{
    return this->svms->getByName< intelligence::Svm >( name );
}
