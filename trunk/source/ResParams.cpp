/***************************************************************************/
/*File created on 18.05.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/***************************************************************************/
#include "vsgePCH.h"

#include "ResParams.h"
#include "MemLeakDetector.h"
#include <tinyxml.h>
#include "vsge/base/Types.h"

#include "vsge/fs/fs.h"

#include "vsge/scene/Material.h"
#include "vsge/scene/Texture.h"
#include "vsge/scene/Object3D.h"
#include "vsge/scene/Mesh.h"
#include "vsge/scene/Shader.h"
#include "vsge/LevelBase.h"
#include "vsge/base/ObjTypesMgr.h"
#include "vsge/scene/Scene.h"

#include <map>
#include <string>




using namespace vsge;

RPContent::RPContent(const std::string& name, const std::string& type, TiXmlElement* xmlNode)
{
    m_name = name;
    m_type = type;
    m_value = GetValueAsSmartRTTI(type, xmlNode);
}

RPContent::RPContent(const std::string& name, const std::string& type, SmartRTTI value)
{
    m_name = name;
    m_type = type;
    m_value = value;
}



RPContent::~RPContent()
{
    
}

SmartRTTI RPContent::GetValueAsSmartRTTI(const std::string& type, TiXmlElement* xmlNode)
{
    /** 
     * s_XML_type_int
     *
     * Example: <param name="any name" 	type="int"  value="10" />
     */
    if(type.compare(s_XML_type_int) == 0)
    {
        int val = 0;
        const char* value = xmlNode->Attribute(s_XML_value, &val);
        if(value == NULL)
        {
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__, " int value=NULL\n" );
            val = 0;
        }   
        IntRTTI* valRTTI = NEW IntRTTI(val);
                
        return SmartRTTI(valRTTI);
    }


    /** 
     * s_XML_type_double
     *
     * Example: <param name="any name" 	type="double"  value="10" />
     */
    else if(type.compare(s_XML_type_double) == 0)
    {
        double* val = NEW double;
        const char* value = xmlNode->Attribute(s_XML_value, val);
        if(value == NULL)
        {
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__," double value=NULL\n" );
            return SmartRTTI(NULL);
        }   
        DoubleRTTI* valRTTI = NEW DoubleRTTI(*val);
        delete val;

        return SmartRTTI(valRTTI);
    }


    /** 
     * s_XML_type_string
     *
     * Example: <param name="any name" 	type="string"  value="this is the content" />
     */
    else if(type.compare(s_XML_type_string) == 0)
    {
        const char* value = xmlNode->Attribute(s_XML_value);
        if(value == NULL)
        {
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__," string value=NULL\n" );
            return SmartRTTI(NULL);
        }   
        StringRTTI* valRTTI = NEW StringRTTI((char*)value);

        return SmartRTTI(valRTTI);
    }


    /** 
     * s_XML_type_Object3D
     *
     * Example: <param name="object name" type="Object3D" meshFile="SkyBox.obj" materialFile="SkyBox.material"/>
     */
    else if(type.compare(s_XML_type_Object3D) == 0)
    {
        const char* file = xmlNode->Attribute(s_XML_meshFile);
        if(file == NULL)
        {
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__, " meshFile file=NULL\n" );
            return SmartRTTI(NULL);
        }   

        SmartP<Mesh> mesh = MeshMgr::GetInstance()->GetOrLoadFromFile(file, file);

       
        file = xmlNode->Attribute(s_XML_materialFile);
        if(file == NULL)
        {
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__, " file=NULL\n" );
            return SmartRTTI(NULL);
        }   

        SmartP<Material> material = MaterialsMgr::GetInstance()->GetOrLoadFromFile(file, file);
        if(!material.IsValid())
            return SmartRTTI(NULL);

        Object3D* obj = NEW Object3D(mesh, material);
        
        return SmartRTTI(obj);
    }


    /** 
     * s_XML_type_Shader
     *
     * Example: <param name="Shader name" type="Shader"  VertexShaderFile="texture_vert.c" FragmentShaderFile="texture_frag.c"/> 
     */
    else if(type.compare(s_XML_type_Shader) == 0)
    {
        SmartP<Shader> sp;

        const char* vFile = xmlNode->Attribute(s_XML_VertexShaderFile);
        const char* fFile = xmlNode->Attribute(s_XML_FragmentShaderFile);
        if(vFile == NULL || fFile == NULL)
        {
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__, "vFile=NULL || fFile==NULL\n" );
            return SmartRTTI(NULL);
        }   

        std::string vert_fullPathToFile = FS::GetInstance()->GetFullPathToFile(vFile);
        std::string frag_fullPathToFile = FS::GetInstance()->GetFullPathToFile(fFile);


        Shader* shdr = NEW Shader(vert_fullPathToFile, frag_fullPathToFile);

        sp = SmartP<Shader>(shdr);

        return sp.DynamicCastToSmartRTTI();
    }


    /** 
     * s_XML_type_attribute
     *
     * Example: <param name="a_pos" type="attribute"  dataSize="3" dataType="GL_FLOAT" dataNormalized="false" dataOffset="0"/>
     */
    else if(type.compare(s_XML_type_attribute) == 0)
    {
        int dataSize = 1;
        const char* retVal = xmlNode->Attribute(s_XML_dataSize, &dataSize);
        VSGE_ASSERT(retVal != nullptr);

        int dataOffset = 0;
        retVal = xmlNode->Attribute(s_XML_dataOffset, &dataOffset);
        VSGE_ASSERT(retVal != nullptr);

        const char* dataType = xmlNode->Attribute(s_XML_dataType);
        VSGE_ASSERT(dataType != nullptr);
        
        const char* dataNormalized = xmlNode->Attribute(s_XML_dataNormalized);
        VSGE_ASSERT(dataNormalized != nullptr);

        ///create the attribute
        ShaderAttrib* attrib = NEW ShaderAttrib(m_name.c_str());

        attrib->SetSize(dataSize);
        attrib->SetOffset(dataOffset);

        #define CHECK_TYPE_OF_DATA(tofd) if(strcmp(dataType, #tofd) == 0) attrib->SetTypeOfData(tofd);
        #define CHECK_NORMALIZED(value) if(strcmp(dataNormalized, #value) == 0) attrib->SetNormalized(value);

        CHECK_TYPE_OF_DATA(GL_BYTE);
        CHECK_TYPE_OF_DATA(GL_UNSIGNED_BYTE);
        CHECK_TYPE_OF_DATA(GL_SHORT);
        CHECK_TYPE_OF_DATA(GL_UNSIGNED_SHORT);
        CHECK_TYPE_OF_DATA(GL_INT);
        CHECK_TYPE_OF_DATA(GL_UNSIGNED_INT);
        CHECK_TYPE_OF_DATA(GL_FLOAT);
        CHECK_TYPE_OF_DATA(GL_FIXED);
        
        CHECK_NORMALIZED(false);
        CHECK_NORMALIZED(true);

        SmartP<ShaderAttrib> sp(attrib);

        return sp.DynamicCastToSmartRTTI();
    }


    /** 
     * s_XML_type_uniform
     *
     * Example: 
     *          <param name="u_matrix_world" type="uniform" uniformType="UNIFORM_TYPE_MATRIX_4FV" matrixIsTranspose="false" />
     *          <param name="u_samp_tex0"    type="uniform"	uniformType="UNIFORM_TYPE_SAMPLER2D" samplerOffset="0" file="texture.png" />
     *          <param name="u_samp_tex0"    type="uniform" uniformType="UNIFORM_TYPE_SAMPLERCUBE" samplerOffset="0" filePositiveX="lf.tga" fileNegativeX="rt.tga"
	 *												                                            filePositiveY="up.tga" fileNegativeY="dn.tga"
	 *													                                            filePositiveZ="ft.tga" fileNegativeZ="bk.tga"
	 *													                                            />
     */
    else if(type.compare(s_XML_type_uniform) == 0)
    {
        const char* uniformType = xmlNode->Attribute(s_XML_uniformType);
        VSGE_ASSERT(uniformType != nullptr);
        
        const char* matrixIsTranspose = xmlNode->Attribute(s_XML_matrixIsTranspose);
        if(!matrixIsTranspose) matrixIsTranspose = "false";

        ShaderUniform* uniform = NEW ShaderUniform(m_name.c_str(), UNIFORM_TYPE_1F);

        #define CHECK_TYPE_OF_UNIFORM(uType) if(strcmp(uniformType, #uType) == 0) uniform->SetTypeOfUniform(uType);
        #define CHECK_TRANSPOSE(value) if(strcmp(matrixIsTranspose, #value) == 0) uniform->SetTranspose(value);

        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_1F);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_1FV);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_1I);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_1IV);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_2F);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_2FV);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_2I);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_2IV);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_3F);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_3FV);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_3I);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_3IV);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_4F);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_4FV);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_4I);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_4IV);
		CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_MATRIX_2FV);
		CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_MATRIX_3FV);
		CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_MATRIX_4FV);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_SAMPLER2D);
        CHECK_TYPE_OF_UNIFORM(UNIFORM_TYPE_SAMPLERCUBE);


        CHECK_TRANSPOSE(false);
        CHECK_TRANSPOSE(true);

        if(uniform->GetTypeOfUniform() == UNIFORM_TYPE_SAMPLER2D) 
        {
            int samplerOffset = 0;
            const char* value = xmlNode->Attribute(s_XML_samplerOffset, &samplerOffset);
            VSGE_ASSERT(value != nullptr);

            const char* filename = xmlNode->Attribute(s_XML_file);
            
            if((filename != NULL) && (strlen(filename) > 0))
            {
                SmartP<Texture> sp = TexturesMgr::GetInstance()->GetOrLoadFromFile(filename, filename);
                uniform->SetValue(sp.GetPtr());
            }

            uniform->SetOffset(samplerOffset);
        }
        else if(uniform->GetTypeOfUniform() == UNIFORM_TYPE_SAMPLERCUBE)
        {
            ///the samplerOffset is used to bind the texture on the specific location.Ex: texture->Bind(GL_TEXTURE0 + GetOffset());
            int samplerOffset = 0;
            const char* value = xmlNode->Attribute(s_XML_samplerOffset, &samplerOffset);
            VSGE_ASSERT(value != nullptr);


            const char* filenamePX = xmlNode->Attribute(s_XML_filePositiveX);
            VSGE_ASSERT(filenamePX != nullptr);

            const char* filenameNX = xmlNode->Attribute(s_XML_fileNegativeX);
            VSGE_ASSERT(filenameNX != nullptr);


            const char* filenamePY = xmlNode->Attribute(s_XML_filePositiveY);
            VSGE_ASSERT(filenamePY != nullptr);

            const char* filenameNY = xmlNode->Attribute(s_XML_fileNegativeY);
            VSGE_ASSERT(filenameNY != nullptr);


            const char* filenamePZ = xmlNode->Attribute(s_XML_filePositiveZ);
            VSGE_ASSERT(filenamePZ != nullptr);

            const char* filenameNZ = xmlNode->Attribute(s_XML_fileNegativeZ);
            VSGE_ASSERT(filenameNZ != nullptr);


            SmartP<Texture> sp = TexturesMgr::GetInstance()->GetOrLoadFromFile(filenamePX,/* the key */
                                                                               filenamePX, filenameNX,
                                                                               filenamePY, filenameNY,
                                                                               filenamePZ, filenameNZ);

            uniform->SetOffset(samplerOffset);
            uniform->SetValue(sp.GetPtr());
        }


        SmartP<ShaderUniform> sp(uniform);

        return sp.DynamicCastToSmartRTTI();
    }


    /** 
     * s_XML_type_define
     *
     * Example: 
     *          <param name="lighting"		type="define" 	value="LIGHTING" />
	 *          <param name="PinkOnly"		type="define" 	value="PINK_ONLY" />
     */
    else if(type.compare(s_XML_type_define) == 0)
    {
        const std::string value = xmlNode->Attribute(s_XML_value);
        
        ShaderDefine* define = NEW ShaderDefine(value);

        SmartP<ShaderDefine> sp(define);

        return sp.DynamicCastToSmartRTTI();
    }

    
    /** 
     * s_XML_type_LevelObject
     *
     * Example: 
     *          <param name="Ground"    type="LevelObject" posX="0" posY="0" posZ="0" rotX="0" rotY="0" rotZ="0" file="objects/Grid.xml"/> 
     */
    else if(type.compare(s_XML_type_LevelObject) == 0)
    {
        double posx = 0.0f;
        xmlNode->Attribute("posx", &posx);
        double posy = 0.0f;
        xmlNode->Attribute("posy", &posy);
        double posz = 0.0f;
        xmlNode->Attribute("posz", &posz);

        double rotx = 0.0f;
        xmlNode->Attribute("rotx", &rotx);
        double roty = 0.0f;
        xmlNode->Attribute("roty", &roty);
        double rotz = 0.0f;
        xmlNode->Attribute("rotz", &rotz);

        const char* file = xmlNode->Attribute(s_XML_file);

       
        LevelObject* lo = NEW LevelObject();
        lo->m_pos = Vector3f((float)posx, (float)posy, (float)posz);
        lo->m_rot = Vector3f((float)rotx, (float)roty, (float)rotz);
        lo->m_fileName = file;

        SmartP<LevelObject> lvlobj(lo);
        
        return lvlobj.DynamicCastToSmartRTTI();
    }
    
    std::stringstream ss;
    ss << "Cannot convert type=" << type;
    LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__, ss);

    return SmartRTTI(NULL);
}


bool ResParams::LoadParams(const std::string& fileName)
{
    if(fileName.size() == 0) 
    {
        LogMgr::GetInstance()->LogEX(LogMgr::LOG_ERROR, __FILE__, __LINE__, "the filename is empty!");
        return false;
    }

    std::string fullPathToFile = FS::GetInstance()->GetFullPathToFile(fileName);

    TiXmlDocument doc(fullPathToFile.c_str());
    bool loadOkay = doc.LoadFile();
    if (loadOkay)
    {
        std::stringstream ss;
        ss << "Loading: " << fullPathToFile;
        LogMgr::GetInstance()->Log(LogMgr::LOG_DEBUG, ss);

        TiXmlElement* root = doc.RootElement();
        if ( root && (strcmp(root->Value(), s_XML_Params) == 0))
        {
            //if(m_name.size() == 0)
            //{
            //    m_name = fileName;
            //}

            if(m_classTypeID.size() == 0)
            {
                const char* clsName = root->Attribute(s_XML_class);
                if((clsName != NULL) && (strlen(clsName) > 0))
                    m_classTypeID = std::string(clsName);
            }

            //std::stringstream ss;
            //ss << "fileName=" << fileName;
            //LogMgr::GetInstance()->Log(LogMgr::LOG_DEBUG, ss);

            this->LoadParams(*root);
        }
        else
        {
            std::stringstream ss;
            ss << "Failed to find " << s_XML_Params;
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_DEBUG, __FILE__, __LINE__, ss);

            return false;
        }
    }
    else
    {
        std::stringstream ss;
        ss << "Failed to load file " << fileName << endl;
        ss << "XML problems: " << doc.ErrorDesc() << endl;
        LogMgr::GetInstance()->LogEX(LogMgr::LOG_ERROR, __FILE__, __LINE__, ss);

        return false;
    }

    return true;
}

bool ResParams::LoadParams(const TiXmlElement& rootNode)
{     
    TiXmlElement* xmlNode = (TiXmlElement*)rootNode.FirstChildElement();
    while(xmlNode)
    {
        if(strcmp(xmlNode->Value(), s_XML_import) == 0)
        {
            const char* file = xmlNode->Attribute(s_XML_file);

            std::string fullPathToFile = FS::GetInstance()->GetFullPathToFile(file);

            //FS::GetInstance()->PushPath(FS::GetOnlyPath(fullPathToFile));

            bool success = LoadParams(fullPathToFile);

            //FS::GetInstance()->PopPath();

            if(!success)
            {
                std::stringstream ss;
                ss << " Failed to load params file " << fullPathToFile << endl;
                LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__, ss);

                return false;
            }
        }
        else
        if(strcmp(xmlNode->Value(), s_XML_param) == 0)
        {
            const char* name = xmlNode->Attribute(s_XML_name);
            if(name == NULL)
            {
                LogMgr::GetInstance()->Log(LogMgr::LOG_DEBUG, "name=NULL\n");

                continue;
            }

            const char* type = xmlNode->Attribute(s_XML_type);
            if(type == NULL)
            {
                LogMgr::GetInstance()->Log(LogMgr::LOG_DEBUG, "type=NULL\n");

                continue;
            }

            
            this->Add(name, SmartP<RPContent>(NEW RPContent(name, type, xmlNode)));
            //m_params[name] = NEW ParamContent(name, type, xmlNode);
        }

        xmlNode = (TiXmlElement*)xmlNode->NextSibling();
    }

   
    return true;
}

SmartP<RPContent> ResParams::GetFirstParamOfType(const std::string& type)
{
    auto it= m_map.begin();

    for( ; it != m_map.end(); ++it)
    {
        SmartP<RPContent> prm = (*it).second;
        if(prm->GetType().compare(type)== 0)
            return prm;
    }

    std::stringstream ss;
    ss << "Failed to find any parameter of type='" << type << "'";
    LogMgr::GetInstance()->LogEX(LogMgr::LOG_WARNING, __FILE__, __LINE__, ss);

    return NULL;
}

std::vector< SmartP<RPContent> > ResParams::GetAllParamsOfType(const std::string& type)
{
    std::vector< SmartP<RPContent> > returnVector;

    auto it= m_map.begin();

    for( ; it != m_map.end(); ++it)
    {
        SmartP<RPContent> prm = (*it).second;
        if(prm->GetType().compare(type)== 0)
            returnVector.push_back(prm);
    }

    return returnVector;
}
