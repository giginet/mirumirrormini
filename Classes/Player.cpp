#include "Player.h"
#include "Stage.h"

USING_NS_CC;
const int FRAME_COUNT = 3;
Player::Player()
	:indexCheck(-1)
	, magicPosition(0,0)
{
}

void Player::update(float dt)
{

}

bool Player::init()
{
	if (!Sprite::initWithFile("graphics/luk_sprite.png"))
	{
		return false;
	}


	Point PBox[4]{Point(-4, -12), Point(-4, 0), Point(4, 0), Point(4, -12)};

	auto body = PhysicsBody::createPolygon(PBox,4);
	body->setRotationEnable(false);
	body->setVelocityLimit(30.0);
	//
	body->setCategoryBitmask(static_cast<int>(Stage::TileType::PLAYER));
	//BLOCKSとの接触判定をON
	body->setCollisionBitmask(static_cast<int>(Stage::TileType::BLOCKS));

	body->setContactTestBitmask(INT_MAX);

	this->setPhysicsBody(body);
	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	this->scheduleUpdate();
	return true;
}


Magic* Player::MirrorMethod()
{
	Size winSize = Director::getInstance()->getWinSize();
	magic = Magic::create();
	magic->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	float whiteRightScale = 384 * 2;
	float whiteLeftScale = -384 * 2;

	magic->setScaleY(240.0);

	if (rightFlag == true)
	{
		magic->setScaleX(whiteRightScale);
	}

	else if (rightFlag == false)
	{
		magic->setScaleX(whiteLeftScale);
	}

	return magic;
}

// 0~2のアニメーションを再生するメソッド(0通常、1上、2下)
//void型のPlayer::playAnimation(int index)関数を作成
void Player::playAnimation(int index)
{

	//もしも変数indexCheckと変数indexが等しかったら
	if (indexCheck == index) {
		//ここで関数の処理を終了する
		return;
	}

	// アニメーション用のタグを固定値で定義
	const int ACTION_TAG = 999; // 好きな数

	// 既にアニメーションが動いていたら止める
	// アニメーションには全て999のタグが付いてるはずなので、重複しないように止める
	this->stopActionByTag(ACTION_TAG);

	// cocos2d::Size型のframeSize変数で表示するかわずたんのスプライトのサイズを指定？
	//Size(frameSize.width, frameSize.height)にそれぞれ16.0を代入している。型はfloat
	auto frameSize = Size(16.0, 24.0);

	//？
	//スプライトのX座標0、Y座標0(左上)から、16x16を切り出す？
	//Rectはx,y,width,heightの4つをとれる。矩形という意味
	this->setTextureRect(Rect(0, 0, frameSize.width, frameSize.height));

	//SpriteFrame*というテンプレートでframesという配列を宣言
	Vector<SpriteFrame*> frames;

		//変数iを宣言、0で初期化しiがFRAME_COUNT未満である時、変数iに1を加算し{}内の処理を行いループ
		//iがFRAME_COUNT未満でなくなった時にループを抜ける
		for (int i = 0; i < FRAME_COUNT; ++i)
		{
			// indexの値によってy座標を変える
			//cocos2d::SpriteFrame型のポイント変数frameを宣言
			//画像kawaz_shooting.pngを読み、frameSize.widthにiをかけ2コマアニメーションのループ
			//indexに16(frameSize.heightの値)をかけ、アニメーションを切り替え
			//frameSize.widthとframeSize.heightで表示する画像の大きさを指定？
			auto frame = SpriteFrame::create("graphics/luk_sprite.png", Rect(frameSize.width * i, index * 24, frameSize.width, frameSize.height));

			//配列framesの終わりにframeの値を挿入する
			frames.pushBack(frame);
		}
		auto frame = SpriteFrame::create("graphics/luk_sprite.png", Rect(frameSize.width, index * 24, frameSize.width, frameSize.height));
		frames.pushBack(frame);

	if (index == 2)
	{
		frames.clear();
		auto frame = SpriteFrame::create("graphics/luk_sprite.png", Rect(16, 48, frameSize.width, frameSize.height));
		frames.pushBack(frame);
	}
	else if (index == 3)
	{
		frames.clear();
		auto frame = SpriteFrame::create("graphics/luk_sprite.png", Rect(32, 48, frameSize.width, frameSize.height));
		frames.pushBack(frame);
	}

	//cocos2d::Animation型のポインタ変数animationを宣言
	//配列framesを代入してるっぽいけどWithなんちゃらがよくわからない
	auto animation = Animation::createWithSpriteFrames(frames);
	if (index == 0)
	{
		animation->setDelayPerUnit(0.2);
	}
	else
	{
		animation->setDelayPerUnit(0.1);
	}
	//cocos2d::RepeatForever型のポインタ変数animateを宣言　なんか闇っぽい
	//Animate::create(animation)でanimationで宣言された2コマアニメを生成してるとおもう
	//RepeatForeverは生成したアニメーションをを無限に繰り返す
	auto animate = RepeatForever::create(Animate::create(animation));

	//ポインタ変数animateにACTION_TAGというタグを設定
	animate->setTag(ACTION_TAG);

	//animateというアクションを走らせる
	this->runAction(animate);

	//indexCheckにindexの値を代入する
	indexCheck = index;

}
