#pragma once

/// <summary>
/// HUD�̕�����`�悷��
/// </summary>
class HUDFont
{
public:
	HUDFont(const CVector4& color = CVector4::One(), const CVector2& scale = 1.0f) : m_color(color), m_scale(scale) {
		m_font.LoadFont(L"Resource/font/eunomia_0200/Eunomia.spritefont");
	}

	/// <summary>
	/// �����x��ݒ�
	/// </summary>
	void SetAlpha(float alpha) { m_color.w = alpha; }

	/// <summary>
	/// ������`��
	/// </summary>
	/// <param name="string">������</param>
	void Draw(std::wstring_view string, CVector2 pos, CVector2 pivot = {0.0f,0.0f})const {
		m_font.Draw(string.data(), pos, m_color, m_scale, pivot);
	}

	/// <summary>
	/// �������g���ĕ�����`��
	/// </summary>
	/// <param name="string">������</param>
	/// <param name="args">�ϒ�����</param>
	template <class... Args>
	void DrawFormat(std::wstring_view string, CVector2 pos, CVector2 pivot, Args... args)const {
		wchar_t out[256];
		swprintf_s(out, 256, string.data(), args...);
		
		Draw(out,pos,pivot);
	}

private:
	CFont m_font;
	CVector4 m_color;
	CVector2 m_scale = 1.0f;
};

