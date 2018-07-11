#ifndef INPEXPORTER_H
#define INPEXPORTER_H

#include <QObject>
#include <QFile>

struct InpJunction{
    QString node_id;
    double elev;
    double ymax;
    double y0;
    double ysur;
    double apond;

    InpJunction(QString n, double e, double ym, double y, double ys, double a):
        node_id(n), elev(e), ymax(ym), y0(y), ysur(ys), apond(a) {}
};

struct InpConduit{
    QString arc_id;
    QString n1;
    QString n2;
    double length;
    double nvalue;
    double zup;
    double zdown;
    double q0;
    double qmax;

    InpConduit(QString arc, QString node1, QString node2, double l, double nv, double zu, double zd, double qe0, double qm) :
       arc_id(arc), n1(node1), n2(node2), length(l), nvalue(nv), zup(zu), zdown(zd), q0(qe0), qmax(qm) {}
};

struct InpCoordinate{
    QString node_id;
    double x;
    double y;

    InpCoordinate(QString n, double px, double py):
        node_id(n), x(px), y(py) {}
};

struct InpVertex{
    QString arc_id;
    double x;
    double y;

    InpVertex(QString arc, double px, double py) :
        arc_id(arc), x(px), y(py) {}
};


class InpExporter
{
public:
    InpExporter();
    bool process( QString filepath );

    void addConduit(InpConduit conduit);
    void addVertex(InpVertex vertex);
    void addJunction(InpJunction junction);
    void addCoordinate(InpCoordinate coordinate);

private:
    bool processTarget(int id, QString table_name, int lines);
    bool processTarget2(int id, QString table_name, int lines);
    void parseField(QMap<QString, QString> row_data, QString skey, int size);
    QList< QMap<QString, QString> > getTableData(QString table_name);

    QString OPTIONS_TABLE = "v_inp_options";
    QString REPORTS_TABLE = "v_inp_report";
    QString REPORTS_TABLE2 = "inp_report";
    QString TIMES_TABLE = "v_inp_times";
    QString PATTERNS_TABLE = "inp_pattern";
    int DEFAULT_SPACE = 23;
    bool isVersion51;

    QFile input_inp;
    QFile output_inp;

    QList<InpConduit> conduits;
    QList<InpCoordinate> coordinates;
    QList<InpJunction> junctions;
    QList<InpVertex> vertices;
};

#endif // INPEXPORTER_H
