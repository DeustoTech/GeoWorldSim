#ifndef FUZZYLOGICTRAINER_H
#define FUZZYLOGICTRAINER_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QVector>
#include <qt5/QtCore/QRegularExpression>
#include <QMutex>

//#include "usecases/gtplat/GTPlatItinerary.h"
#include "utils/geometry/Coordinate.h"

#include "utils/fuzzy_logic/FuzzyLogic.h"

QT_FORWARD_DECLARE_CLASS(FuzzyLogicTrainerOperator)
QT_FORWARD_DECLARE_CLASS(RuleCrossoverOperator)
QT_FORWARD_DECLARE_CLASS(InputCrossoverOperator)
QT_FORWARD_DECLARE_CLASS(InputMutationOperator)

class FuzzyLogicTrainer : public QObject
{
    Q_OBJECT
public:
    explicit FuzzyLogicTrainer( QMap<QString , QStringList> input_values , QStringList outputs , QObject *parent = 0);
    ~FuzzyLogicTrainer();

    // STRUCTS
    struct FuzzyExperiment {
        // Fuzzy Logic data
        QList< FuzzyLogic::InputMemberShipFunction > inputset;
        QList< FuzzyLogic::Rule > ruleset;
        FuzzyLogic::OutputMembershipFunction output;
        // Fuzzy logic parameters
        QString accumulator;
        QString conjunction;
        QString disjunction;
        QString activator;
        inline QString toString() const {
            QString str = QString("[%1,%2,%3,%4] ").arg( this->accumulator ).arg( this->conjunction ).arg( this->disjunction ).arg( this->activator );
            foreach(FuzzyLogic::Rule r , this->ruleset ){
                str += QString("if ");
                foreach(FuzzyLogic::RuleTerm rt , r.terms ){ str += QString("(%1 is %2 %3)").arg( rt.input_name ).arg( rt.hedge ).arg( rt.term_name ); }
                str += QString("then %1 ").arg( r.output_value );
            }
            foreach(FuzzyLogic::InputMemberShipFunction r , this->inputset ){
                str += QString("[%1:").arg(r.input_name);
                foreach(FuzzyLogic::MemberShipFunctionTerm it , r.terms ){ str += QString("(%1 %2 %3)").arg( it.shape_points.at(0) ).arg( it.shape_points.at(1) ).arg( it.shape_points.at(2) ); }
                str += QString("]");
            }
            return str;
        }
        inline void fromString(QString text){
            { // Configuration
                QRegularExpressionMatch match = QRegularExpression("\\[(?<acc>[A-Za-z]+),(?<con>[A-Za-z]+),(?<dis>[A-Za-z]+),(?<act>[A-Za-z]+)\\]").match( text );
                this->accumulator = match.captured("acc").trimmed();
                this->conjunction = match.captured("con").trimmed();
                this->disjunction = match.captured("dis").trimmed();
                this->activator = match.captured("act").trimmed();
            }
            { // Rules
                QRegularExpressionMatch match = QRegularExpression("(?<rulesets>\\ if\\ [A-Za-z_ ()]+then\\ [A-Za-z]+)").match( text );
                foreach(QString rs , match.captured("rulesets").split(" if") ){
                    if( rs.trimmed().isEmpty() ){ continue; }
                    FuzzyLogic::Rule rule;
                    match = QRegularExpression("then\\ (?<then>[A-Za-z]+)").match(rs);
                    rule.output_value = match.captured("then").trimmed();
                    match = QRegularExpression("(?<term>\\([A-Za-z_ ]+is[A-Za-z ]+\\)+)").match(rs);
                    while( match.hasMatch() ){
                        QString t = match.captured("term").trimmed();
                        match = QRegularExpression("\\((?<in>[A-Za-z_ ]+)is(?<he>[a-z ]+)\\ (?<on>[A-Za-z]+)\\)").match(t);
                        FuzzyLogic::RuleTerm ruleterm;
                        ruleterm.input_name = match.captured("in").trimmed();
                        ruleterm.hedge = match.captured("he").trimmed();
                        ruleterm.term_name = match.captured("on").trimmed();
                        rule.terms.append( ruleterm );
                        rs.replace( t , "" );
                        match = QRegularExpression("(?<term>\\([A-Za-z_ ]+is[A-Za-z ]+\\)+)").match(rs);
                    }
                    if( !rule.terms.isEmpty() ){ this->ruleset.append( rule ); }
                }
            }
        }
    };

    // GETTERS
    QStringList getOutputNames() const;
    QMap<QString , QStringList> getInputNames() const;
    double getBestFitness() const;
    FuzzyExperiment getBestExperiment() const;
    int getIteration() const;
    int getSubiteration() const;

    // GENERATORS
    QList< FuzzyLogic::InputMemberShipFunction > generateInputset();
    QList< FuzzyLogic::Rule > generateRuleset();
    FuzzyLogic::Rule generateRule();
    FuzzyLogic::RuleTerm generateRuleTerm();
    FuzzyLogic::OutputMembershipFunction generateOutput();

    // METHODS
    virtual void execute();

    // FITNESS
    virtual double calculateFitness( QList< FuzzyLogic::InputMemberShipFunction > inputset , QList< FuzzyLogic::Rule > ruleset , FuzzyLogic::OutputMembershipFunction output , QString accumulator , QString conjunction , QString disjunction , QString activator ) = 0; // IMPORTANT!! The greater, the better

protected:

    // PROTECTED METHODS THAT CAN BE OVERRIDEN BY CHILDREN
    virtual void init();
    virtual void loop();
    virtual void finish();

    // GENETIC ALGORITHM FUNCTIONS
    void executeRules();
    void executeInputs();
    void executeAccumulators();
    void executeConjunctions();
    void executeDisjunctions();
    void executeActivators();

    QList< FuzzyLogic::Rule > getBestRuleset( QList< QList< FuzzyLogic::Rule > > rules , QVector<double> &return_fitness );
    QList< FuzzyLogic::InputMemberShipFunction > getBestInput( QList< QList< FuzzyLogic::InputMemberShipFunction > > inputs , QVector<double> &return_fitness );

    // GENETIC ALGORITHM OPERATORS FOR RULES
    QList<FuzzyLogicTrainerOperator*> rule_operators;

    // GENETIC ALGORITHM OPERATORS FOR INPUTS
    QList<FuzzyLogicTrainerOperator*> input_operators;

    // GENETIC ALGORITHM POPULATION
    QList< QList< FuzzyLogic::Rule > > rule_population;
    QList< double > rule_polulation_fitness;

    QList< QList< FuzzyLogic::InputMemberShipFunction > > input_population;
    QList< double > input_population_fitness;

    // BEST FUZZY EXPERIMENT FOUND SO FAR
    FuzzyLogicTrainer::FuzzyExperiment best_experiment;
    double best_fitness = 0;

    // CONSTRUCTION DATA FOR GENERATING POPULATION
    QStringList output_names; // CAR, BICYCLE, MOTORCYCLE, TRANSIT...
    QMap<QString , QStringList > input_names; // QMAP < PRICE < QSTRINGLIST( CHEAP, EXPENSIVE, ..) > , TIME < QSTRINGLIST( LOW, MEDIUM, HIGH,... ) > >
    QStringList accumulators;
    QStringList conjunctions;
    QStringList disjunctions;
    QStringList activators;

    // Population size
    int population_size = 10;

    // Iteration step
    int iteration = 0;
    int subiteration = 0;

    // For parallel access
    mutable QMutex mutex;

};

#endif // FUZZYLOGICTRAINER_H
