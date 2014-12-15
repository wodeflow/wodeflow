
#include "menu.hpp"
#include "wbfs.h"

#include <dirent.h>
#include <wiiuse/wpad.h>
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/stat.h> 
#include <unistd.h> 

using namespace std;

static const int g_curPage = 2;

template <class T> static inline T loopNum(T i, T s)
{
	return (i + s) % s;
}

void CMenu::_hideConfigAdv(bool instant)
{
	m_btnMgr.hide(m_configLblTitle, instant);
	m_btnMgr.hide(m_configBtnBack, instant);
	m_btnMgr.hide(m_configLblPage, instant);
	m_btnMgr.hide(m_configBtnPageM, instant);
	m_btnMgr.hide(m_configBtnPageP, instant);
	// 
	m_btnMgr.hide(m_configAdvLblInstall, instant);
	m_btnMgr.hide(m_configAdvBtnInstall, instant);
	m_btnMgr.hide(m_configAdvLblTheme, instant);
	m_btnMgr.hide(m_configAdvLblCurTheme, instant);
	m_btnMgr.hide(m_configAdvBtnCurThemeM, instant);
	m_btnMgr.hide(m_configAdvBtnCurThemeP, instant);
	m_btnMgr.hide(m_configAdvLblLanguage, instant);
	m_btnMgr.hide(m_configAdvLblCurLanguage, instant);
	m_btnMgr.hide(m_configAdvBtnCurLanguageM, instant);
	m_btnMgr.hide(m_configAdvBtnCurLanguageP, instant);
	m_btnMgr.hide(m_configAdvLblCFTheme, instant);
	m_btnMgr.hide(m_configAdvBtnCFTheme, instant);
	for (u32 i = 0; i < ARRAY_SIZE(m_configAdvLblUser); ++i)
		if (m_configAdvLblUser[i] != -1u)
			m_btnMgr.hide(m_configAdvLblUser[i], instant);
}

void CMenu::_showConfigAdv(void)
{
	_setBg(m_configAdvBg, m_configAdvBg);
	m_btnMgr.show(m_configLblTitle);
	m_btnMgr.show(m_configBtnBack);
	m_btnMgr.show(m_configLblPage);
	m_btnMgr.show(m_configBtnPageM);
	m_btnMgr.show(m_configBtnPageP);
	// 
	m_btnMgr.show(m_configAdvLblCurTheme);
	m_btnMgr.show(m_configAdvBtnCurThemeM);
	m_btnMgr.show(m_configAdvBtnCurThemeP);
	m_btnMgr.show(m_configAdvLblTheme);
	if( !m_locked )
	{
		m_btnMgr.show(m_configAdvLblInstall);
		m_btnMgr.show(m_configAdvBtnInstall);
		m_btnMgr.show(m_configAdvLblLanguage);
		m_btnMgr.show(m_configAdvLblCurLanguage);
		m_btnMgr.show(m_configAdvBtnCurLanguageM);
		m_btnMgr.show(m_configAdvBtnCurLanguageP);
		m_btnMgr.show(m_configAdvLblCFTheme);
		m_btnMgr.show(m_configAdvBtnCFTheme);
	}
	for (u32 i = 0; i < ARRAY_SIZE(m_configAdvLblUser); ++i)
		if (m_configAdvLblUser[i] != -1u)
			m_btnMgr.show(m_configAdvLblUser[i]);
	// 
	m_btnMgr.setText(m_configLblPage, wfmt(L"%i / %i", g_curPage, m_locked ? g_curPage : CMenu::_nbCfgPages));
	m_btnMgr.setText(m_configAdvLblCurLanguage, m_curLanguage);
	m_btnMgr.setText(m_configAdvLblCurTheme, m_cfg.getString(" GENERAL", "theme"));
}

static const char upper_case_lookup[0xFF] = {
	'.', //0
	'.', //1
	'.', //2
	'.', //3
	'.', //4
	'.', //5
	'.', //6
	'.', //7
	'.', //8
	'.', //9
	'.', //10
	'.', //11
	'.', //12
	'.', //13
	'.', //14
	'.', //15
	'.', //16
	'.', //17
	'.', //18
	'.', //19
	'.', //20
	'.', //21
	'.', //22
	'.', //23
	'.', //24
	'.', //25
	'.', //26
	'.', //27
	'.', //28
	'.', //29
	'.', //30
	'.', //31
	' ', //
	'!', //!
	'"', //"
	'#', //#
	'$', //$
	'%', //%
	'&', //&
	'\'',//'
	'(', //(
	')', //)
	'*', //*
	'+', // +
	',', //,
	'-', // -
	'.', //.
	'/', // /
	'0', //0
	'1', //1
	'2', //2
	'3', //3
	'4', //4
	'5', //5
	'6', //6
	'7', //7
	'8', //8
	'9', //9
	':', //:
	';', //;
	'<', //<
	'=', // =
	'>', //>
	'?', // ?
	'@', //@
	'A', //A
	'B', //B
	'C', //C
	'D', //D
	'E', //E
	'F', //F
	'G', //G
	'H', //H
	'I', //I
	'J', //J
	'K', //K
	'L', //L
	'M', //M
	'N', //N
	'O', //O
	'P', //P
	'Q', //Q
	'R', //R
	'S', //S
	'T', //T
	'U', //U
	'V', //V
	'W', //W
	'X', //X
	'Y', //Y
	'Z', //Z
	'[', //[
	'\\', //
	']', //]
	'^', //^
	'_', //_
	'`', //`
	'A', //a
	'B', //b
	'C', //c
	'D', //d
	'E', //e
	'F', //f
	'G', //g
	'H', //h
	'I', //i
	'J', //j
	'K', //k
	'L', //l
	'M', //m
	'N', //n
	'O', //o
	'P', //p
	'Q', //q
	'R', //r
	'S', //s
	'T', //t
	'U', //u
	'V', //v
	'W', //w
	'X', //x
	'Y', //y
	'Z', //z
	'{', //{
	'|', // |
	'}', //}
	'~', //~
	'.', //127
	'.', //128
	'.', //129
	'.', //130
	'.', //131
	'.', //132
	'.', //133
	'.', //134
	'.', //135
	'.', //136
	'.', //137
	'.', //138
	'.', //139
	'.', //140
	'.', //141
	'.', //142
	'.', //143
	'.', //144
	'.', //145
	'.', //146
	'.', //147
	'.', //148
	'.', //149
	'.', //150
	'.', //151
	'.', //152
	'.', //153
	'.', //154
	'.', //155
	'.', //156
	'.', //157
	'.', //158
	'.', //159
	'.', //160
	'.', //161
	'.', //162
	'.', //163
	'.', //164
	'.', //165
	'.', //166
	'.', //167
	'.', //168
	'.', //169
	'.', //170
	'.', //171
	'.', //172
	'.', //173
	'.', //174
	'.', //175
	'.', //176
	'.', //177
	'.', //178
	'.', //179
	'.', //180
	'.', //181
	'.', //182
	'.', //183
	'.', //184
	'.', //185
	'.', //186
	'.', //187
	'.', //188
	'.', //189
	'.', //190
	'.', //191
	'.', //192
	'.', //193
	'.', //194
	'.', //195
	'.', //196
	'.', //197
	'.', //198
	'.', //199
	'.', //200
	'.', //201
	'.', //202
	'.', //203
	'.', //204
	'.', //205
	'.', //206
	'.', //207
	'.', //208
	'.', //209
	'.', //210
	'.', //211
	'.', //212
	'.', //213
	'.', //214
	'.', //215
	'.', //216
	'.', //217
	'.', //218
	'.', //219
	'.', //220
	'.', //221
	'.', //222
	'.', //223
	'.', //224
	'.', //225
	'.', //226
	'.', //227
	'.', //228
	'.', //229
	'.', //230
	'.', //231
	'.', //232
	'.', //233
	'.', //234
	'.', //235
	'.', //236
	'.', //237
	'.', //238
	'.', //239
	'.', //240
	'.', //241
	'.', //242
	'.', //243
	'.', //244
	'.', //245
	'.', //246
	'.', //247
	'.', //248
	'.', //249
	'.', //250
	'.', //251
	'.', //252
	'.', //253
	'.', //254
};

static std::string &upperCase(const string& text_x)
{
	std::string& text = (std::string&)text_x;

	for (std::size_t i = 0; i < text.size(); ++i)
	{
		text[i] = upper_case_lookup[(uint8_t)text[i]];
	}

	return text;
}

static void listThemes(const char * path, vector<string> &themes)
{
	DIR *d;
	struct dirent *dir;
	string fileName;
	bool def = false;

	themes.clear();
	d = opendir(path);
	if (d != 0)
	{
		dir = readdir(d);
		while (dir != 0)
		{
			fileName = upperCase(dir->d_name);
			def = def || fileName == "DEFAULT.INI";
			if (fileName.size() > 4 && fileName.substr(fileName.size() - 4, 4) == ".INI")
				themes.push_back(fileName.substr(0, fileName.size() - 4));
			dir = readdir(d);
		}
		closedir(d);
	}
	if (!def)
		themes.push_back("DEFAULT");
	sort(themes.begin(), themes.end());
}

int CMenu::_configAdv(void)
{
	s32 padsState;
	WPADData *wd;
	int nextPage = 0;
	vector<string> themes;
	int curTheme;
	string prevTheme = m_cfg.getString(" GENERAL", "theme");

	listThemes(m_themeDir.c_str(), themes);
	curTheme = 0;
	for (u32 i = 0; i < themes.size(); ++i)
	{
		if (themes[i] == prevTheme)
		{
			curTheme = i;
			break;
		}
	}

	_showConfigAdv();
	while (true)
	{
		WPAD_ScanPads();
		padsState = WPAD_ButtonsDown(0);
		wd = WPAD_Data(0);
		_btnRepeat(wd->btns_h);
		if ((padsState & (WPAD_BUTTON_HOME | WPAD_BUTTON_B)) != 0)
			break;
		if (wd->ir.valid)
			m_btnMgr.mouse(wd->ir.x - m_cur.width() / 2, wd->ir.y - m_cur.height() / 2);
		else if ((padsState & WPAD_BUTTON_UP) != 0)
			m_btnMgr.up();
		else if ((padsState & WPAD_BUTTON_DOWN) != 0)
			m_btnMgr.down();
		if ((padsState & WPAD_BUTTON_MINUS) != 0 || ((padsState & WPAD_BUTTON_A) != 0 && m_btnMgr.selected() == m_configBtnPageM))
		{
			nextPage = max(1, m_locked ? 1 : g_curPage - 1);
			m_btnMgr.click(m_configBtnPageM);
			break;
		}
		if (!m_locked && ((padsState & WPAD_BUTTON_PLUS) != 0 || ((padsState & WPAD_BUTTON_A) != 0 && m_btnMgr.selected() == m_configBtnPageP)))
		{
			nextPage = min(g_curPage + 1, CMenu::_nbCfgPages);
			m_btnMgr.click(m_configBtnPageP);
			break;
		}
		if ((padsState & WPAD_BUTTON_A) != 0)
		{
			m_btnMgr.click();
			if (m_btnMgr.selected() == m_configBtnBack)
				break;
			else if (m_btnMgr.selected() == m_configAdvBtnInstall)
			{
				if (!WBFS_IsReadOnly())
				{
					_hideConfigAdv();
					_wbfsOp(CMenu::WO_ADD_GAME);
					_showConfigAdv();
				}
				else
				{
					error(_t("wbfsop10", L"This filesystem is read-only. You cannot install games or remove them."));
				}
			}
			else if (m_btnMgr.selected() == m_configAdvBtnCurThemeP)
			{
				curTheme = loopNum(curTheme + 1, (int)themes.size());
				m_cfg.setString(" GENERAL", "theme", themes[curTheme]);
				_showConfigAdv();
			}
			else if (m_btnMgr.selected() == m_configAdvBtnCurThemeM)
			{
				curTheme = loopNum(curTheme - 1, (int)themes.size());
				m_cfg.setString(" GENERAL", "theme", themes[curTheme]);
				_showConfigAdv();
			}
			else if (m_btnMgr.selected() == m_configAdvBtnCurLanguageP)
			{
				m_curLanguage = m_loc.nextDomain(m_curLanguage);
				m_cfg.setString(" GENERAL", "language", m_curLanguage);
				_updateText();
				_showConfigAdv();
			}
			else if (m_btnMgr.selected() == m_configAdvBtnCurLanguageM)
			{
				m_curLanguage = m_loc.prevDomain(m_curLanguage);
				m_cfg.setString(" GENERAL", "language", m_curLanguage);
				_updateText();
				_showConfigAdv();
			}
			else if (m_btnMgr.selected() == m_configAdvBtnCFTheme)
			{
				_hideConfigAdv();
				_cfTheme();
				_showConfigAdv();
			}
		}
		_mainLoopCommon(wd);
	}
	_hideConfigAdv();
	if (m_gameList.empty())
		_loadGameList();
	return nextPage;
}

void CMenu::_initConfigAdvMenu(CMenu::SThemeData &theme)
{
	_addUserLabels(theme, m_configAdvLblUser, ARRAY_SIZE(m_configAdvLblUser), "CONFIG_ADV");
	m_configAdvBg = _texture(theme.texSet, "CONFIG_ADV/BG", "texture", theme.bg);
	m_configAdvLblTheme = _addLabel(theme, "CONFIG_ADV/THEME", theme.lblFont, L"", 40, 130, 290, 56, theme.lblFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
	m_configAdvLblCurTheme = _addLabel(theme, "CONFIG_ADV/THEME_BTN", theme.btnFont, L"", 386, 130, 158, 56, theme.btnFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE, theme.btnTexC);
	m_configAdvBtnCurThemeM = _addPicButton(theme, "CONFIG_ADV/THEME_MINUS", theme.btnTexMinus, theme.btnTexMinusS, 330, 130, 56, 56);
	m_configAdvBtnCurThemeP = _addPicButton(theme, "CONFIG_ADV/THEME_PLUS", theme.btnTexPlus, theme.btnTexPlusS, 544, 130, 56, 56);
	m_configAdvLblLanguage = _addLabel(theme, "CONFIG_ADV/LANGUAGE", theme.lblFont, L"", 40, 190, 290, 56, theme.lblFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
	m_configAdvLblCurLanguage = _addLabel(theme, "CONFIG_ADV/LANGUAGE_BTN", theme.btnFont, L"", 386, 190, 158, 56, theme.btnFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE, theme.btnTexC);
	m_configAdvBtnCurLanguageM = _addPicButton(theme, "CONFIG_ADV/LANGUAGE_MINUS", theme.btnTexMinus, theme.btnTexMinusS, 330, 190, 56, 56);
	m_configAdvBtnCurLanguageP = _addPicButton(theme, "CONFIG_ADV/LANGUAGE_PLUS", theme.btnTexPlus, theme.btnTexPlusS, 544, 190, 56, 56);
	m_configAdvLblCFTheme = _addLabel(theme, "CONFIG_ADV/CUSTOMIZE_CF", theme.lblFont, L"", 40, 250, 290, 56, theme.lblFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
	m_configAdvBtnCFTheme = _addButton(theme, "CONFIG_ADV/CUSTOMIZE_CF_BTN", theme.btnFont, L"", 330, 250, 270, 56, theme.btnFontColor);
	m_configAdvLblInstall = _addLabel(theme, "CONFIG_ADV/INSTALL", theme.lblFont, L"", 40, 310, 290, 56, theme.lblFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
	m_configAdvBtnInstall = _addButton(theme, "CONFIG_ADV/INSTALL_BTN", theme.btnFont, L"", 330, 310, 270, 56, theme.btnFontColor);
	// 
	_setHideAnim(m_configAdvLblTheme, "CONFIG_ADV/THEME", 100, 0, -2.f, 0.f);
	_setHideAnim(m_configAdvLblCurTheme, "CONFIG_ADV/THEME_BTN", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configAdvBtnCurThemeM, "CONFIG_ADV/THEME_MINUS", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configAdvBtnCurThemeP, "CONFIG_ADV/THEME_PLUS", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configAdvLblLanguage, "CONFIG_ADV/LANGUAGE", 100, 0, -2.f, 0.f);
	_setHideAnim(m_configAdvLblCurLanguage, "CONFIG_ADV/LANGUAGE_BTN", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configAdvBtnCurLanguageM, "CONFIG_ADV/LANGUAGE_MINUS", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configAdvBtnCurLanguageP, "CONFIG_ADV/LANGUAGE_PLUS", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configAdvLblCFTheme, "CONFIG_ADV/CUSTOMIZE_CF", 100, 0, -2.f, 0.f);
	_setHideAnim(m_configAdvBtnCFTheme, "CONFIG_ADV/CUSTOMIZE_CF_BTN", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configAdvLblInstall, "CONFIG_ADV/INSTALL", 100, 0, -2.f, 0.f);
	_setHideAnim(m_configAdvBtnInstall, "CONFIG_ADV/INSTALL_BTN", 0, 0, 1.f, -1.f);
	_hideConfigAdv(true);
	_textConfigAdv();
}

void CMenu::_textConfigAdv(void)
{
	m_btnMgr.setText(m_configAdvLblTheme, _t("cfga7", L"Theme"));
	m_btnMgr.setText(m_configAdvLblLanguage, _t("cfga6", L"Language"));
	m_btnMgr.setText(m_configAdvLblCFTheme, _t("cfgc4", L"Adjust Coverflow"));
	m_btnMgr.setText(m_configAdvBtnCFTheme, _t("cfgc5", L"Go"));
	m_btnMgr.setText(m_configAdvLblInstall, _t("cfga2", L"Install game"));
	m_btnMgr.setText(m_configAdvBtnInstall, _t("cfga3", L"Install"));
}
