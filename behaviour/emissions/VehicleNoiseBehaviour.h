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
    static QString VEHICLE_TYPE;
    static QString STORE_NOISE_AS;
    static QString NEXTS;

private slots:
    virtual QJsonArray behave();

};

#endif // VEHICLENOISEBEHAVIOUR_H
