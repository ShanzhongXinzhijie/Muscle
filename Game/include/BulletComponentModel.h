#pragma once
#include"BeamModel.h"
#include"CSmoke.h"
#include"BulletKani.h"

/// <summary>
/// ビームモデルコンポーネント
/// ※画面分割数を変更した場合作り直す必要がある
/// </summary>
class BD_BeamModel : public IBulletComponent {
public:
	BD_BeamModel(float radius = 3.0f, std::wstring_view beamName = L"BLUE") :m_radius(radius) {
		//モデル
		m_modelNum = GetScreenNum();
		m_model = std::make_unique<BeamModel[]>(m_modelNum);//画面の数だけ作成
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].Init(beamName.data());
			m_model[i].SetRadius(radius);
			m_model[i].SetIsDraw(false);
		}
	}
	void Start()override {
		//座標初期化
		m_lastDrawPos = m_bullet->GetPos();
		//攻撃判定作成
		m_bullet->m_col.m_collision.CreateSphere(m_bullet->GetPos(), {}, m_radius);
		m_bullet->m_radius = m_radius;
	}
	void PreLoopUpdate()override {
		//最後に描画した座標更新
		m_lastDrawPos = m_bullet->GetPos();
		//モデル表示初期化
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].SetIsDraw(true);
		}
	}
	void PostLoopUpdate()override {
		//モデルの更新(画面数分)
		for (int i = 0; i < m_modelNum; i++) {
			CVector3 rootPos, tipPos;
			GameObj::ICamera* screenCamera = m_modelNum == 1 ? GetMainCamera() : ViewCameraList()[i];

			//カメラと相対的な座標を求める(お尻側)
			CVector3 soutaiPos = m_lastDrawPos;
			CMatrix viewMat = screenCamera->GetViewMatrixOld();
			viewMat.Mul(soutaiPos);
			viewMat = screenCamera->GetViewMatrix();
			viewMat.Inverse();
			viewMat.Mul(soutaiPos);
			rootPos = soutaiPos;

			//先端側の座標求める
			tipPos = m_lastDrawPos + (m_bullet->GetPos() - m_lastDrawPos);

			//blurScaleを適応
			constexpr float blurScale = 1.0f;
			rootPos = tipPos + (rootPos - tipPos)*blurScale;

			//モデル更新
			m_model[i].SetPos(rootPos, tipPos);
		}
	}
	void Pre3DRender(int screenNum)override {
		//表示するモデルの切り替え
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].SetIsDraw(i == screenNum ? true : false);
		}
	}
private:
	int m_modelNum = 0;//モデルの数
	std::unique_ptr<BeamModel[]> m_model;//モデル
	float m_radius = 3.0f;//半径
	CVector3 m_lastDrawPos;//最後に描画した座標
};


/// <summary>
/// 脚モデルコンポーネント
/// </summary>
class BD_LegModel : public IBulletComponent {
public:
	//scaleにはホトケのスケールを入れればいいと思うよ
	BD_LegModel(const CQuaternion& rot, const CVector3& scale) : m_rot(rot), m_scale(scale*5.0f) {}

	void Start()override {
		//モデル作成
		if (CMath::RandomZeroToOne() < 1.0f / 5.0f) {
			//まれにうんちに
			m_model.Init(L"Resource/modelData/unchi.cmo");
			//ノーマルマップ適用
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
			TextureFactory::GetInstance().Load(L"Resource/normalMap/unchi_n.jpg", nullptr, &textureView);// , nullptr, true);
			m_model.GetSkinModel().FindMaterialSetting(
				[&](MaterialSetting* mat) {
					mat->SetNormalTexture(textureView.Get());
					mat->SetShininess(0.5f);
				}
			);
		}
		else {
			m_model.Init(L"Resource/modelData/leg.cmo");
		}
		m_model.SetPos(m_bullet->GetPos());
		m_model.SetRot(m_rot);
		m_model.SetScale(m_scale);
		//攻撃判定作成
		float radius = 50.0f*((m_scale.GetMin() + m_scale.GetMax())*0.5f);
		m_bullet->m_col.m_collision.CreateSphere(m_bullet->GetPos(), {}, radius);
		m_bullet->m_radius = radius;
	}
	void PostLoopUpdate()override {
		//モデルの更新
		m_model.SetPos(m_bullet->GetPos());
	}

private:
	CQuaternion m_rot;
	CVector3 m_scale = 1.0f;
	GameObj::CSkinModelRender m_model;//モデル
};

/// <summary>
/// 煙軌跡コンポーネント
/// </summary>
class BD_SmokeTrail : public IBulletComponent {
public:
	void PostUpdate()override {
		new CSmoke(m_bullet->GetPos(), CVector3::Zero(),
			{ 0.9f,0.88f,0.88f,0.65f },
			100.0f, 1.02f,//1.05f + 0.05f*(0.5f + CMath::RandomZeroToOne()),
			360
		);
	}

private:
};
