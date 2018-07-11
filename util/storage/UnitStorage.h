#ifndef GWSUNITSTORAGE_H
#define GWSUNITSTORAGE_H

#include <QMutex>

#include "utils/object/Object.h"
#include "utils/units/Units.h"

class GWSUnitStorage : public GWSObject
{
    Q_OBJECT

public:
    explicit GWSUnitStorage(GWSObject *parent = 0);

    // GETTERS
    GWSUnit getAmount() const;
    GWSUnit getCapacity() const;

    // SETTERS
    void setCapacity( GWSUnit capacity );
    virtual void add( GWSUnit amount );
    virtual GWSUnit remove( GWSUnit amount );
    virtual GWSUnit empty();

    // PROPERTIES
    static const char* STORED_PROP;
    static const char* CAPACITY;

protected:

    QMutex mutex;
};

#endif // GWSUNITSTORAGE_H
