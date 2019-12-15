#pragma once

/// <summary>
/// HUDの文字を描画する
/// </summary>
class HUDFont
{
public:
	HUDFont(const CVector4& color = CVector4::One(), const CVector2& scale = 1.0f) : m_color(color), m_scale(scale) {
		m_font.LoadFont(L"Resource/font/eunomia_0200/Eunomia.spritefont");
	}

	/// <summary>
	/// 透明度を設定
	/// </summary>
	void SetAlpha(float alpha) { m_color.w = alpha; }

	/// <summary>
	/// スケールを設定
	/// </summary>
	void SetScale(const CVector2& scale) { m_scale = scale; }
	const CVector2& GetScale() { return m_scale; }

	/// <summary>
	/// 色を取得
	/// </summary>
	const CVector4& GetColor()const { return m_color; }

	/// <summary>
	/// 文字列描画
	/// </summary>
	/// <param name="string">文字列</param>
	void Draw(std::wstring_view string, CVector2 pos, CVector2 pivot = {0.0f,0.0f})const {
		m_font.Draw(string.data(), pos, m_color, m_scale, pivot);
	}

	/// <summary>
	/// 書式を使って文字列描画
	/// </summary>
	/// <param name="string">文字列</param>
	/// <param name="args">可変長引数</param>
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

