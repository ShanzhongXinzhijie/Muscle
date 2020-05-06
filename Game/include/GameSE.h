#pragma once

class CGameMode;
class Game;

/// <summary>
/// ゲーム効果音
/// </summary>
class GameSE : private SuicideObj::CSE
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="fileName">wavファイルパス</param>
	/// <param name="pos">再生位置</param>
	/// <param name="distanceMeter">再生距離(m)</param>
	/// <param name="screenNum">再生スクリーン番号</param>
	/// <param name="isLoop">ループするか</param>
	/// <param name="mute">ミュートするか</param>
	GameSE(const wchar_t* fileName, const CVector3& pos, float distanceMeter, int screenNum, bool isLoop = false, bool mute = false);
	
	/// <summary>
	/// 再生終了後自殺するか設定
	/// </summary>
	void SetIsAutoDelete(bool isAutoDel) {
		SuicideObj::CSE::SetIsAutoDelete(isAutoDel);
	}

	/// <summary>
	/// 再生位置設定
	/// </summary>
	void SetPos(const CVector3& pos) {
		SuicideObj::CSE::SetPos(pos);
	}

	/// <summary>
	/// 音量設定
	/// </summary>
	void SetVol(float vol) {
		SuicideObj::CSE::SetVolume(vol);
	}

	/// <summary>
	/// 再生中か取得
	/// </summary>
	bool GetIsPlaying()const { 
		return SuicideObj::CSE::GetIsPlaying();
	}

	/// <summary>
	/// ゲームモードへの参照を設定
	/// </summary>
	static void SetGameModePtr(const CGameMode* gamemode) {
		m_s_gameMode = gamemode;
	}
	/// <summary>
	/// ゲームへの参照を設定
	/// </summary>
	static void SetGamePtr(const Game* game) {
		m_s_game = game;
	}

private:
	bool m_is3D = false;

	static inline const CGameMode* m_s_gameMode = nullptr;
	static inline const Game* m_s_game = nullptr;
};

