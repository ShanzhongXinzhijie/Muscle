#pragma once
static constexpr float METER = 16.45f / 1.65f;//1メートル
static constexpr float ONE_G = 10.0f;//1G

static constexpr float FRAME_PER_SECOND = 1.0f / 60.0f;//1fの秒数(60fps)
static constexpr int FRAME_RATE = 60;//動作フレームレート

static constexpr int PLAYER_NUM = 2;//プレイヤー数

static constexpr float MAX_HEIGHT_METER = 400.0f;//限界高度(m)
static constexpr float LIMIT_HEIGHT_METER = 250.0f;//最高高度(m)

static constexpr float MIN_Y = -3000.0f;//最低高さ

//空気の濃さ取得
static inline constexpr float CalcAirScale(float heightMeter) {
	return max(0.0f, 1.0f - max(0.0f, heightMeter / MAX_HEIGHT_METER));//高度MAX_HEIGHT_METERに近づくに連れ空気が薄くなる
}

//血の色
static inline const CVector4 BLOOD_COLOR = { 95.3f / 100.0f,28.6f / 100.0f,24.3f / 100.0f,1.0f };
static inline const CVector4 BLOOD_BLACK_COLOR = { 0.8f, 0.0f, 0.005f, 1.0f };

//解像度
static constexpr int SCREEN_SIZE_2D = 640;
static constexpr int SCREEN_SIZE_3D = 640*2;//1280
static constexpr int SCREEN_SPRITE_BAR = 0;//64
static constexpr int WINDOW_SIZE = 640;