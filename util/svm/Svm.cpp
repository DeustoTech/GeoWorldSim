#include "Svm.h"

#include <QDebug>
#include <QHash>
#include <QStringList>

geoworldsim::intelligence::Svm::Svm() : Intelligence (){

    // set some defaults
    // NU_SVC fits to integer categories
    // NU_SVR tries to fit to double precision
    this->parameters.svm_type = EPSILON_SVR;

    this->parameters.kernel_type = RBF;
    this->parameters.degree = 3;
    this->parameters.gamma = 0.5; // 1/num_features
    this->parameters.coef0 = 0;
    this->parameters.nu = 0.5;
    this->parameters.cache_size = 100;
    this->parameters.C = 1;
    this->parameters.eps = 1e-8;
    this->parameters.p = 0.1;    // epsilon in epsilon SVR
    this->parameters.shrinking = 1;
    this->parameters.probability = 0;
    this->parameters.nr_weight = 0;
    this->parameters.weight_label = Q_NULLPTR;
    this->parameters.weight = Q_NULLPTR;

    svm_set_print_string_function( [](const char* msg){
        qDebug() << msg;
    });

}

geoworldsim::intelligence::Svm::~Svm(){
    if( this->model != Q_NULLPTR ){ svm_free_and_destroy_model( &this->model ); }
    svm_destroy_param( &this->parameters );
}


/**********************************************************************
  METHODS
**********************************************************************/

/* NOTE [from https://www.csie.ntu.edu.tw/~cjlin/libsvm/faq.html#/Q03:_Data_preparation]:
 * Currently libsvm supports only numerical data. You may have to change non-numerical data to numerical.
 * For example, you can use several binary attributes to represent a categorical attribute.
 */


/* Train SVM on given files */

void geoworldsim::intelligence::Svm::train( const QList< QMap< QString, QVariant> > &input_train_dataset , const QList< QMap<QString, QVariant> > &output_train_dataset){

    Q_ASSERT( input_train_dataset.size() == output_train_dataset.size() );

    svm_problem problem;

    // Set problem size

    problem.l = input_train_dataset.size();

    // Set outputs

    problem.y = new double[ problem.l ];
    for( int i = 0 ; i < output_train_dataset.size() ; i++ ) {

        //! WARNING, SVM ONLY ALLOWS SINGLE OUTPUT, WE ARE GOING TO GET ONLY THE FIRST OUTPUT

        QMap< QString , QVariant > map = output_train_dataset.at( i );
        QString hash = map.keys().at( 0 );
        QVariant value_variant = map.values().at( 0 );
        hash = this->getIOName( hash , value_variant );

        // Outputs have no position, since only one output is allowed
        problem.y[ i ] = this->normalizeIOMeanStdev( value_variant , hash , this->output_means , this->output_stdevs );
    }

    // Set inputs

    problem.x = new svm_node*[ problem.l ];
    for( int i = 0 ; i < input_train_dataset.size() ; i++ ){

        const QMap< QString , QVariant> row = input_train_dataset.at( i );
        problem.x[i] = new svm_node[ row.size() + 1 ];

        for(int j = 0 ; j < row.keys().size() ; j++ ){

            QString hash = row.keys().at( j );
            QVariant value_variant = row.value( hash );
            hash = this->getIOName( hash , value_variant );

            double value_double = this->normalizeIOMeanStdev( value_variant , hash , this->input_means , this->input_stdevs );

            int position = this->input_positions.value( hash , -1 );
            if( position > -1 ){
                problem.x[i][j].index = position;
                problem.x[i][j].value = value_double;
            } else {
               // qWarning() << QString("Input %1 not contained in training data").arg( hash );
            }
        }

        problem.x[i][ row.size() ].index = -1;
    }

    this->parameters.gamma = 1.0 / this->input_positions.keys().size(); // 1/num_features!!!

    this->model = svm_train(&problem, &this->parameters);
}

void geoworldsim::intelligence::Svm::saveModel( QString model_file_path ){
    if( this->model ){
        svm_save_model( model_file_path.toUtf8() , this->model );
    }
}

void geoworldsim::intelligence::Svm::loadModel( QString model_file_path ){
    if ( !model_file_path.isNull() ){
        this->model = svm_load_model( model_file_path.toUtf8() );
    }
}

QJsonObject geoworldsim::intelligence::Svm::run( const QMap<QString, QVariant> &inputs){

    if( !this->model ){
        return QJsonObject();
    }

    svm_node* x = new svm_node[ inputs.size() + 1 ];

    QJsonObject result;

    // Loop over input pairs:
    for(int i = 0 ; i < inputs.keys().size() ; i++ ){

        QString hash = inputs.keys().at( i );
        QVariant value_variant = inputs.value( hash );
        hash = this->getIOName( hash , value_variant );

        double value_double = this->normalizeIOMeanStdev( value_variant , hash , this->input_means , this->input_stdevs );

        int position = this->input_positions.value( hash , -1 );
        if( position > -1 ){
            x[ i ].index = position;
            x[ i ].value = value_double;
        } else {
            //qWarning() << QString("Input %1 not contained in training data").arg( hash );
            result.insert( this->output_positions.keys().at(0) , 0 );
            return result;
        }
    }

    // Last element needs to be -1
    x[ inputs.size() ].index = -1;

    // Predict SVM result on test input:
    double normResult = svm_predict( this->model , x );
    delete[] x;

    double denormResult =  this->denormalizeIOMeanStdev( normResult , 0 );

    result.insert( this->output_positions.keys().at(0) , denormResult );
    return result;
}
