#include "SharedCueSheet.h"

namespace ADX2 {
    
    SharedCueSheet * SharedCueSheet::_instance = nullptr;
    
    SharedCueSheet::SharedCueSheet()
    : _cueSheet(nullptr)
    {
    }
    
    SharedCueSheet::~SharedCueSheet()
    {
        CC_SAFE_RELEASE_NULL(_cueSheet);
    }
    
    void SharedCueSheet::initialize(const char *acf, const char *acb)
    {
        this->initialize(acf, acb, nullptr);
    }
    
    void SharedCueSheet::initialize(const char *acf, const char *acb, const char *awb)
    {
        CCASSERT(_cueSheet == nullptr, "You can't call SharedCueSheet::initialize twice");
        _cueSheet = CueSheet::create(acf, acb, awb);
        _cueSheet->retain();
    }
    
    SharedCueSheet * SharedCueSheet::getInstance()
    {
        if (_instance == nullptr) {
            _instance = new SharedCueSheet();
        }
        return _instance;
    }
    
    void SharedCueSheet::destroyInstance()
    {
        CC_SAFE_DELETE(_instance);
    }
    
}