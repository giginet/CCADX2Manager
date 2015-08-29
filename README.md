# CCADX2Manager

This is wrapper classes collection for ADX2 LE for cocos2d-x.

# Usage

```cpp

#include "ADX2Manager.h"

bool init()
{
  if (!Layer::init()) {
    return false;
  }
  ADX2::Manager::initialize();

  _cueSheet = CueSheet::create("sample.acf", "sample.acb");
  _cueSheet->playCueByID(CRI_SAMPLE_BGM);

  this->scheduleUpdate();

  return true;
}

void update(float dt)
{
  ADX2::Manager::getInstance()->update();
}

```

### Shared CueSheet Manager

If you would like to use same cue sheet globally, `SharedCueSheet` may help you.


```cpp
bool AppDelegate::applicationDidFinishLaunching() {
    // get singleton shared sheet
    auto sharedCueSheet = ADX2::SharedCueSheet::getInstance();

    // initialize shared cue sheet by acf and acb
    sharedCueSheet->initialize("sample.acf", "sample.acb");

    // get cue sheet
    sharedCueSheet->getCueSheet()->playCueByID(CRI_SAMPLE_BGM);

    // purge shared cue sheet
    SharedCueSheet::destroyInstance();
}
```

# Run example

1. copy `cocos2d` to `example`
2. Open `proj.ios/CcAdx2Basic.xcodeproj`
3. Build & Run
