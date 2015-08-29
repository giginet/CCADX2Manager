#ifndef __CCADX2Manager__SharedCueSheet__
#define __CCADX2Manager__SharedCueSheet__

#include "cocos2d.h"
#include "CueSheet.h"

namespace ADX2 {
    class SharedCueSheet
    {
    private:
        static SharedCueSheet *_instance;
    CC_CONSTRUCTOR_ACCESS:
        SharedCueSheet();
        virtual ~SharedCueSheet();
    public:
        
        /*
         * get shared cue sheet instance
         */
        static SharedCueSheet * getInstance();
        
        /*
         * destroy shared cue sheet instance
         */
        static void destroyInstance();
        
        /*
         * initialize shared cue sheet instance by acf, acb, awb
         */
        void initialize(const char *acf, const char *acb);
        void initialize(const char *acf, const char *acb, const char *awb);
        
        CC_SYNTHESIZE_READONLY(ADX2::CueSheet *, _cueSheet, CueSheet);
    };
}

#endif /* defined(__CCADX2Manager__SharedCueSheet__) */
