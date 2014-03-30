/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/
#include "gu/InstanceFactory.h"

namespace gu
{

    ImplementSingleton(InstanceFactory);

    InstanceFactory::InstanceFactory()
    {

    }

    InstanceFactory::~InstanceFactory()
    {
        m_abstractUnits.clear();
    }

} // namespace gu
