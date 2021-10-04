#ifndef _DATARECORD_

#define _DATARECORD_

#include <string>
#include <vector>

class DataRecord
{
    public:
        std::string dateTime;
        float timeStamp;
        float xAccel;
        float yAccel;
        float zAccel;
        float xGyro;
        float yGyro;
        float zGyro;
        float strain;
        float analog0;
        float analog1;
        float current;
        float encDisp;
        float encCont;
        float encComm;
        float vb;
        float vg;
        float temp;
        float status1;
        float status2;

        // Dummy constructor for use in dynamic array construction
        DataRecord() {}

        DataRecord(std::vector<std::string> data)
        {
            dateTime = data[0];
            timeStamp = stof(data[1]);
            xAccel = stof(data[2]);
            yAccel = stof(data[3]);
            zAccel = stof(data[4]);
            xGyro = stof(data[5]);
            yGyro = stof(data[6]);
            zGyro = stof(data[7]);
            strain = stof(data[8]);
            analog0 = stof(data[9]);
            analog1 = stof(data[10]);
            current = stof(data[11]);
            encDisp = stof(data[12]);
            encCont = stof(data[13]);
            encComm = stof(data[14]);
            vb = stof(data[15]);
            vg = stof(data[16]);
            temp = stof(data[17]);
            status1 = stof(data[18]);
            status2 = stof(data[19]);
        }
};

#endif
