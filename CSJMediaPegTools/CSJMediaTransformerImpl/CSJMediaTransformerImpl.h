#ifndef __CSJMEDIATRANSFORMERIMPL_H__
#define __CSJMEDIATRANSFORMERIMPL_H__

#include "CSJMediaTransformer/CSJMediaTransformer.h"

#include "CSJMediaPegHeader.h"

class CSJMediaTransformerImpl : public CSJMediaTransformer {
public:
    CSJMediaTransformerImpl();
    CSJMediaTransformerImpl(std::string& srcFile, std::string& destFile);
    ~CSJMediaTransformerImpl();

    void setTransformerFiles(std::string srcFile, std::string destFile) override;

    bool transformMedia() override;

protected:
    /**
     *  Initialize the input and output format context;
     */
    bool initFormatContext();

    /**
     *  Release the input and output format context;
     */
    void unInitFormatContext();

private:
    std::string m_srcFile;
    std::string m_destFile;

    AVFormatContext *m_inputFmtCtx = nullptr;
    AVFormatContext *m_outputFmtCtx = nullptr;

    // numbers of streams in the inputFormatCtx;
    int             m_streamNb = 0;             

    // an array to save the stream/index mapping of inputFormatCtx;
    int             *m_streamMapping = nullptr; 

    CSJMediaTransformer::Delegate *m_delegate;
};

#endif // __CSJMEDIATRANSFORMERIMPL_H__