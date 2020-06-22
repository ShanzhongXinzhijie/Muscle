#pragma once
inline constexpr float METER = 16.45f / 1.65f;//1メートル
inline constexpr float ONE_G = 10.0f;//1G

inline constexpr float FRAME_PER_SECOND = 1.0f / 60.0f;//1fの秒数(60fps)
inline constexpr int FRAME_RATE = 60;//動作フレームレート

inline constexpr int PLAYER_NUM = 2;//プレイヤー数

inline constexpr float MAX_HEIGHT_METER = 400.0f;//限界高度(m)
inline constexpr float LIMIT_HEIGHT_METER = 250.0f;//最高高度(m)

inline constexpr float MIN_Y = -3000.0f;//最低高さ

//空気の濃さ取得
inline constexpr float CalcAirScale(float heightMeter) {
	return max(0.0f, 1.0f - max(0.0f, heightMeter / MAX_HEIGHT_METER));//高度MAX_HEIGHT_METERに近づくに連れ空気が薄くなる
}

//血の色
inline const CVector4 BLOOD_COLOR = { 95.3f / 100.0f,28.6f / 100.0f,24.3f / 100.0f,1.0f };
inline const CVector4 BLOOD_BLACK_COLOR = { 0.8f, 0.0f, 0.005f, 1.0f };

//解像度
inline constexpr int SCREEN_SIZE_2D = 640;
inline constexpr int SCREEN_SIZE_3D = 640;
inline constexpr int SCREEN_SPRITE_BAR = 0;//64
inline constexpr int WINDOW_SIZE = 640;