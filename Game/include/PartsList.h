#pragma once

class IBodyParts;

class PartsList
{
private:
	/// <summary>
	/// �p�[�c�̃f�[�^
	/// </summary>
	struct PartsType
	{
		wchar_t name[64];				//���O
		IBodyParts* bodyPart = nullptr;	//���f�[�^(������R�s�[����)
	};
	static constexpr int BEAM_TYPE_MAXNUM = 4;
	static constexpr PartsType m_s_beamTypes[BEAM_TYPE_MAXNUM];

public:
	/// <summary>
	/// �p�[�c�𐶐�����
	/// </summary>
	std::unique_ptr<IBodyParts> CreateBodyPart(const wchar_t* partName);

};

