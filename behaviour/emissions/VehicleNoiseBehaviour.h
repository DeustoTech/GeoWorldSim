#ifndef VEHICLENOISEBEHAVIOUR_H
#define VEHICLENOISEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

/** FROM
* https://www.mp.nl/sites/all/files/publicaties/IMA55TR-060821-MP10%20-%20IMAGINE%20Deliverable%20D11.pdf
**/

class VehicleNoiseBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit VehicleNoiseBehaviour();

    // PROPERTIES
    static QString INPUT_VEHICLE_TYPE;
    static QString OUTPUT_NOISE;
    static QString NEXTS;

private slots:
    virtual QPair< double , QJsonArray > behave();

};

#endif // VEHICLENOISEBEHAVIOUR_H
