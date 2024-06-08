#ifndef __CSJMEDIAHANDLERBASE_H__
#define __CSJMEDIAHANDLERBASE_H__

#include <memory>

class CSJMediaHandlerBase;
using CSJSharedHandler = std::shared_ptr<CSJMediaHandlerBase>;

typedef enum {
    CSJHANDLERTYPE_NONE = 0,
    CSJHANDLERTYPE_LIVE,            // the handler implements capacities about live.
    CSJHANDLERTYPE_PLAYER,          // the handler implements capacities about media player.
    CSJHANDLERTYPE_TRANSFORMCODE,   // the handler implements capacities about encoder transforming.
} CSJMediaHandlerType;

/**
 * This is the base class for all the handler, which provides
 * a function to stop a handler, so the subclasses of this 
 * class must implement the Stop() function.
 * when the program exit, it will invoke stop() function to 
 * teiminate the active handlers.
 */
class CSJMediaHandlerBase {
public:
    CSJMediaHandlerBase() = default;
    virtual ~CSJMediaHandlerBase() = default;

    static CSJSharedHandler getHandlerWithType(CSJMediaHandlerType handler_type);

    /**
     * @brief the function that stop the handler.
     */
    virtual void stopHandler() = 0;
};

#endif // __CSJMEDIAHANDLERBASE_H__