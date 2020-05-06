#include "stdafx.h"
#include "BGMPlayer.h"

void BGMPlayer::Play(std::wstring_view path) {

	if (m_playingPath == path) {
		return;//すでに再生中
	}

	//BGM作成
	m_bgm = std::make_unique<SuicideObj::CBGM>(path.data());
	//自動で削除しない
	m_bgm->SetIsAutoDelete(false);
	//ループで再生
	m_bgm->Play(false, true); 

	//再生中のパスを保存
	m_playingPath = path;

	//bgm->Stop(); //再生を止める(その後削除される)
	//bgm->Pause(); //一時停止。Play()で再生再開(引数は意味ない)
}