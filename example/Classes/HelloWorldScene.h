#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "Cue.h"

#include <string>

#include "ADX2/ADX2_samples_acf.h"
#include "ADX2/Basic.h"

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init() override;
    virtual void update(float dt) override;
    
    CC_SYNTHESIZE_RETAIN(ADX2::Cue*, _cue, Cue);
    
private:
    HelloWorld();
    ~HelloWorld();
    
    // a selector callback
    void menuStop(cocos2d::Ref* pSender);
    void menuAllStop(cocos2d::Ref* pSender);
    
    cocos2d::Label *vnumLabel;
    cocos2d::Label *stateLabel;
    cocos2d::Label *timeLabel;
    
    CriSint64			currentPlaybackId;
};

#endif // __HELLOWORLD_SCENE_H__
