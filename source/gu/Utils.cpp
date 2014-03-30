/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#include "Utils.h"
#include "Log.h"
#include <chrono>


namespace gu
{
    vector<string> tokenize(const string& str,const string& delimiters)
    {
	    vector<string> tokens;
        	
	    // skip delimiters at beginning.
        string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        	
	    // find first "non-delimiter".
	    string::size_type pos = str.find_first_of(delimiters, lastPos);

	    while (string::npos != pos || string::npos != lastPos)
	    {
    	    // found a token, add it to the vector.
    	    tokens.push_back(str.substr(lastPos, pos - lastPos));
    	
    	    // skip delimiters.  Note the "not_of"
    	    lastPos = str.find_first_not_of(delimiters, pos);
    	
    	    // find next "non-delimiter"
    	    pos = str.find_first_of(delimiters, lastPos);
	    }

	    return tokens;
    }

    
    unsigned int GetU32(FILE *file, bool isLittleEndian)
    {
        unsigned char buffer[4];
        size_t result = fread (buffer, 1, 4, file);
        if(result != 4) return 0;

        unsigned int ret = 0;
        
        if(!isLittleEndian)
        {
            //bigEndian
            ret <<= 8;
            ret |= buffer[0];
            ret <<= 8;
            ret |= buffer[1];
            ret <<= 8;
            ret |= buffer[2];
            ret <<= 8;
            ret |= buffer[3];
        }
        else
        {
            //littleEndian
            ret <<= 8;
            ret |= buffer[3];
            ret <<= 8;
            ret |= buffer[2];
            ret <<= 8;
            ret |= buffer[1];
            ret <<= 8;
            ret |= buffer[0];        
        }

        return ret;
    }


    
    unsigned short GetU16(FILE *file, bool isLittleEndian)
    {
        //not implemented yet
        GU_ASSERT(false);
        return 0;
    }


    unsigned char GetU8(FILE *file)
    {
        unsigned char buffer[1];
        size_t result = fread (buffer, 1, 1, file);
        if(result != 4) return 0;

        return buffer[0];
    }


    

    double GetAngle(int x1, int y1, int x2, int y2)
    {
        double degrees;

        y1 = 0 - y1;
        y2 = 0 - y2;

        // Avoid divide by zero run values.
        if (x2 - x1 == 0)
        {
            if (abs(y2) > abs(y1))
                degrees = 270;
            else
                degrees = 90;
        }
        else
        {
            // Calculate angle from offset.
            double riseoverrun = (double)(y2 - y1) / (double)(x2 - x1);
            double radians = atan(riseoverrun);
            degrees = radians * ((double)180 / PI);

            // Handle quadrant specific transformations.       
            if ((x2 - x1) < 0 || (y2 - y1) < 0)
                degrees += 180;
            if ((x2 - x1) > 0 && (y2 - y1) < 0)
                degrees -= 180;
            if (degrees < 0)
                degrees += 360;
        }
        return degrees;
    }

    bool VsgeAssert(bool x, const char *msg, char* file, unsigned int line)
    {
        if( false == x)
        {  

            std::stringstream ss;
            ss << "Assert '" << msg << "'" << endl;
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_ERROR, file, line, ss);
            #ifdef WIN32
            ss << "file: '" << file << "'" << endl;
            ss << "line: " << line << endl;
            MessageBox(NULL, ss.str().c_str(), "ERROR", MB_SYSTEMMODAL|MB_ICONERROR|MB_OK);
            #endif
        
            abort(); 
            return true;
        }
        else
        {
            return false;
        }
    }

    unsigned int millisSinceLastCall()
    {
        // lastCall 
        static std::chrono::time_point<std::chrono::system_clock> lastCall = std::chrono::system_clock::now();
        
        // current time
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

        unsigned int valueToReturn = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCall).count();

        lastCall = now;

        return valueToReturn;
    }


} //namespace gu