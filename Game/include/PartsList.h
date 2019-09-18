#pragma once

class IBodyParts;

class PartsList
{
private:
	/// <summary>
	/// パーツのデータ
	/// </summary>
	struct PartsType
	{
		wchar_t name[64];				//名前
		IBodyParts* bodyPart = nullptr;	//元データ(これをコピーする)
	};
	static constexpr int BEAM_TYPE_MAXNUM = 4;
	static constexpr PartsType m_s_beamTypes[BEAM_TYPE_MAXNUM];

public:
	/// <summary>
	/// パーツを生成する
	/// </summary>
	std::unique_ptr<IBodyParts> CreateBodyPart(const wchar_t* partName);

};

