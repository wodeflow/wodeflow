
#include <fstream>

#include "config.hpp"
#include "text.hpp"
#include "string_utils.hpp"

static const int g_floatPrecision = 10;

const std::string Config::emptyString;

Config::Config(void) :
	m_loaded(false), m_domains(), m_filename(), m_iter()
{
}

bool Config::hasDomain(const std::string &domain) const
{
	return m_domains.count(domain) > 0;
}

void Config::copyDomain(const std::string &dst, const std::string &src)
{
	m_domains[dst] = m_domains[src];
}

const std::string &Config::firstDomain(void)
{
	m_iter = m_domains.begin();
	if (m_iter == m_domains.end())
		return Config::emptyString;
	return m_iter->first;
}

const std::string &Config::nextDomain(void)
{
	++m_iter;
	if (m_iter == m_domains.end())
		return Config::emptyString;
	return m_iter->first;
}

const std::string &Config::nextDomain(const std::string &start) const
{
	Config::DomainMap::const_iterator i;
	Config::DomainMap::const_iterator j;
	if (m_domains.empty())
		return Config::emptyString;
	i = m_domains.find(start);
	if (i == m_domains.end())
		return m_domains.begin()->first;
	j = i;
	++j;
	return j != m_domains.end() ? j->first : i->first;
}

const std::string &Config::prevDomain(const std::string &start) const
{
	Config::DomainMap::const_iterator i;
	if (m_domains.empty())
		return Config::emptyString;
	i = m_domains.find(start);
	if (i == m_domains.end() || i == m_domains.begin())
		return m_domains.begin()->first;
	--i;
	return i->first;
}

bool Config::load(const char *filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	std::string domain("");
	std::string line;
	u32 n = 0;

	m_loaded = false;
	m_filename = filename;

	if (!file.is_open())
		return m_loaded;

	m_domains.clear();
	while (file.good())
	{
		line.clear();
		getline(file, line, '\n');
		++n;
		
		if (!file.bad() && !file.fail())
		{
			line = trimEnd(line);
			if (line.empty() || line[0] == '#')
				continue;
			if (line[0] == '[')
			{
				auto i = line.find_first_of(']');
				if (i != std::string::npos && i > 1)
				{
					domain = upperCase(line.substr(1, i - 1));
					if (m_domains.find(domain) != m_domains.end())
						domain.clear();
				}
			}
			else
				if (!domain.empty())
				{
					std::string::size_type i = line.find_first_of('=');
					if (i != std::string::npos && i > 0)
						m_domains[domain][lowerCase(trim(line.substr(0, i)))] = unescNewlines(trim(line.substr(i + 1)));
				}
		}
	}
	m_loaded = true;
	return m_loaded;
}

void Config::save(void)
{
	std::ofstream file(m_filename.c_str(), std::ios::out | std::ios::binary);
	//for (Config::DomainMap::iterator k = m_domains.begin(); k != m_domains.end(); ++k)
	for (auto k : m_domains)
	{
		Config::KeyMap *m = &k.second;
		file << '\n' << '[' << k.first << ']' << '\n';
		for (Config::KeyMap::iterator l = m->begin(); l != m->end(); ++l)
			file << l->first << '=' << escNewlines(l->second) << '\n';
	}
}

bool Config::has(const std::string &domain, const std::string &key) const
{
	if (domain.empty() || key.empty())
		return false;
	DomainMap::const_iterator i = m_domains.find(upperCase(domain));
	if (i == m_domains.end())
		return false;
	return i->second.find(lowerCase(key)) != i->second.end();
}

void Config::setWString(const std::string &domain, const std::string &key, const wstringEx &val)
{
	if (domain.empty() || key.empty())
		return;
	m_domains[upperCase(domain)][lowerCase(key)] = val.toUTF8();
}

void Config::setString(const std::string &domain, const std::string &key, const std::string &val)
{
	if (domain.empty() || key.empty())
		return;
	m_domains[upperCase(domain)][lowerCase(key)] = val;
}

void Config::setBool(const std::string &domain, const std::string &key, bool val)
{
	if (domain.empty() || key.empty())
		return;
	m_domains[upperCase(domain)][lowerCase(key)] = val ? "yes" : "no";
}

void Config::setOptBool(const std::string &domain, const std::string &key, int val)
{
	if (domain.empty() || key.empty())
		return;
	switch (val)
	{
		case 0:
			m_domains[upperCase(domain)][lowerCase(key)] = "no";
			break;
		case 1:
			m_domains[upperCase(domain)][lowerCase(key)] = "yes";
			break;
		default:
			m_domains[upperCase(domain)][lowerCase(key)] = "default";
	}
}

void Config::setInt(const std::string &domain, const std::string &key, int val)
{
	if (domain.empty() || key.empty())
		return;
	m_domains[upperCase(domain)][lowerCase(key)] = sfmt("%i", val);
}

void Config::setFloat(const std::string &domain, const std::string &key, float val)
{
	if (domain.empty() || key.empty())
		return;
	m_domains[upperCase(domain)][lowerCase(key)] = sfmt("%.*g", g_floatPrecision, val);
}

void Config::setVector3D(const std::string &domain, const std::string &key, const Vector3D &val)
{
	if (domain.empty() || key.empty())
		return;
	m_domains[upperCase(domain)][lowerCase(key)] = sfmt("%.*g, %.*g, %.*g", g_floatPrecision, val.x, g_floatPrecision, val.y, g_floatPrecision, val.z);
}

void Config::setColor(const std::string &domain, const std::string &key, const CColor &val)
{
	if (domain.empty() || key.empty())
		return;
	m_domains[upperCase(domain)][lowerCase(key)] = sfmt("#%.2X%.2X%.2X%.2X", val.r, val.g, val.b, val.a);
}

wstringEx Config::getWString(const std::string &domain, const std::string &key, const wstringEx &defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	std::string &data = m_domains[upperCase(domain)][lowerCase(key)];
	if (data.empty())
	{
		data = defVal.toUTF8();
		return defVal;
	}
	wstringEx ws;
	ws.fromUTF8(data.c_str());
	return ws;
}

std::string Config::getString(const std::string &domain, const std::string &key, const std::string &defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	
	std::string &data = m_domains[upperCase(domain)][lowerCase(key)];
	if (data.empty())
	{
		data = defVal;
		return defVal;
	}
	return data;
}

bool Config::getBool(const std::string &domain, const std::string &key, bool defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	
	std::string &data = m_domains[upperCase(domain)][lowerCase(key)];
	if (data.empty())
	{
		data = defVal ? "yes" : "no";
		return defVal;
	}
	std::string s(lowerCase(trim(data)));
	if (s == "yes" || s == "true" || s == "y" || s == "1")
		return true;
	return false;
}

bool Config::testOptBool(const std::string &domain, const std::string &key, bool defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	KeyMap &km = m_domains[upperCase(domain)];
	KeyMap::iterator i = km.find(lowerCase(key));
	if (i == km.end())
		return defVal;
	if (lowerCase(trim(i->second)) == "yes")
		return true;
	if (lowerCase(trim(i->second)) == "no")
		return false;
	return defVal;
}

int Config::getOptBool(const std::string &domain, const std::string &key, int defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	std::string &data = m_domains[upperCase(domain)][lowerCase(key)];
	if (data.empty())
	{
		switch (defVal)
		{
			case 0:
				data = "no";
				break;
			case 1:
				data = "yes";
				break;
			default:
				data = "default";
		}
		return defVal;
	}
	if (lowerCase(trim(data)) == "yes")
		return 1;
	if (lowerCase(trim(data)) == "no")
		return 0;
	return 2;
}

int Config::getInt(const std::string &domain, const std::string &key, int defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	std::string &data = m_domains[upperCase(domain)][lowerCase(key)];
	if (data.empty())
	{
		data = sfmt("%i", defVal);
		return defVal;
	}
	return strtol(data.c_str(), 0, 10);
}

float Config::getFloat(const std::string &domain, const std::string &key, float defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	std::string &data = m_domains[upperCase(domain)][lowerCase(key)];
	if (data.empty())
	{
		data = sfmt("%.*g", g_floatPrecision, defVal);
		return defVal;
	}
	return strtod(data.c_str(), 0);
}

Vector3D Config::getVector3D(const std::string &domain, const std::string &key, const Vector3D &defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	std::string &data = m_domains[upperCase(domain)][lowerCase(key)];
	std::size_t i;
	std::size_t j = std::string::npos;
	i = data.find_first_of(',');
	if (i != std::string::npos)
		j = data.find_first_of(',', i + 1);
	if (j == std::string::npos)
	{
		data = sfmt("%.*g, %.*g, %.*g", g_floatPrecision, defVal.x, g_floatPrecision, defVal.y, g_floatPrecision, defVal.z);
		return defVal;
	}
	return Vector3D(strtod(data.substr(0, i).c_str(), 0), strtod(data.substr(i + 1, j - i - 1).c_str(), 0), strtod(data.substr(j + 1).c_str(), 0));
}

CColor Config::getColor(const std::string &domain, const std::string &key, const CColor &defVal)
{
	if (domain.empty() || key.empty())
		return defVal;
	std::string &data = m_domains[upperCase(domain)][lowerCase(key)];
	std::string text(upperCase(trim(data)));
	std::size_t i = text.find_first_of('#');
	if (i != std::string::npos)
	{
		text.erase(0, i + 1);
		i = text.find_first_not_of("0123456789ABCDEF");
		if ((i != std::string::npos && i >= 6) || (i == std::string::npos && text.size() >= 6))
		{
			u32 n = ((i != std::string::npos && i >= 8) || (i == std::string::npos && text.size() >= 8)) ? 8 : 6;
			for (i = 0; i < n; ++i)
				if (text[i] <= '9')
					text[i] -= '0';
				else
					text[i] -= 'A' - 10;
			CColor c(text[0] * 0x10 + text[1], text[2] * 0x10 + text[3], text[4] * 0x10 + text[5], 1.f);
			if (n == 8)
				c.a = text[6] * 0x10 + text[7];
			return c;
		}
	}
	data = sfmt("#%.2X%.2X%.2X%.2X", defVal.r, defVal.g, defVal.b, defVal.a);
	return defVal;
}
