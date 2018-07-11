#ifndef FUZZYLOGIC_H
#define FUZZYLOGIC_H

#include "utils/object/Object.h"
#include <QVector>

// Fuzzylite library
// https://github.com/fuzzylite/fuzzylite
#include "fl/Engine.h"

#include "fl/variable/InputVariable.h"
#include "fl/variable/OutputVariable.h"
#include "fl/rule/Rule.h"
#include "fl/rule/RuleBlock.h"

#include "fl/term/Triangle.h"
#include "fl/term/Rectangle.h"
#include "fl/term/Ramp.h"
#include "fl/term/Gaussian.h"

using namespace fl;

class FuzzyLogic : public GWSObject
{
    Q_OBJECT
public:

    FuzzyLogic( QObject* parent = 0 );
    ~FuzzyLogic();

    // STRUCTS
        // RULES
    struct RuleTerm {
        QString input_name; // Must match an InputMemberShipFunction input_value . For example CAR_PRICE / WALK_DISTANCE / ...
        QString hedge; // Optional hedge: any, not, extremely, seldom, somewhat, very
        QString term_name; // Must match an InputMemberShipFunction term.value_name . For example VERY_CHEAP / CHEAP / ...
        inline bool operator==(const RuleTerm& r) const {
            return input_name == r.input_name && hedge == r.hedge && term_name == r.term_name;
        }
    };
    struct Rule {
        QList<RuleTerm> terms;
        QString output_value; // Must match an OutputMembershipFunction output_name . For example CAR / WALK / ...
        inline bool operator==(const Rule& r) const {
            bool match = output_value == r.output_value && terms.size() == r.terms.size();
            for( int i = 0 ; match && i < terms.size() ; i++ ){ match &= terms.at(i) == r.terms.at(i); }
            return match;
        }
    };

        // FOR INPUTS OR OUTPUTS
    struct MemberShipFunctionTerm {
        QString name;
        QVector<double> shape_points;
        double shape_height = 1;
        inline bool operator==(const MemberShipFunctionTerm& r) const {
            return name == r.name && shape_points == r.shape_points && shape_height == r.shape_height;
        }
    };
    struct InputMemberShipFunction {
        QString input_name;
        QList<MemberShipFunctionTerm> terms;
        inline bool operator==(const InputMemberShipFunction& r) const {
            bool match = input_name == r.input_name && terms.size() == r.terms.size();
            for( int i = 0 ; match && i < terms.size() ; i++ ){ match &= terms.at(i) == r.terms.at(i); }
            return match;
        }
    };

        // OUTPUTS
    struct OutputMembershipFunction {
        QString output_name;
        QList<MemberShipFunctionTerm> terms;
    };

    // GETTERS
    double getOutputValue( QString variable_name );
    QString getOutputValueAsName( QString variable_name );
    QString getOutputValueWeights( QString variable_name );
    double getInputValue( QString variable_name );
    const fl::Engine* getEngine();
    bool hasInputVariable( QString variable_name );
    bool isReady();

    // SETTERS
    void setInputValue( QString input_name , double value );

    // METHIDS
    void setParameters( QList<FuzzyLogic::InputMemberShipFunction> inputs , QList<FuzzyLogic::Rule> rules , FuzzyLogic::OutputMembershipFunction output );
    void configure(QString conjunctionT, QString disjunctionS, QString activationT, QString accumulationS, QString defuzzifier, int resolution = IntegralDefuzzifier::defaultResolution() );
    void resetInputValues();
    void process();

private:

    QStringList output_variable_names;

    fl::Engine* fuzzy_logic_engine;

    // METHODS
    void addTermsToVariable( fl::Variable* var , QList<FuzzyLogic::MemberShipFunctionTerm> terms );

};

#endif // FUZZYLOGIC_H
