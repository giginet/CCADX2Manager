/****************************************************************************
 *
 * CRI ADX2 SDK
 *
 * Copyright (c) 2009-2013 CRI Middleware Co., Ltd.
 *
 * Title   : CRI ADX2 sample program
 * Module  : Simple Playback using Cocos2Dx
 *         : Cocos2Dxでの単純再生サンプル
 * File    : HelloWorldScene.cpp
 *
 ****************************************************************************/

/*
 このサンプルは、Cocos2Dxによって、サウンドをキューID指定再生します。
 画面上のUIボタンは以下のような機能を持っています。
 　(1) 左側の再生ボタン x8	:	それぞれ対応するIDのキューを再生
 　(2) 右上の停止ボタン x2	:	PlaybackId単位の停止，Player単位の停止
 　(3) 右下のラベル x2	:	再生状態表示、再生時間表示。 情報は毎フレーム更新
 */

#include "HelloWorldScene.h"
#include "ADX2Manager.h"

/* プレイヤーの状態 */
const std::string g_playback_status_description[] = {
    "--",
    "Prepare",
    "Playing",
    "Removed",
};

/* キュー一覧 */
CriAtomExCueId g_cue_list[] = {
    CRI_BASIC_MUSIC1,
    CRI_BASIC_MUSIC2,
    CRI_BASIC_VOICE,
    CRI_BASIC_KALIMBA_RANDOM,
    CRI_BASIC_KALIMBA,
    CRI_BASIC_GUNSHOT,
    CRI_BASIC_BOMB_LIMIT,
    CRI_BASIC_BOMB,
    CRI_BASIC_VOICE_EFFECT
};
const CriUint32 g_num_cue_items = sizeof(g_cue_list)/sizeof(CriAtomExCueId);

USING_NS_CC;

//---------------------------------------------
//---- cocos2d-x ----
//---------------------------------------------

/* 画面に配置するUIボタンに割り当てるコールバック。音を止める */
void HelloWorld::menuStop(Ref* pSender)
{
    _cue->stop(currentPlaybackId);
}

/* 画面に配置するUIボタンに割り当てるコールバック。音をプレイヤーごと止める */
void HelloWorld::menuAllStop(Ref* pSender)
{
    ADX2::ADX2Manager::getInstance()->stopAll();
}

bool HelloWorld::init()
{
    CCLOG("%s", "HelloWorld::init");
    if ( !Layer::init() )
    {
        return false;
    }
    cocos2d::Size size = cocos2d::Director::getInstance()->getWinSize();
    
    /* GUIの設定と生成。そして配置。ボタンとラベル */
    {
        /* MenuItem (ボタン) をためこむベクタ。あとで一気に一個のMenuとしてadd */
        cocos2d::Vector<cocos2d::MenuItem*> buttonItems;
        
        /* ボタン生成 & ベクタへのプッシュバックの処理 */
        auto pushButton = [this, &size, &buttonItems](float x, float y,
                                                      const char* ui_normal_image, const char* ui_push_image, cocos2d::ccMenuCallback c){
            auto *item = cocos2d::MenuItemImage::create(ui_normal_image, ui_push_image, std::move(c));
            item->setPosition(cocos2d::Point(size.width  * x, size.height * y));
            buttonItems.pushBack(item);
        };
        
        /* ラベル生成 & UIとして追加の処理 */
        auto addLabel = [this, &size](cocos2d::Label *pLabel, float x, float y){
            pLabel->setPosition(cocos2d::Point(size.width  * x, size.height * y));
            addChild(pLabel);
        };
        
        /* 鳴らしたいキューの数だけボタンを作る */
        for (CriAtomExCueId id = 0; id < g_num_cue_items; id++) {
            pushButton(0.10f, 0.95f - static_cast<float>(id)*0.10f, "PlayNormal.png", "PlayPush.png",
                       [this, id](Ref*)->void{currentPlaybackId = _cue->playCueByID(id);});
            
            /* キュー名のラベルを併記 */
            addLabel(cocos2d::Label::createWithTTF(_cue->getCueName(id), "fonts/Marker Felt.ttf", 30),
                     0.30f, 0.95f - static_cast<float>(id)*0.10f);
        }
        
        /* 音声停止ボタン (プレイバックID単位) */
        {
            pushButton(0.60f, 0.95f, "StopNormal.png", "StopPush.png",
                       CC_CALLBACK_1(HelloWorld::menuStop, this));
            addLabel(cocos2d::Label::createWithTTF("Stop", "fonts/Marker Felt.ttf", 30),
                     0.75f, 0.95f);
        }
        
        /* 音声停止ボタン (プレイヤー単位) */
        {
            pushButton(0.60f, 0.85f, "StopNormal.png", "StopPush.png",
                       CC_CALLBACK_1(HelloWorld::menuAllStop, this));
            addLabel(cocos2d::Label::createWithTTF("StopAll", "fonts/Marker Felt.ttf", 30),
                     0.75f, 0.85f);
        }
        
        /* ここまでに作った全てのMenuItem(ボタン)をGUIとして追加 */
        {
            cocos2d::Menu* menu = cocos2d::Menu::createWithArray(buttonItems);
            menu->setPosition(cocos2d::Point::ZERO);
            this->addChild(menu);
        }
        
        /* 現在のボイス数を示すラベルを追加 */
        {
            //adx2_get_voice_num(),
            
            vnumLabel = cocos2d::Label::createWithTTF(StringUtils::toString(ADX2::ADX2Manager::getInstance()->getVoiceNum()),
                                                  "fonts/Marker Felt.ttf", 30);
            addLabel(vnumLabel, 0.75f, 0.15f);
        }
        
        /* 現在のプレイヤー状態を示すラベルを追加 */
        {
            stateLabel = cocos2d::Label::createWithTTF(
                                                   g_playback_status_description[ _cue->getStatus(currentPlaybackId) ],
                                                   "fonts/Marker Felt.ttf", 30);
            addLabel(stateLabel, 0.75f, 0.10f);
        }
        
        /* 現在の再生時間を示すラベルを追加 */
        {
            auto time = _cue->getTime(currentPlaybackId);
            auto string = StringUtils::format("[Time(msec): %s]", StringUtils::toString(time).c_str());
            timeLabel = cocos2d::Label::createWithTTF(
                                                  string,
                                                  "fonts/Marker Felt.ttf", 30);
            addLabel(timeLabel, 0.75f, 0.05f);
        }
    }
    
    scheduleUpdate();
    
    return true;
}

void HelloWorld::update(float dt)
{
    /* ADX2 Atomライブラリのアップデート処理 */
    ADX2::ADX2Manager::getInstance()->update();
    
    /* ラベル情報の更新 */
    vnumLabel->setString(StringUtils::toString(ADX2::ADX2Manager::getInstance()->getVoiceNum()));
    stateLabel->setString(g_playback_status_description[ _cue->getStatus(currentPlaybackId) ]);
    timeLabel->setString(StringUtils::toString(_cue->getTime(currentPlaybackId)));
}


cocos2d::Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = cocos2d::Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

HelloWorld::HelloWorld() : _cue(nullptr)
{
    /* ADX2 Atomライブラリの初期化。内部的に多重初期化は回避している */
    ADX2::ADX2Manager::initialize();
    
    auto cue = ADX2::Cue::create("ADX2_samples.acf", "Basic.acb", "Basic.awb");
    this->setCue(cue);
}

HelloWorld::~HelloWorld()
{
    CC_SAFE_RELEASE_NULL(_cue);
    ADX2::ADX2Manager::finalize();
}

