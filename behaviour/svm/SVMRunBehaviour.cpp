#include "SVMRunBehaviour.h"

#include "../../skill/svm/SVMSkill.h"

QString geoworldsim::behaviour::SVMRunBehaviour::INPUT_SVM_MODEL_PATH = "svm_model_path";
QString geoworldsim::behaviour::SVMRunBehaviour::INPUT_SVM_INPUTS_OUTPUTS_PATH = "svm_inputs_outputs_path";
QString geoworldsim::behaviour::SVMRunBehaviour::SVM_INPUTS = "svm_inputs";
QString geoworldsim::behaviour::SVMRunBehaviour::STORE_SVM_OUTPUTS_AS = "store_svm_outputs_as";
QString geoworldsim::behaviour::SVMRunBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::SVMRunBehaviour::SVMRunBehaviour(){

}

void geoworldsim::behaviour::SVMRunBehaviour::afterCreateHook(){

    QSharedPointer<Entity> behaving_entity = this->getEntity();

    // Check if agent has a SVMSkill, otherwise create it
    QSharedPointer< skill::SVMSkill > svm_skill = behaving_entity->getSkill( skill::SVMSkill::staticMetaObject.className() ).dynamicCast< skill::SVMSkill >();
    if( svm_skill.isNull() ){
        svm_skill = QSharedPointer< skill::SVMSkill >( new skill::SVMSkill() );
        behaving_entity->addSkill( svm_skill );
    }

}


QPair< double , QJsonArray > geoworldsim::behaviour::SVMRunBehaviour::behave(){

    QString model_path = this->getProperty( INPUT_SVM_MODEL_PATH ).toString();
    QString inputs_outputs_path = this->getProperty( INPUT_SVM_INPUTS_OUTPUTS_PATH ).toString();

    if( model_path.isEmpty() || inputs_outputs_path.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
    }

    QSharedPointer<Entity> behaving_entity = this->getEntity();
    QSharedPointer< skill::SVMSkill > svm_skill = behaving_entity->getSkill( skill::SVMSkill::staticMetaObject.className() ).dynamicCast< skill::SVMSkill >();

    // Use model_path as unique name for the svm
    QSharedPointer< intelligence::Svm > svm = svm_skill->getSVM( model_path );
    if( svm.isNull() ){
        svm_skill->loadSVM( model_path , model_path , inputs_outputs_path );
        svm = svm_skill->getSVM( model_path );
    }

    // Prepare svm inputs
    QMap<QString, QVariant> svm_inputs;
    QJsonObject json_inputs = this->getProperty( SVM_INPUTS ).toObject();

    foreach( QString k , json_inputs.keys() ) {
        svm_inputs.insert( k , json_inputs.value( k ) );
    }

    // Run svm
    QJsonObject svm_outputs = svm->run( svm_inputs );

    // Store the results back to the entity
    QJsonObject store_outputs_as = this->getProperty( STORE_SVM_OUTPUTS_AS ).toObject();
    foreach( QString svm_output_name , store_outputs_as.keys() ) {
        QString store_name = store_outputs_as.value( svm_output_name ).toString();
        behaving_entity->setProperty( store_name , svm_outputs.value( svm_output_name ) );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
