#include "Svm.h"

#include <QDebug>

GWSSvm::GWSSvm() : GWSIntelligence (){

    // set some defaults
    this->parameters.svm_type = C_SVC;
    this->parameters.kernel_type = LINEAR;
    this->parameters.degree = 3;
    this->parameters.gamma = 0;	// 1/num_features
    this->parameters.coef0 = 0;
    this->parameters.nu = 0.5;
    this->parameters.cache_size = 100;
    this->parameters.C = 1;
    this->parameters.eps = 1e-3;
    this->parameters.p = 0.1;
    this->parameters.shrinking = 1;
    this->parameters.probability = 0;
    this->parameters.nr_weight = 0;
    this->parameters.weight_label = 0;
    this->parameters.weight = 0;

}

GWSSvm::~GWSSvm(){
    if(model != Q_NULLPTR){ svm_free_and_destroy_model(&model); }
    svm_destroy_param(&this->parameters);
}


/**********************************************************************
  METHODS
**********************************************************************/

/* NOTE [from https://www.csie.ntu.edu.tw/~cjlin/libsvm/faq.html#/Q03:_Data_preparation]:
 * Currently libsvm supports only numerical data. You may have to change non-numerical data to numerical.
 * For example, you can use several binary attributes to represent a categorical attribute.
 */


/* Train SVM on given files */

void GWSSvm::train(QList<QList<QPair< std::string, QVariant> > > input_train_dataset, QList<QList<QPair< std::string, QVariant> > > output_train_dataset){

    Q_ASSERT( input_train_dataset.size() == output_train_dataset.size() );

    svm_problem problem;

    // Set problem size
    problem.l = input_train_dataset.size();

    // Set outputs
    problem.y = new double[ problem.l ];
    for( int i = 0 ; i < output_train_dataset.size() ; i++ ) {

        //! WARNING, SVM ONLY ALLOWS SINGLE OUTPUT, WE ARE GOING TO GET ONLY THE FIRST OUTPUT

        QPair< std::string , QVariant > pair = output_train_dataset.at( i ).at( 0 );
        std::string hash = pair.first;
        QVariant value_variant = pair.second;
        hash = this->getIOName( hash , value_variant );

        // Outputs have no position, since only one output is allowed
        problem.y[ i ] = this->normalizeIO( value_variant , hash , this->output_maximums , this->output_minimums );

    }

    // Set inputs
    problem.x = new svm_node*[ problem.l ];
    for( int i = 0 ; i < input_train_dataset.size() ; i++ ){

        problem.x[i] = new svm_node[ input_train_dataset.at( i ).size() + 1 ];

        QList< QPair< std::string , QVariant> > row = input_train_dataset.at( i );
        for( int j = 0 ; j < row.size() ; j++ ) {

            QPair< std::string , QVariant > pair = row.at( j );
            std::string hash = pair.first;
            QVariant value_variant = pair.second;
            hash = this->getIOName( hash , value_variant );

            double value_double = this->normalizeIO( value_variant , hash , this->input_maximums , this->input_minimums );

            int position = this->input_positions->value( hash , -1 );
            if( position > -1 ){
                problem.x[i][position].index = position;
                problem.x[i][position].value = value_double;
            }
        }

        problem.x[i][ input_train_dataset.at( i ).size() ].index = -1;
    }

    this->model = svm_train(&problem, &this->parameters);
}

QJsonObject GWSSvm::run( QList<QPair < std::string, QVariant> > inputs ){
    return QJsonObject();
}
