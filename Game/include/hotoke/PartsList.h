#pragma once

class IBodyPart;

/// <summary>
/// �p�[�c�̃f�[�^
/// </summary>
struct PartType
{
	//�p�[�c�쐬�֐�
	std::function<std::unique_ptr<IBodyPart>()> createPartFunc;

	wchar_t name[64];//���O
	//�p�[�c����
	//������@		
};

class PartsList
{
private:	
	static constexpr int PART_TYPE_MAXNUM = 1;
	static const PartType m_s_partTypes[PART_TYPE_MAXNUM];

public:
	/// <summary>
	/// �p�[�c�f�[�^���擾����
	/// </summary>
	const PartType& GetPartType(const wchar_t* partName);

	/// <summary>
	/// �p�[�c�𐶐�����
	/// </summary>
	std::unique_ptr<IBodyPart> CreateBodyPart(const wchar_t* partName);
};

