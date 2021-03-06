#include "TitleScene.h"

#include "MainScene.h"
#include"AudioManager.h"

USING_NS_CC;

const int INITIAL_LEVEL = 1;

Scene *TitleScene::createScene()
{
	auto scene = Scene::create();
	auto layer = TitleScene::create();
	scene->addChild(layer);

	return scene;
}

TitleScene::TitleScene()
	:titleBGM(0)
{
	Music::mainMusicID = -1;
}

TitleScene::~TitleScene()
{

}
void TitleScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	titleBGM = experimental::AudioEngine::play2d("sounds/title_bgm.mp3", 0.8f);
}

bool TitleScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	cocos2d::Size winSize = Director::getInstance()->getWinSize();

	//wi\ฆ
	auto background1 = Sprite::create("graphics/sky.png");
	background1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	background1->setPosition(Vec2(0, 0));
	background1->getTexture()->setAliasTexParameters();

	auto background2 = Node::create();
	background2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	auto sky1 = Sprite::create("graphics/cloud.png");
	sky1->setPosition(Vec2(0, 0));
	sky1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	sky1->getTexture()->setAliasTexParameters();

	auto sky2 = Sprite::create("graphics/cloud.png");
	sky2->setPosition(winSize.width, 0);
	sky2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	sky2->getTexture()->setAliasTexParameters();

	background2->addChild(sky1);
	background2->addChild(sky2);
	//_ฎฉท
	background2->runAction(
		RepeatForever::create(
		Sequence::create(
		MoveTo::create(32, (Vec2(-winSize.width, 0))),
		Place::create(Vec2::ZERO),
		NULL
		)));

	//wi๓
	this->addChild(background1);
	//wi_
	this->addChild(background2);


	logo = Sprite::create("graphics/mmm_logo.png");
	logo->setPosition(Vec2(194, 300));
	logo->runAction(MoveTo::create(1.0, Vec2(194, 160)));
	logo->getTexture()->setAliasTexParameters();
	logo->setScale(2.0f);
	this->addChild(logo);


	Sequence* textShow = Sequence::create(Hide::create(), DelayTime::create(1), Show::create(), NULL);

	Start = Sprite::create("graphics/titleguide.png");
	Start->setPosition(Vec2(194, 60));
	Start->getTexture()->setAliasTexParameters();
	Start->runAction(textShow);
	this->addChild(Start);

	auto keyboardListener = EventListenerKeyboard::create();

	//L[{[hชณ๊ฝฬstop๐ญึH
	keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event)
	{


		if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
		{
			Start->stopAllActions();
			logo->stopAllActions();

			Start->runAction(Blink::create(1,4));
			logo->setPosition(Vec2(194, 160));
			experimental::AudioEngine::stop(titleBGM);
			experimental::AudioEngine::play2d("sounds/se_cancel.mp3", false, 0.8f);
			Music::mainMusicID = -1;
			auto scene = MainScene::createSceneWithLevel(INITIAL_LEVEL);
			auto transition = TransitionFade::create(1.0f, scene);
			Director::getInstance()->replaceScene(transition);
		}

		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		{
			Director::getInstance()->end();
		}


	};

	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	this->scheduleUpdate();
	return true;

}

