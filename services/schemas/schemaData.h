#ifndef SCHEMADATA_H
#define SCHEMADATA_H
#include "schemas.h"

#define datetime_format "yyyy/MM/dd hh:mm:ss"

namespace Data {
    class NodeSample {
        const uint id;
        QString standard, material, specification, condPeriod;
        uint diamNom, diamReal, wallthick, lenFree, lenTotal;

    public:
        NodeSample(const NodeSample& me, const uint id_);
        NodeSample(const uint id_, const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal, const QString condPeriod);
        ~NodeSample();

        static QSharedPointer<NodeSample> get(const uint idSample);
        static uint insert(QSharedPointer<NodeSample> sample);
        static uint exists(const QString standard, const QString material, const QString specification, const uint diamNom, const uint diamReal, const uint wallthick, const uint lenFree, const uint lenTotal);

        uint getID();
        QString getStandard();
        QString getMaterial();
        QString getSpecification();
        QString getCondPeriod();
        uint  getDiamNom();
        uint  getDiamReal();
        uint  getWallThick();
        uint  getLenFree();
        uint  getLenTotal();
    };

    class NodeSpecimen {
        const uint id, idSample;
        QString enviroment,
            operatorName,
            testName,
            endCap;
        int targetPressure, targetTemp;

    public:
        NodeSpecimen(const NodeSpecimen& me, const uint id, const uint idSample);
        NodeSpecimen(const uint id, const uint idSample, const int targetPressure, const int targetTemp,  const QString& operatorName, const QString& enviroment, const QString& testName, const QString& endCap);
        ~NodeSpecimen();

        static QSharedPointer<NodeSpecimen> get(const uint idSpecimen);
        static uint insert(QSharedPointer<NodeSpecimen> specimen);
        static uint insert(QSharedPointer<NodeSpecimen> specimen, const uint idSample);
        static uint count(const uint idSample);

        uint getID();
        uint getIDSample();
        int  getTargetPressure();
        int  getTargetTemperature();
        const QString getEnviroment();
        const QString getTestName();
        const QString getEndCap();
        const QString getOperatorName();
    };

    class NodeData {
        const uint idSpecimen;
        double pressure, temperature;

    public:
        NodeData(const uint idData, const uint idSpecimen, const double pressure, const double temperature);
        NodeData(const uint idSpecimen, const double pressure, const double temperature);
        ~NodeData();

        static void insert(NodeData &myData);

        uint getIDSpecimen();
        double getTemperature();
        double getPressure();
    };
}
#endif // SCHEMADATA_H
