// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2010 RobotCub Consortium
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/


/*! \file AnalogSensorClient.cpp implementation of an analog sensor client class*/

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IAnalogSensor.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include <iostream>

namespace yarp {
    namespace dev { 
        class AnalogSensorClient;
    }
}

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

const int ANALOG_TIMEOUT=100; //ms

class InputPortProcessor:public BufferedPort<yarp::sig::Vector>
{
    yarp::sig::Vector lastVector;
    Semaphore mutex;
    Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    int state;
    int count;

public:

    inline void resetStat()
    {
        mutex.wait();
        count=0;
        deltaT=0;
        deltaTMax=0;
        deltaTMin=1e22;
        now=Time::now();
        prev=now;
        mutex.post();
    }

    InputPortProcessor()
    {
        state=IAnalogSensor::AS_ERROR;
        resetStat();
    }

    virtual void onRead(yarp::sig::Vector &v)
    {
        now=Time::now();
        mutex.wait();

        if (count>0)
        {
            double tmpDT=now-prev;
            deltaT+=tmpDT;
            if (tmpDT>deltaTMax)
                deltaTMax=tmpDT;
            if (tmpDT<deltaTMin)
                deltaTMin=tmpDT;

            //compare network time
            if (tmpDT*1000<ANALOG_TIMEOUT)
            {
                state=IAnalogSensor::AS_OK;
            }
            else
            {
                state=IAnalogSensor::AS_TIMEOUT;
            }
        }

        prev=now;
        count++;

        lastVector=v;
        Stamp newStamp;
        getEnvelope(newStamp);

        //now compare timestamps
        if ((1000*(newStamp.getTime()-lastStamp.getTime()))<ANALOG_TIMEOUT)
        {
            state=IAnalogSensor::AS_OK;
        }
        else
        {
            state=IAnalogSensor::AS_TIMEOUT;
        }
            
        mutex.post();
    }

    inline int getLast(yarp::sig::Vector &data, Stamp &stmp)
    {
        mutex.wait();
        int ret=state;
        if (ret!=IAnalogSensor::AS_ERROR)
        {
            data=lastVector;
            stmp = lastStamp;
        }
        mutex.post();

        return ret;
    }

    inline int getIterations()
    {
        mutex.wait();
        int ret=count;
        mutex.post();
        return ret;
    }

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max)
    {
        mutex.wait();
        ite=count;
        min=deltaTMin*1000;
        max=deltaTMax*1000;
        if (count<1)
        {
            av=0;
        }
        else
        {
            av=deltaT/count;
        }
        av=av*1000;
        mutex.post();
    }

    int getState()
    { return state; }

    int getChannels()
    {
        if (state==IAnalogSensor::AS_ERROR)
        {
            return 0;
        }
        else
            return lastVector.size();
    }

};



/**
* @ingroup dev_impl_wrapper
*
* The client side of any IAnalogSensor capable device.
* Still single thread! concurrent access is unsafe.
*/
class yarp::dev::AnalogSensorClient: public DeviceDriver,
                          public IPreciselyTimed,
                          public IAnalogSensor
{
protected:
    InputPortProcessor inputPort;
    yarp::os::impl::String local;
    yarp::os::impl::String remote;
    Stamp lastTs; //used by IPreciselyTimed
public:

    /* DevideDriver methods */
    bool open(Searchable& config);
    bool close();

    /* IAnalogSensor methods*/    
    int read(yarp::sig::Vector &out);
   
    /* Check the state value of a given channel.
    * @param ch: channel number.
    * @return status.
    */
    int getState(int ch);
    
    /* Get the number of channels of the sensor.
     * @return number of channels (0 in case of errors).
     */
    int getChannels();

    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    Stamp getLastInputStamp();
};

bool yarp::dev::AnalogSensorClient::open(yarp::os::Searchable &config)
{
    remote = config.find("remote").asString().c_str();
    local = config.find("local").asString().c_str();

    ConstString carrier = config.check("carrier",
        Value("udp"),
        "default carrier for streaming robot state").asString().c_str();

    if (local=="")
    {
        std::cerr<<"AnalogSensorClient::open() error you have to provide valid local name"<<std::endl;
        return false;
    }
    if (remote=="")
    {
        std::cerr<<"AnalogSensorClient::open() error you have to provide valid remote name"<<std::endl;
        return false;
    }

    if (!inputPort.open(local.c_str()))
    {
        std::cerr<<"AnalogSensorClient::open() error could not open port, check network"<<std::endl;
        return false;
    }
    inputPort.useCallback();
    
    bool ok=Network::connect(remote.c_str(), local.c_str(), carrier.c_str());
    
    if (!ok)
    {
        std::cerr<<"AnalogSensorClient::open() error could not connect to "<< remote.c_str() << std::endl;
        return false;
    }

    return true;
}

bool yarp::dev::AnalogSensorClient::close()
{
    inputPort.close();
    return true;
}

int yarp::dev::AnalogSensorClient::read(yarp::sig::Vector &out)
{
    return inputPort.getLast(out, lastTs);
}

int yarp::dev::AnalogSensorClient::getState(int ch)
{
    //not implemented yet
    return AS_OK;
}

int yarp::dev::AnalogSensorClient::getChannels()
{
    return inputPort.getChannels();
}

Stamp yarp::dev::AnalogSensorClient::getLastInputStamp()
{
    return lastTs;
}

yarp::dev::DriverCreator *createAnalogSensorClient() {
    return new DriverCreatorOf<AnalogSensorClient>("analogsensorclient", 
        "",
        "AnalogSensorClient");
}