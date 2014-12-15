#include "text.hpp"
#include <stdio.h>
#include <wchar.h>

using namespace std;

static const wchar_t *g_whitespaces = L" \f\n\r\t\v";

bool SFont::fromBuffer(const u8 *buffer, u32 bufferSize, u32 size, u32 lspacing)
{
	size = min(max(6u, size), 1000u);
	lspacing = min(max(6u, lspacing), 1000u);
	lineSpacing = lspacing;
	font.release();
	data.release();
	dataSize = 0;
	font = SmartPtr<FreeTypeGX>(new FreeTypeGX);
	if (!font)
		return false;
	font->loadFont(buffer, bufferSize, size, false);
	return true;
}

bool SFont::newSize(u32 size, u32 lspacing)
{
	if (!data)
		return false;
	size = min(max(6u, size), 1000u);
	lspacing = min(max(6u, lspacing), 1000u);
	lineSpacing = lspacing;
	font.release();
	font = SmartPtr<FreeTypeGX>(new FreeTypeGX);
	if (!font)
		return false;
	font->loadFont(data.get(), dataSize, size, false);
	return true;
}

bool SFont::fromFile(const char *filename, u32 size, u32 lspacing)
{
	FILE *file;
	u32 fileSize;

	size = min(max(6u, size), 1000u);
	lspacing = min(max(6u, lspacing), 1000u);
	font.release();
	data.release();
	dataSize = 0;
	lineSpacing = lspacing;
	file = fopen(filename, "rb");
	if (file == NULL)
		return false;
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (fileSize == 0)
		return false;
	data = smartMem2Alloc(fileSize);	// Use MEM2 because of big chinese fonts
	if (!!data)
		fread(data.get(), 1, fileSize, file);
	fclose(file);
	file = NULL;
	if (!data)
		return false;
	dataSize = fileSize;
	font = SmartPtr<FreeTypeGX>(new FreeTypeGX);
	if (!font)
	{
		data.release();
		return false;
	}
	font->loadFont(data.get(), dataSize, size, false);
	return true;
}

void CText::setText(SFont font, const wstringEx &t)
{
	CText::SWord w;
	vector<wstringEx> lines;

	m_lines.clear();
	m_font = font;
	if (!m_font.font)
		return;
	// Don't care about performance
	lines = stringToVector(t, L'\n');
	m_lines.reserve(lines.size());
	// 
	for (u32 k = 0; k < lines.size(); ++k)
	{
		wstringEx &l = lines[k];
		m_lines.push_back(CText::CLine());
		m_lines.back().reserve(32);
		wstringEx::size_type i = l.find_first_not_of(g_whitespaces);
		wstringEx::size_type j;
		while (i != wstringEx::npos)
		{
			j = l.find_first_of(g_whitespaces, i);
			if (j != wstringEx::npos && j > i)
			{
				w.text.assign(l, i, j - i);
				m_lines.back().push_back(w);
				i = l.find_first_not_of(g_whitespaces, j);
			}
			else if (j == wstringEx::npos)
			{
				w.text.assign(l, i, l.size() - i);
				m_lines.back().push_back(w);
				i = wstringEx::npos;
			}
		}
	}
}

void CText::setFrame(float width, u16 style, bool ignoreNewlines, bool instant)
{
	float posX;
	float posY;
	float wordWidth;
	float space;
	float shift;
	u32 lineBeg;

	if (!m_font.font)
		return;
	space = m_font.font->getWidth(L" ");
	posX = 0.f;
	posY = 0.f;
	lineBeg = 0;
	for (u32 k = 0; k < m_lines.size(); ++k)
	{
		CText::CLine &words = m_lines[k];
		if (words.empty())
			posY += (float)m_font.lineSpacing;
		for (u32 i = 0; i < words.size(); ++i)
		{
			wordWidth = m_font.font->getWidth(words[i].text.c_str());
			if (posX == 0.f || posX + (float)wordWidth <= width)
			{
				words[i].targetPos = Vector3D(posX, posY, 0.f);
				posX += wordWidth + space;
			}
			else
			{
				posY += (float)m_font.lineSpacing;
				words[i].targetPos = Vector3D(0.f, posY, 0.f);
				if ((style & (FTGX_JUSTIFY_CENTER | FTGX_JUSTIFY_RIGHT)) != 0)
				{
					posX -= space;
					shift = (style & FTGX_JUSTIFY_CENTER) != 0 ? -posX * 0.5f : -posX;
					for (u32 j = lineBeg; j < i; ++j)
						words[j].targetPos.x += shift;
				}
				posX = wordWidth + space;
				lineBeg = i;
			}
		}
		// Quick patch for newline support
		if (!ignoreNewlines && k + 1 < m_lines.size())
			posX = 9999999.f;
	}
	if ((style & (FTGX_JUSTIFY_CENTER | FTGX_JUSTIFY_RIGHT)) != 0)
	{
		posX -= space;
		shift = (style & FTGX_JUSTIFY_CENTER) != 0 ? -posX * 0.5f : -posX;
		for (u32 k = 0; k < m_lines.size(); ++k)
			for (u32 j = lineBeg; j < m_lines[k].size(); ++j)
				m_lines[k][j].targetPos.x += shift;
	}
	if ((style & (FTGX_ALIGN_MIDDLE | FTGX_ALIGN_BOTTOM)) != 0)
	{
		posY += (float)m_font.lineSpacing;
		shift = (style & FTGX_ALIGN_MIDDLE) != 0 ? -posY * 0.5f : -posY;
		for (u32 k = 0; k < m_lines.size(); ++k)
			for (u32 j = 0; j < m_lines[k].size(); ++j)
				m_lines[k][j].targetPos.y += shift;
	}
	if (instant)
		for (u32 k = 0; k < m_lines.size(); ++k)
			for (u32 i = 0; i < m_lines[k].size(); ++i)
				m_lines[k][i].pos = m_lines[k][i].targetPos;
}

void CText::setColor(const CColor &c)
{
	m_color = c;
}

void CText::tick(float dt)
{
	for (u32 k = 0; k < m_lines.size(); ++k)
	{
		for (u32 i = 0; i < m_lines[k].size(); ++i)
		{
			m_lines[k][i].pos += (m_lines[k][i].targetPos - m_lines[k][i].pos) * 2.5f * dt;
		}
	}
}

void CText::draw(void)
{
	if (!m_font.font)
		return;
	for (u32 k = 0; k < m_lines.size(); ++k)
	{
		for (u32 i = 0; i < m_lines[k].size(); ++i)
		{
			m_font.font->setX(m_lines[k][i].pos.x);
			m_font.font->setY(m_lines[k][i].pos.y);
			m_font.font->drawText(0, m_font.lineSpacing, m_lines[k][i].text.c_str(), m_color);
		}
	}
}
