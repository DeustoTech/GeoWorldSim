#ifndef RECYCLINGPLANTAGENT_H
#define RECYCLINGPLANTAGENT_H

#include "../../agent/Agent.h"

class RecyclingPlantAgent : public GWSAgent
{
    Q_OBJECT

public:

    // Constructor
    Q_INVOKABLE RecyclingPlantAgent();
    // METHODS
    ~RecyclingPlantAgent();
};

Q_DECLARE_METATYPE(RecyclingPlantAgent*) // REQUIRED IN EVERY CHILD

#endif // RECYCLINGPLANTAGENT_H
