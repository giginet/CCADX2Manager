//
//  ADX2Manager.h
//  CcAdx2Basic
//
//  Created by giginet on 2014/6/27.
//
//

#ifndef __CcAdx2Basic__ADX2Manager__
#define __CcAdx2Basic__ADX2Manager__

#include <iostream>
#include "cri_adx2le.h"

namespace ADX2 {
    
    class ADX2Manager {
     private:
        static ADX2Manager* _instance;
        
        ADX2Manager();
     public:
        
        virtual ~ADX2Manager();
        
        static ADX2Manager* initialize();
        static ADX2Manager* getInstance();
        void update();
        void finalize();
    };
    
}

#endif /* defined(__CcAdx2Basic__ADX2Manager__) */
