/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <ostream>
#include <sstream>

#ifdef _WIN32
#include <math.h>
#endif

#include <ctime>


using namespace std;

namespace gu
{

    /** the PI number does not need any comments */
    #ifndef PI
    #define PI (3.1415926535897932384626433832795)
    #endif


    /** This enum is used to specify the anchors */
    enum
    {
        A_FREE      = 0,

        A_LEFT      = 1,
        A_HCENTER   = 2,
        A_RIGHT     = 4,

        A_TOP       = 8,
        A_VCENTER   = 16,
        A_BOTTOM    = 32
    };


    /**
    * This is a helper function to split a big string in smaller strings.
    * @param str is the string that need to be splitted.
    * @param delimiters is a string representing the separators.
    * @return an vector with all the found strings.
    */ 
    vector<string> tokenize(const string& str,const string& delimiters);

    /**
    * Helper function to read froma file an int value. So the next 4 bites from file will be converted to an int value. 
    * @param file is a pointer to file .
    * @param isLittleEndian means how the 4 bites will be interpreted.
    * @return an int or 0 if cannot be read 4 bites from that file.
    */
    unsigned int GetU32(FILE *file, bool isLittleEndian = true);

    /**
    * Helper function to read froma file an int value. So the next 2 bites from file will be converted to an short value. 
    * @param file is a pointer to file .
    * @param isLittleEndian means how the 2 bites will be interpreted.
    * @return an short or 0 if cannot be read 2 bites from that file.
    */
    unsigned short GetU16(FILE *file, bool isLittleEndian = true);

    /**
    * Helper function to read froma file an int value. So the next 1 bite from file will be converted to an char value. 
    * @param file is a pointer to file .
    * @return an char or 0 if cannot be read 1 bites from that file.
    */
    unsigned char GetU8(FILE *file);


    /**
    * This will calculate the angle between tow lines. The lines are caracterized by the two points (for each line).
    * @param x1 is the x for the first line.
    * @param y1 is the y for the first line.
    * @param x2 is the x for the second line.
    * @param y2 is the y for the second line.
    * @return the angle (in degres: 0-360)
    */
    double GetAngle(int x1, int y1, int x2, int y2);

    /** 
     * This function will show an assert message and the app will crash.
     * @param x is the condition that will be evaluated.
     * @param msg is the message that will be displayed.
     * @param file is the source file name where the assert occured.
     * @param line is the line number where the assert occured.
     * @return false if the assertion does not occured.
     */
    bool GUAssert(bool x, const char *msg, char* file, unsigned int line);

    /** Swtch the values */
    template<class T> void SwitchValue(T &a, T &b)
    {
        T buffer = a;
        a = b;
        b = buffer;
    };

    /**
     * Check if the point is inside the rectangle.
     * @param x is the x coord of the point.
     * @param y is the y coord of the point.
     * @param x1 is the x coord of the rectangle.
     * @param y1 is the y coord of the rectangle.
     * @param x2 is the x coord of the rectangle.
     * @param y2 is the y coord of the rectangle.
     * @return true if the point is inside rectangle.
     */
    inline bool IsPointInside(int x, int y, int x1, int y1, int x2, int y2)
    {
        return (x1 <= x) && (x <= x2) && (y1 <= y) && (y <= y2);
    }


    /**
     * This function will return the position outX, outY regarding the point pX,pY and anchor.
     * If the anchor is A_LEFT means the outX = pX  (aka the left side of the point pX,pY).
     * If the anchor is A_RIGHT means the outX = (pX - W) (aka the right side of the point pX,pY).
     *    
     *      pX,pY                                    outX,outY  pX,pY  
     *      __|_____________                              +_______|________        
     *     |  |             |                             |       |        |
     *     |  +             |   A_HCENTER | A_VCENTER     |       |        |
     *   H |                |   ======================>   |_______|________|
     *     |                |          anchor             |       |        |
     *     |                |                             |       |        |
     *     |________________|                             |_______|________|
     *             W                                    
     */
    inline void ConvertPosRegardingAnchor(int pX, int pY, int anchor, int W, int H, int& outX, int& outY)
    {
        if((anchor&A_LEFT) == A_LEFT)
        {
            outX = pX;
        }
        else if((anchor&A_RIGHT) == A_RIGHT)
        {
            outX = pX - W;
        }
        else if((anchor&A_HCENTER) == A_HCENTER)
        {
            outX = pX - W/2;
        }

        if((anchor&A_TOP) == A_TOP)
        {
            outY = pY - H;
        }
        else if((anchor&A_BOTTOM) == A_BOTTOM)
        {
            outY = pY;
        }
        else if((anchor&A_VCENTER) == A_VCENTER)
        {
            outY = pY - H/2;
        }
    }

    
   
    /** this is a helper macro for ASSERT */
    #if defined(DEBUG) || defined(_DEBUG)
    #define GU_ASSERT(to_check) gu::GUAssert (to_check, #to_check, __FILE__, __LINE__ )
    #define GU_ASSERT_MSG(to_check, msg) gu::GUAssert (to_check, msg, __FILE__, __LINE__ )
    #else
    #define GU_ASSERT(to_check) ;
    #define GU_ASSERT_MSG(to_check, msg) ;
    #endif


    /** this is a helper macro for delete*/
    #ifndef SAFE_DELETE
    #define SAFE_DELETE(pointer) if(pointer){delete pointer; pointer = NULL;}
    #endif
           
    

    #ifndef _WIN32
        #include <unistd.h>
    #endif

    #define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

    #ifdef _WIN32
        #define SNPRINTF _snprintf_s
    #else
        #define SNPRINTF snprintf
    #endif
    
    #define INVALID_OGL_VALUE 0xFFFFFFFF

    /**
     * This will check if a bit si set or not.
     * @param value is the container of the flags.
     * @param flag_to_test is the flag that we want to see if is true or false.
     * @return true if that flag is set.
     */
	inline bool BIT_FLAG_IS_PRESENT(unsigned int value, unsigned int flag_to_test)
	{
		return ((value & flag_to_test) == flag_to_test);
	}


    unsigned int MillisSinceLastCall();


    template<typename T>
    std::string ToString(T val)
    {
        std::ostringstream s;
        s << val;
        return s.str();
    }


} //namespace gu


#endif //UTILS_H
