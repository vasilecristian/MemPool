/***************************************************************************/
/*File created on 18.05.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/***************************************************************************/

#include <map>
#include <string>
#include <sstream>


#include "Params.h"
#include "Log.h"


namespace gu 
{
    ParanContent::ParanContent(const std::string& name, const std::string& type)
    {
        m_name = name;
        m_type = type;
        m_value = GetValueAsSmartRTTI(type);
    }

    ParanContent::ParanContent(const std::string& name, const std::string& type, smart_ptr<RTTI> value)
    {
        m_name = name;
        m_type = type;
        m_value = value;
    }



    ParanContent::~ParanContent()
    {
    
    }

    smart_ptr<RTTI> ParanContent::GetValueAsSmartRTTI(const std::string& type)
    {
    
        return smart_ptr<RTTI>(NULL);
    }


    bool Params::LoadParams(const std::string& fileName)
    {
   

        return true;
    }

    bool Params::LoadParams()
    {     
   
   
        return true;
    }

    smart_ptr<ParanContent> Params::GetFirstParamOfType(const std::string& type)
    {
        auto it= m_map.begin();

        for( ; it != m_map.end(); ++it)
        {
            smart_ptr<ParanContent> prm = (*it).second;
            if(prm->GetType().compare(type)== 0)
                return prm;
        }

        std::stringstream ss;
        ss << "Failed to find any parameter of type='" << type << "'";
        LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__, ss);

        return NULL;
    }

    std::vector< smart_ptr<ParanContent> > Params::GetAllParamsOfType(const std::string& type)
    {
        std::vector< smart_ptr<ParanContent> > returnVector;

        auto it= m_map.begin();

        for( ; it != m_map.end(); ++it)
        {
            smart_ptr<ParanContent> prm = (*it).second;
            if(prm->GetType().compare(type)== 0)
                returnVector.push_back(prm);
        }

        return returnVector;
    }
} //namespace gu