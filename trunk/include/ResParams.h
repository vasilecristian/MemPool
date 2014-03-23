/************************************************************************/
/*File created on 18.05.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef PARAMSMAP_H
#define PARAMSMAP_H

#include "vsge/base/fast-rtti/RTTI.h"
#include "vsge/base/smart_ptr.hpp"
#include "vsge/base/ManagerBase.hpp"
#include <string>

class TiXmlElement;

namespace vsge
{
    /**
     * This class will store one resource parameter. Along with the value, will be stored the type.
     * The m_value is smart pointer of smart_ptr<RTTI> type , so can store any type of instances if that instance 
     * is derived from IRTTI.
     */
    class RPContent:public IRTTI<RPContent>
    {
		/** The name of the parameter. */
        std::string m_name;

		/** The type of the parameter */
        std::string m_type;

		/** The value of the parameter */
        smart_ptr<RTTI> m_value;

        /**
         * This function will create an object of type 'type' with value 'value'.
         * The object is a parameter of type int, float, char*, and even mesh.
         * The return value is a smart_ptr<RTTI> (aka smart_ptr<RTTI>) for that pointer.
         * This function si designed to be used only internally (only in this class).
         *
         * @param type is a string representing the type of the object. If the type is not suported
         *        a null pointer will be returned (smart_ptr to NULL).
         * @param value is a TiXmlElement pointer containing the value. This value will be converted 
         *        from string in pointer of type 'type'
         * @return an smart_ptr<RTTI> for the instance. 
         */
        smart_ptr<RTTI> GetValueAsSmartRTTI(const std::string& type, TiXmlElement* xmlNode);

    public:

        /**
         * The Constructor. This will create an object with the name 'name' of type 'type'
         * and the content will be taken from xmlNode
         *
         * @param name is the name of this parameter (string).
         * @param type is the type of this parameter (string).
         * @param xmlNode is a pointer to TiXmlElement that contines the value.
         */
        RPContent(const std::string& name, const std::string& type, TiXmlElement* xmlNode);


        /**
         * The Constructor. This will create an object with the name 'name' of type 'type'
         * and the content will be taken from xmlNode
         *
         * @param name is the name of this parameter (string).
         * @param type is the type of this parameter (string).
         * @param value is a smart_ptr<RTTI> that contines the value.
         */
        RPContent(const std::string& name, const std::string& type, smart_ptr<RTTI> value);


		/** The destructor. */
        ~RPContent();

		/** @return the name of the parameter. */
        inline std::string& GetName(){return m_name;};

		/** @return the value of parameter */
        inline smart_ptr<RTTI> GetValue(){return m_value;};

		/** @return the type of the parameter. */
        inline std::string& GetType(){return m_type;};
    };

    
    static const char* s_XML_Params         = "Params";
    static const char* s_XML_class          = "class";
    static const char* s_XML_import         = "import";
    static const char* s_XML_file           = "file";
    static const char* s_XML_filePositiveX  = "filePositiveX";
    static const char* s_XML_fileNegativeX  = "fileNegativeX";
    static const char* s_XML_filePositiveY  = "filePositiveY";
    static const char* s_XML_fileNegativeY  = "fileNegativeY";
    static const char* s_XML_filePositiveZ  = "filePositiveZ";
    static const char* s_XML_fileNegativeZ  = "fileNegativeZ";
    static const char* s_XML_param          = "param";
    static const char* s_XML_name           = "name";
    static const char* s_XML_type           = "type";
    static const char* s_XML_value          = "value";
    static const char* s_XML_meshFile       = "meshFile";
    static const char* s_XML_materialFile   = "materialFile";
    

    static const char* s_XML_TextureName    = "Texture";

    static const char* s_XML_type_Shader    = "Shader";
    static const char* s_XML_type_attribute = "attribute";
    static const char* s_XML_type_uniform   = "uniform";
    static const char* s_XML_type_define    = "define";

    static const char* s_XML_type_Object3D  = "Object3D";
    static const char* s_XML_type_int       = "int";
    static const char* s_XML_type_double    = "double";
    static const char* s_XML_type_string    = "string";
    static const char* s_XML_type_LevelObject   = "LevelObject";

    static const char* s_XML_VertexShaderFile   = "VertexShaderFile";
    static const char* s_XML_FragmentShaderFile = "FragmentShaderFile";

    static const char* s_XML_dataSize       = "dataSize";
    static const char* s_XML_dataOffset     = "dataOffset";
    static const char* s_XML_dataType       = "dataType";
    static const char* s_XML_dataNormalized = "dataNormalized";

    static const char* s_XML_uniformType    = "uniformType";
    static const char* s_XML_samplerOffset  = "samplerOffset";
    static const char* s_XML_matrixIsTranspose  = "matrixIsTranspose";


    /**
     * This class is used to load and store the parameters from a xml file.
     * This parameters can be any resource type (int, double values, mesh, geometry, etc...)
     * The structure of the xml must be like this:
     * ------------------------------------
     * <?xml version="1.0"?>
     * <Params class="Ship">
     *      <import file="BaseUnit.xml"/>
     * 
     *      <param name="speed1"    type="int"       value="0" />
     *      <param name="speed2"    type="int"       value="0" />
     *      <param name="Body"      type="Object3D"  meshFile="geometry/T-90/T-90.dae" materialFile="materials/Material.xml"/>
     *      <param name="R2D2"      type="Object3D"  meshFile="geometry/R2D2/R2D2.dae" materialFile="materials/OnlyWVP.xml"/>
     *      <param name="Shader1"   type="Shader"    VertexShaderFile="shader_vert.c" FragmentShaderFile="shader_frag.c"/> 
     *      <param name="Shader"    type="Shader"    VertexShaderFile="shaders/shader_vert.c" FragmentShaderFile="shaders/shader_frag.c"/> 
     *      <param name="Name"      type="string"    value="text"/> 
     *      <param name="Ground"    type="LevelObject" posX="0" posY="0" posZ="0" rotX="0" rotY="0" rotZ="0" file="objects/Grid.xml"/> 
     *      <param name="name"      type="Object3D" meshFile="geometry/MDewCan/MDewCan.obj" materialFile="materials/Texture_MDewCan.material"/>    
     *      <param name="u_samp_tex0"   type="uniform"  	uniformType="UNIFORM_TYPE_SAMPLER2D" samplerOffset="0" file="textures/betty.tga" />
	 *      <param name="u_samp_tex0"   type="uniform"  	uniformType="UNIFORM_TYPE_SAMPLERCUBE" samplerOffset="0" 
     *                                                      filePositiveX="lf.tga" fileNegativeX="rt.tga" filePositiveY="up.tga"
	 *  												    fileNegativeY="dn.tga" filePositiveZ="ft.tga" fileNegativeZ="bk.tga" />
     *
     *      <param name="a_pos"   			type="attribute"  	dataSize="3" dataType="GL_FLOAT" dataNormalized="false" dataOffset="0"/>
	 *      <param name="a_texture_uv"  	type="attribute"  	dataSize="2" dataType="GL_FLOAT" dataNormalized="false" dataOffset="0"/>
	 *      <param name="a_norm"   			type="attribute"  	dataSize="3" dataType="GL_FLOAT" dataNormalized="false" dataOffset="0"/>
	 *      <param name="u_matrix_world"	type="uniform" 		uniformType="UNIFORM_TYPE_MATRIX_4FV" matrixIsTranspose="false" />
	 *      <param name="u_matrix_wvp"		type="uniform" 		uniformType="UNIFORM_TYPE_MATRIX_4FV" matrixIsTranspose="false" />
     *   	
     *
     *      <param name="lighting"		type="define" 	value="LIGHTING" />
	 *      <param name="PinkOnly"		type="define" 	value="PINK_ONLY" />
     *
     * </Params>
     * ------------------------------------
     *
     * <import> read all the parameters from a similar file.
     * <param> will override or add the values to the map.
     */
    class ResParams: public ManagerBase<const std::string, smart_ptr<RPContent> >
    {  
        
    protected:
        
        /**
         * This is the name of the class that must be created to use this parameters.
         * Will be used by the InstalceFactory mechanism.
         */
        std::string m_classTypeID;


    public:

        /**  The Constructor of ResParams */
        ResParams():m_classTypeID("")
        {};

		/** The destructor */
        virtual ~ResParams()
        {};

        /**
         * This is a virtual function that load the data from an xml node.
         * 
         * @param rootNode is a TiXmlElement& for the root node (aka <Params>)
         * @return is true or false for Success or Fail.
         */
        virtual bool LoadParams(const TiXmlElement& rootNode);

        /**
         * This is a virtual function that load the data from an xml file.
         * 
         * @param fileName is the file ame(and path) of the xml that contines parameters.
         * @return is true or false for Success or Fail.
         */
        virtual bool LoadParams(const std::string& fileName);


        /**
         * This is a virtual function that save the data to an xml file.
         * 
         * @param fileName is the filename(and path) of the xml with parameters.
         * @return is true or false for Success or Fail.
         */
        virtual bool SaveParams(const std::string& fileName){return false;};


        /**
         * Get the string with class type ID (aka the name of the class)
         * The class name is speciffied in xml file. Ex: <Params class="Ship">
         * This is used in a Class Factory mechanism. 
         * @return an std::string representing the string with class name
         */
        inline std::string& GetClassTypeID(){return m_classTypeID;};


		/**
         * Get the parameter with the name paramName.
         * 
         * @return an Smart pointer for the parameter with paramName.
         */
        inline smart_ptr<RPContent> GetParam(const std::string& paramName)
        {
            return this->Get(paramName); 
        }


        /**
         * Get the first parameter with the type 'type'.
         * 
         * @return an Smart pointer for the parameter with type 'type'.
         */
        smart_ptr<RPContent> GetFirstParamOfType(const std::string& type);


        /**
         * Get all parameters with the type 'type'.
         * 
         * @return an vector of Smart pointers for the parameter with type 'type'.
         */
        std::vector< smart_ptr<RPContent> > GetAllParamsOfType(const std::string& type);



    private:
        /**
         * Use this to Get the object from the map or load it if does 
         * not exists in the map. The object loaded is added to the map.
         * If the load fails will be returned an NULL object.
         * @param keyName is the name of the key that you want get.
         * @param file represent the filename to load.
         * @return the object loaded.
         */
        smart_ptr<RPContent> GetOrLoadFromFile(const std::string& keyName, const std::string& file) override
        {
            ///do not use this becaseu is not applicable
            VSGE_ASSERT(false);
            return  smart_ptr<RPContent>(NULL);
        }
    };
};

#endif //PARAMSMAP_H