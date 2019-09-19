#pragma once

class IBodyPart;

/// <summary>
/// パーツのデータ
/// </summary>
struct PartType
{
	//パーツ作成関数
	std::function<std::unique_ptr<IBodyPart>()> createPartFunc;

	wchar_t name[64];//名前
	//パーツ説明
	//操作方法		
};

class PartsList
{
private:	
	static constexpr int PART_TYPE_MAXNUM = 1;
	static const PartType m_s_partTypes[PART_TYPE_MAXNUM];

public:
	/// <summary>
	/// パーツデータを取得する
	/// </summary>
	const PartType& GetPartType(const wchar_t* partName);

	/// <summary>
	/// パーツを生成する
	/// </summary>
	std::unique_ptr<IBodyPart> CreateBodyPart(const wchar_t* partName);
};

