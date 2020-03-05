#pragma once
#include"IGamePad.h"
#include"AI.h"
#include"BodyPartsHeader.h"
#include"IFu.h"
#include"HUDFont.h"

class CDeathHotoke :
	public ILifeObject
{
public:
	//�R���X�g���N�^
	CDeathHotoke() {
		//�����p�̖��O�o�^
		SetName(L"CDeathHotoke");
		SetLockableObjectName(L"CDeathHotoke");
		//���b�N�I�����ꂽ�ۂɎQ�Ƃ����HP�ϐ���ݒ�
		SetHPRef(&m_hp);
	}
	CDeathHotoke(int playernum, IGamePad* ptrPad, bool isDrawHUD, HUDFont* ptrFont, std::unique_ptr<IAI> AI){
		//�����p�̖��O�o�^
		SetName(L"CDeathHotoke");
		SetLockableObjectName(L"CDeathHotoke");
		//���b�N�I�����ꂽ�ۂɎQ�Ƃ����HP�ϐ���ݒ�
		SetHPRef(&m_hp);		
		//������
		Init(playernum, ptrPad, isDrawHUD, ptrFont, std::move(AI));
	}

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="playernum">�v���C���[�ԍ�</param>
	/// <param name="ptrPad">�p�b�h�̃|�C���^</param>
	/// <param name="isDrawHUD">HUD��`�悷�邩</param>
	/// <param name="ptrFont">HUD�p�t�H���g�̃|�C���^</param>
	/// <param name="AI">AI</param>
	void Init(int playernum, IGamePad* ptrPad, bool isDrawHUD, HUDFont* ptrFont, std::unique_ptr<IAI> AI) {
		//������
		m_playerNum = playernum;
		m_ptrPad = ptrPad;
		m_isDrawHUD = isDrawHUD;
		m_ptrHUDFont = ptrFont;
		m_ai = std::move(AI);

		//�����ʒu
		SetPos(CVector3::AxisY()*2000.0f);
		if (m_playerNum == 0) {
			SetPos({ 0.0f, 1000.0f, -2400.0f*1.0f });
		}
		if (m_playerNum == 1) {
			SetPos({ 0.0f, 1000.0f, 2400.0f*1.0f });
			SetRot({ CVector3::AxisY(),CMath::PI });
		}
	}

	//�t�H���g��ݒ�
	void SetFonts(HUDFont* warningfont, HUDFont* japanesefont) {
		m_ptrWarningHUDFont = warningfont;
		m_ptrJapaneseHUDFont = japanesefont;
	}

	//�{�f�B�p�[�c�̎��
	enum enBodyParts {
		enHead, enArm, enWing, enLeg, enPartsNum,
	};
	//�p�[�c�ݒ�
	void SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part);
	//�p�[�c���擾
	const IBodyPart& GetBodyPart(enBodyParts partsType)const {
		return *m_parts[partsType].get();
	}

	//���W���瑫���܂ł̋�����ݒ�
	void SetToFootDistance(float distance) {
		m_toFootDistance = distance;
	}
	//�����J�����I�t�Z�b�g���W
	void SetFootCameraOffsetPos(const CVector3& offsetpos) {
		m_footCamOffsetPos = offsetpos;
	}
	//�Œ�J�������g�p���邩�̃t���O��ݒ�
	void SetUseFixedCamera(bool enable) {
		m_useFixedCamera = enable;
	}
	//�Œ�J�����̈ʒu��ݒ�
	void SetFixedCameraPoint(const CVector3& pos) {
		m_fixedCameraPos = pos;
	}

	//�U���l��ݒ�
	void SetShakePower(float shakePower) {
		m_shakePower = shakePower;
	}

	//����\���ǂ����ݒ�
	void SetIsControl(bool isControl) {
		m_isControl = isControl;
	}

	//IGameObject�֌W
	bool Start()override;
	void PreLoopUpdate()override;
	void PreUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void Pre3DRender(int)override;
	void HUDRender(int)override;

	//�ړ��ʂ�������
	void AddVelocity(const CVector3& vec) { m_veloxity += vec; }
	void AddLinearVelocity(const CVector3& vec) { m_linearVelocity += vec; }
	//�e�v�f�̐�Βl���傫�������Z�b�g
	void SetMaxLinearVelocity(const CVector3& vec){
		m_linearVelocity.x = abs(m_linearVelocity.x) > abs(vec.x) ? m_linearVelocity.x : vec.x;
		m_linearVelocity.y = abs(m_linearVelocity.y) > abs(vec.y) ? m_linearVelocity.y : vec.y;
		m_linearVelocity.z = abs(m_linearVelocity.z) > abs(vec.z) ? m_linearVelocity.z : vec.z;
	}
	//��]�ʂ�������
	void AddAngularVelocity(const CVector3& axis, float radAngle) { 
		m_angularVelocity = CQuaternion(axis,radAngle*GetRotatability()) * m_angularVelocity;
	}

	//��R����Z
	void MulDrag(float mulDrag) { m_drag[enNext] *= mulDrag; }
	void MulAngularDrag(float mulDrag) { m_angularDrag[enNext] *= mulDrag; }

	//��]���₷������Z
	void MulRotatability(float rotatability) { m_rotatability[enNext] *= rotatability; }

	//�s���ݒ�
	void SetIsImmortal(bool isImmortal) {
		m_isImmortal = isImmortal;
	}

	//���G��ԉ�
	void Invincible() {
		if (m_isInvincible) {
			return;
		}
		m_isInvincible = true;
		SetCollisionEnable(false);
		m_coreModel.GetSkinModel().FindMaterialSetting(
			[](MaterialSetting* mat) {
				mat->SetAlbedoScale({5.0f,5.0f,5.0f,1.0f});// { 1.0f, 0.01f, 0.08f, 0.9f });
			}
		);
	}

	//�_���[�W����������
	void Damage(const ReferenceCollision& ref, const CVector3& pos);

	//�^�[�Q�b�g�ʒu��ݒ�
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	//�^�[�Q�b�g�Ώېݒ�
	void SetTarget(LockableWrapper* target) { m_target = target; }
	//�����_��ݒ�
	void SetVanisingPoint(const CVector3& pos) { m_vanisingPoint = pos; }

	//HUD��\�����邩�ݒ�
	void SetIsDrawHUD(bool enable) { m_isDrawHUD = enable; }
	//�o�b�N�~���[��Ԃ��ݒ�
	void SetIsBackMirror(bool isMirror) { m_isBackMirror = isMirror; }

	//���w��\�����邩�ݒ�
	void SetIsDrawKouhai(bool isDraw) {
		m_isDrawKouhai = isDraw;
	}
	
	//�Q�b�^�[//

	//���W���瑫���܂ł̋���
	float GetToFootDistance()const {
		return m_toFootDistance;
	}
	//�����̍��W���擾
	CVector3 GetFootPos()const {
		return GetPos() + CVector3::Down()*GetToFootDistance();
	}
	//�����J�����I�t�Z�b�g���W
	const CVector3& GetFootCameraOffsetPos()const {
		return m_footCamOffsetPos;
	}
	//�Œ�J�������g�p���邩
	bool GetUseFixedCamera()const {
		return m_useFixedCamera;
	}
	//�Œ�J�����̈ʒu
	const CVector3& GetFixedCameraPoint()const {
		return m_fixedCameraPos;
	}

	//�U���l���擾
	float GetShakePower()const {
		return m_shakePower;
	}

	//���x���擾(���[�g��)
	float GetHeightMeter()const {
		return GetPos().y / METER;
	}

	//HP���擾
	float GetHP()const {
		return m_hp;
	}
	float GetHPMax()const {
		return HP_MAX;
	}
	//HP���擾(����)
	float GetHPPer()const {
		return m_hp / HP_MAX;
	}

	//�X�^�����Ă��邩?
	bool GetIsStun()const {
		return !(m_stunTimeSec < FLT_EPSILON);
	}
	//

	//�X�P�[�����擾
	[[nodiscard]] const CVector3& GetScale()const { 
		return m_scale;
	}
	//�ړ������x�N�g�����擾
	[[nodiscard]] CVector3 GetMove() const { 
		return GetPos() - m_posOld; 
	}
	//�x���V�e�B���擾
	[[nodiscard]] CVector3 GetTotalVelocity() const {
		return m_veloxity + m_linearVelocity;
	}
	//��R���擾
	[[nodiscard]] float GetDrag()const {
		return m_drag[enNow]; 
	}
	//��]���₷�����擾
	[[nodiscard]] float GetRotatability()const {
		return m_rotatability[enNow];
	}

	//�^�[�Q�b�g�ʒu���擾
	[[nodiscard]] const CVector3& GetTargetPos() const {
		return m_targetPos; 
	}
	//�^�[�Q�b�g���擾
	[[nodiscard]] const LockableWrapper* GetTarget() const {
		return m_target;
	}
	[[nodiscard]] LockableWrapper* GetTarget() {
		return m_target;
	}
	[[nodiscard]] const IFu* GetTargetFu() const { 
		if (m_target) {
			return m_target->GetFu();
		}
		else {
			return nullptr;
		}
	}
	[[nodiscard]] IFu* GetTargetFu() { 
		if (m_target) {
			return m_target->GetFu();
		}
		else {
			return nullptr;
		}
	}
	//�����_���擾
	[[nodiscard]] const CVector3& GetVanisingPoint() const { return m_vanisingPoint; }

	//�v���C���[�ԍ��̎擾
	[[nodiscard]] int GetPlayerNum() { return m_playerNum; }

	//�p�b�h�̎擾
	[[nodiscard]] const IGamePad* GetPad()const { return m_ptrPad; }

	//�t�H���g�̎擾
	[[nodiscard]] HUDFont* GetFont()const { return m_ptrHUDFont; }
	[[nodiscard]] HUDFont* GetWarningFont()const{ return m_ptrWarningHUDFont ? m_ptrWarningHUDFont : m_ptrHUDFont; }
	[[nodiscard]] HUDFont* GetJapaneseFont()const { return m_ptrJapaneseHUDFont ? m_ptrJapaneseHUDFont : m_ptrHUDFont; }

	//HUD��\�����邩�擾
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }
	//�o�b�N�~���[��Ԃ��ݒ�
	[[nodiscard]] bool GetIsBackMirror()const { return m_isBackMirror; }

	//AI�̐��ݏo���X�e�[�^�X���擾
	[[nodiscard]] const AIStatus* GetAIStatus()const { if (m_ai) { return &m_ai->GetOutputStatus(); } return nullptr; }

	//�Y�[���A�E�g�������擾
	const CVector3& GetZoomoutDirection()const {
		return m_zoomoutDir;
	}

	//�������x���擾
	[[nodiscard]] constexpr float GetGravity()const { return GRAVITY; }

	//����\���擾
	[[nodiscard]] bool GetIsControl()const {
		return m_isControl;
	}

private:
	//�X�^������
	void Stun();

private:
	//�d�͒萔
	static constexpr float GRAVITY = ONE_G;

	//�R�A�̃��f��
	GameObj::CSkinModelRender m_coreModel;
	//���w
	CBillboard m_kouhai;
	bool m_isDrawKouhai = true;
	
	//�X�P�[��
	CVector3 m_scale;

	//�����ւ̋���
	float m_toFootDistance = 205.0f;
	//�����J����
	CVector3 m_footCamOffsetPos;
	//�Œ�J����
	bool m_useFixedCamera = false;	//�g����
	CVector3 m_fixedCameraPos;		//�ʒu

	//�U���l
	float m_shakePower = 0.0f;

	//�O�t���[���̈ʒu
	CVector3 m_posOld;
	
	//�ړ���
	CVector3 m_veloxity, m_linearVelocity;
	CQuaternion m_angularVelocity;

	//�X�e�[�^�X
	bool m_isImmortal = false;//�s����
	bool m_isInvincible = false;//���G���
	static constexpr float HP_MAX = 20.0f;
	float m_hp = HP_MAX;//�w���X
	float m_stunTimeSec = 0.0f;//�X�^������
	enum{enNow,enNext};
	float m_drag[2] = { 1.0f,1.0f };//��R
	float m_angularDrag[2] = { 1.0f,1.0f };//��]��R
	float m_rotatability[2] = { 1.0f,1.0f };//��]���₷��

	//�p�[�c
	std::unique_ptr<IBodyPart> m_parts[4];

	//�v���C���[�ԍ�
	int m_playerNum = -1;
	//�Q�[���p�b�h
	IGamePad* m_ptrPad = nullptr;
	//AI
	std::unique_ptr<IAI> m_ai;
		
	//HUD
	bool m_isDrawHUD = false;//HUD��\�����邩
	HUDFont* m_ptrHUDFont = nullptr, *m_ptrWarningHUDFont = nullptr, *m_ptrJapaneseHUDFont = nullptr;//HUD�t�H���g
	bool m_isBackMirror = false;//�o�b�N�~���[���

	//�^�[�Q�b�g
	LockableWrapper* m_target = nullptr;
	CVector3 m_targetPos;
	CVector3 m_vanisingPoint;

	//�Y�[���A�E�g����
	CVector3 m_zoomoutDir = { 0.f, 400.f, 800.f };

	//����\��?
	bool m_isControl = false;
};

