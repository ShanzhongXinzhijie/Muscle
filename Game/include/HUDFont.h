#pragma once

/// <summary>
/// HUD‚Ì•¶š‚ğ•`‰æ‚·‚é
/// </summary>
class HUDFont
{
public:
	HUDFont(const CVector4& color = CVector4::One(), const CVector2& scale = 1.0f) : m_color(color), m_scale(scale) {
		m_font.LoadFont(L"Resource/font/eunomia_0200/Eunomia.spritefont");
	}

	/// <summary>
	/// “§–¾“x‚ğİ’è
	/// </summary>
	void SetAlpha(float alpha) { m_color.w = alpha; }

	/// <summary>
	/// •¶š—ñ•`‰æ
	/// </summary>
	/// <param name="string">•¶š—ñ</param>
	void Draw(std::wstring_view string, CVector2 pos, CVector2 pivot = {0.0f,0.0f})const {
		m_font.Draw(string.data(), pos, m_color, m_scale, pivot);
	}

	/// <summary>
	/// ‘®‚ğg‚Á‚Ä•¶š—ñ•`‰æ
	/// </summary>
	/// <param name="string">•¶š—ñ</param>
	/// <param name="args">‰Â•Ï’·ˆø”</param>
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

