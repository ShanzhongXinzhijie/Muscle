#pragma once

class BGMPlayer :
	public IGameObject
{
public:
	BGMPlayer() {
		SetName(L"BGMPlayer");
	}

	void Play(std::wstring_view path);

private:
	std::wstring m_playingPath;
	std::unique_ptr<SuicideObj::CBGM> m_bgm;
};

