#ifndef __CSJMEDIATRANSFORMER_H__
#define __CSJMEDIATRANSFORMER_H__

#include "CSJMediaPegToolDefine.h"

#include <string>

/**
 * The main functionality of this class is transforming media file from 
 * a media type to another media type. The type is indicated by the suffix
 * of the file.
 */
class CSJMEDIAPEGTOOL_API CSJMediaTransformer {
public:
    /**
     *  Provides some interface to notify the transforming status.
     */
    class Delegate {
    public:
        Delegate() = default;
        ~Delegate() = default;

        /**
         *  Notify the transforming progress.
         */
        virtual void notifyTranformProgress() {};

        /**
         *  Notify the transforming status and result.
         */
        virtual void notifyTranformingStatus() {};
    };

    CSJMediaTransformer() {};

    virtual ~CSJMediaTransformer() {};

    /**
     *  Get a transformer instance, and users must set the source file and 
     *  destination file by other interfaces.
     */
    static CSJMediaTransformer* getTransformer();

    /**
     *  Get a transformer instance by the src file and destination file.
     */
    static CSJMediaTransformer* getTransformerWithFileName(std::string& srcFile, 
                                                           std::string& destFile);

    /**
     *  Set the source file and destination file.
     */
    virtual void setTransformerFiles(std::string srcFile, std::string destFile) = 0;

    /**
     *  Transfomer function. If the file type isn't supported, this function will
     *  turns false. The same as if the source file and destination file is empty.
     */
    virtual bool transformMedia() = 0;
};

#endif // __CSJMEDIATRANSFORMER_H__