/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#include "vsgePCH.h"
#include "vsge/base/InstanceFactory.h"

using namespace vsge;

ImplementSingleton(InstanceFactory);

InstanceFactory::InstanceFactory()
{

}

InstanceFactory::~InstanceFactory()
{
    m_abstractUnits.clear();
}


