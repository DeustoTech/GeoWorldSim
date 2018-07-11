#ifndef DXFIMPORTER_H
#define DXFIMPORTER_H

#include <QObject>
#include <QDebug>

#include "dl_creationadapter.h"
#include "utils/geometry/GSSCoordinate.h"

struct DxfLayer{
    QString name;
    int flags;

    DxfLayer(QString n, int f): name(n), flags(f) {}
};

struct DxfPoint {
    double x;
    double y;
    double z;

    DxfPoint(double px, double py, double pz): x(px), y(py), z(pz) {}
};

struct DxfLine {
    DxfPoint start_point;
    DxfPoint end_point;

    DxfLine(DxfPoint sp, DxfPoint ep) : start_point(sp), end_point(ep) {}
};

struct DxfText{
    DxfPoint point;
    QString text;

    DxfText(DxfPoint p, QString t ) : point(p), text(t) {}
};

struct DxfCircle{
    DxfPoint point;
    double radius;

    DxfCircle( DxfPoint p, double r ) : point(p), radius(r) {}
};

struct DxfArc{
    DxfPoint point;
    double radius;
    double angle1;
    double angle2;

    DxfArc( DxfPoint p, double r, double a1, double a2) :
            point(p), radius(r), angle1(a1), angle2(a2) {}
};

struct DxfVertex{
    DxfPoint point;
    double bulge;

    DxfVertex( DxfPoint p, double b) : point(p), bulge(b) {}
};


/**
 * This class takes care of the entities read from the file.
 * Usually such a class would probably store the entities.
 * this one just prints some information about them to stdout.
 *
 */
class DxfImporter: public DL_CreationAdapter
{
public:
    DxfImporter();
    ~DxfImporter();

    virtual void addLayer(const DL_LayerData& data);
    virtual void addPoint(const DL_PointData& data);
    virtual void addLine(const DL_LineData& data);
    virtual void addArc(const DL_ArcData& data);
    virtual void addCircle(const DL_CircleData& data);
    virtual void addPolyline(const DL_PolylineData& data);
    virtual void addVertex(const DL_VertexData& data);
    virtual void addText(const DL_TextData&);
    virtual void addAttribute(const DL_AttributeData&);

    virtual void closeFile();

    // GETTERS
    QList<DxfLayer> getLayers();
    QList<DxfArc> getArcs();
    QList<DxfCircle> getCircles();
    QList< DxfLine > getLines();
    QList<DxfText> getTexts();
    QList<DxfPoint> getPoints();
    QList< QList<DxfVertex> > getPolylines();

    // METHODS
    QString getTextForCoordinates( const GSSCoordinate coord );


private:
    QList<DxfVertex> current_polyline;

    QList<DxfLayer> layers;
    QList<DxfArc> arcs;
    QList<DxfCircle> circles;
    QList<DxfLine> lines;
    QList<DxfText> texts;
    QList< QList<DxfVertex> > polylines;
    QList<DxfPoint> points;
};

#endif // DXFIMPORTER_H
