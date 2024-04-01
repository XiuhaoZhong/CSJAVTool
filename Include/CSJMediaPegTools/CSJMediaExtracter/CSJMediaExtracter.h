#ifndef __CSJMEDIAEXTRACTER_H__
#define __CSJMEDIAEXTRACTER_H__

#include <string>
#include <memory>

#include "CSJMediaPegToolDefine.h"

class CSJMediaExtracter;
using CSJSharedExtracter = std::shared_ptr<CSJMediaExtracter>;

typedef enum {
    CSJ_EXTRACTDATA_VIDEO = 0,  // extract video data;
    CSJ_EXTRACTDATA_AUDIO,      // extract audio data ;
} CSJExtracteType;

/*
 * This class defines interfaces that can extract video/audio data
 * from multiple media files. Users can use the interfaces to write
 * video/audio data into a new file or get the data in memory bytes
 * streams.
 *
 * Users can get a instance of this class's implementantion class or 
 * implement the interfaces by theirselves.
 *
 */
class CSJMEDIAPEGTOOL_API CSJMediaExtracter {
public:
    /*
    *  A delegate to notify extracting process and status.
    *
    *  Users can implement these interface to get informations while extracting
    *  Data.
    */
    class Delegate {
    public:
        Delegate() = default;

        /*
        *  Notify the progress.
        *
        *  @param progress the extracting progress.
        */
        virtual void notifyProgress(float progress) = 0;

        /*
        *  Notify the status.
        *
        *  When extracting operation occurs some problem or exception,
        *  extracter will notify by this function;
        */
        virtual void notifyStatus() = 0;
        
    };
    /*
    *  Generate a CSJMediaExtracter object.
    *  Users only use this object to extract fixed data from fixed files.
    *  
    *  @param srcFilePath the source file path.
    *  @param dstFilePath the path of the new file that saves the extracted data.
    *  @param type        the type of the data that users want to extract.
    *  
    *  @return            a instance of CSJMediaExtracter. 
    */
    static CSJSharedExtracter getExtracter(std::string &srcFilePath, std::string& dstFilePath, CSJExtracteType type);

    /*
    *  Generate a CSJMediaExtracter object.
    *  Uses can use the same object to extract data repeatly.
    *
    *  @param type the type of the data that users want to extract.
    *
    *  @return            a instance of CSJMediaExtracter.
    */
    static CSJSharedExtracter getExtracter(CSJExtracteType type);

    /*
    *  Set a delegate to the extracter.
    */
    virtual void setDelegate(std::shared_ptr<CSJMediaExtracter::Delegate> delegate) {};

    /*
    *  If users only create extracter with CSJExtracteType, users must set the source file
    *  and destination file with this function, or the extracData(...) function will fail. 
    *
    *  @param srcFilePath the source file path.
    *  @param dstFilePath the path of the new file that saves the extracted data.
    */
    virtual void setFilePath(std::string &srcFilePath, std::string& dstFilePath) = 0;

    /*
     *  Extract data.
     *  If the srcFilePath or the dstFilePath isn't set, the operation 
     *  will fail.
     *
     *  @return return true if the operation successully, or return false.
     *
     */
    virtual bool extractData() = 0;

    virtual ~CSJMediaExtracter() {}
};

#endif // __CSJMEDIAEXTRACTER_H__