///////////////////////////////////////////////////////////////////////////////
// SPI / Susie Plug Ins

#include "spi.hpp"

#include <mbstring.h>
#include <shlwapi.h>

#include "additions/susie_plugins/plugin_manager.hpp"

namespace SPI {
/////////////////////////////////////////////////////////////////////////////
// PlugIn

// SPI モジュールから、利用可能な拡張子のリストを読み出す
void PlugIn::getType()
{
  for (int i = 0; ; ++i) {
    char buf[1024];
    int len = getPluginInfo(2 * i + 2, buf, sizeof(buf));
    if (len == 0) break;

    m_types.emplace_back();
    Type &type = m_types.back();

    // パターンを得る
    for (char *s = buf; ;) {
      char *b = s;
      s = (char *)_mbschr((unsigned char *)s, ';');
      if (s == nullptr) {
        if (*b) type.m_patterns.emplace_back(b);
        break;
      }
      type.m_patterns.emplace_back(b, s);
      ++s;
    }

    // 説明を得る
    getPluginInfo(2 * i + 3, buf, sizeof(buf));
    type.m_description = buf;
  }
}


// コンストラクタ
PlugIn::PlugIn()
  : m_hModule(nullptr),
    m_isPicture(true),
    getPluginInfo(nullptr),
    isSupported(nullptr),
    getPictureInfo(nullptr),
    getPicture(nullptr),
    getPreview(nullptr),
    getArchiveInfo(nullptr),
    getFileInfo(nullptr),
    getFile(nullptr) {}

// デストラクタ
PlugIn::~PlugIn() { unload(); }

// SPI モジュールのロード
bool PlugIn::load(const std::wstring &i_path)
{
  unload();
  m_hModule = LoadLibraryW(i_path.c_str());
  if (!m_hModule) return false;

  getPluginInfo
    = (GetPluginInfo_t)GetProcAddress(m_hModule, "GetPluginInfo");
  isSupported
    = (IsSupported_t)GetProcAddress(m_hModule, "IsSupported");
  getPictureInfo
    = (GetPictureInfo_t)GetProcAddress(m_hModule, "GetPictureInfo");
  getPicture
    = (GetPicture_t)GetProcAddress(m_hModule, "GetPicture");
  getPreview
    = (GetPreview_t)GetProcAddress(m_hModule, "GetPreview");
  getArchiveInfo
    = (GetArchiveInfo_t)GetProcAddress(m_hModule, "GetArchiveInfo");
  getFileInfo
    = (GetFileInfo_t)GetProcAddress(m_hModule, "GetFileInfo");
  getFile
    = (GetFile_t)GetProcAddress(m_hModule, "GetFile");

  if (getPluginInfo) {
    char buf[1024];
    getPluginInfo(0, buf, sizeof(buf));
    if (strcmp(buf, "00IN") == 0)// 絵
    {
      m_isPicture = TRUE;
      getType();
      return true;
    }
    if (strcmp(buf, "00AM") == 0)// 書庫
    {
      m_isPicture = false;
      getType();
      return true;
    }
  }
  unload();
  return false;
}

// SPI モジュールのアンロード
void PlugIn::unload()
{
  if (m_hModule) {
    FreeLibrary(m_hModule);
    m_hModule = nullptr;
  }
  m_types.clear();
}

// この SPI が filename のファイルを読めるかどうか (拡張子で判別)
const PlugIn::Type *PlugIn::doesMatch(const std::string &i_filename)
{
  for (auto &m_type : m_types) {
    for (auto
         j = m_type.m_patterns.begin(); j != m_type.m_patterns.end(); ++j) {
      if (PathMatchSpec(i_filename.c_str(), j->c_str())) return &m_type;
    }
  }
  return nullptr;
}


/////////////////////////////////////////////////////////////////////////////
// Manager

// destructor
Manager::~Manager() { unload(); }

// SPI のロード
bool Manager::load()
{
  unload();

  std::wstring path = getPath();

  if (!path.empty() && path.back() != L'\\') {
    path += L'\\';
  }

  // SPI をロード
  WIN32_FIND_DATAW fd = {};
  std::wstring pattern = path + L"*.spi";
  HANDLE hff = FindFirstFileW(pattern.c_str(), &fd);

  if (hff == INVALID_HANDLE_VALUE) return false;
  do {
    m_plugIns.emplace_back();
    m_plugIns.back().load(path + fd.cFileName);
  } while (FindNextFileW(hff, &fd));
  FindClose(hff);
  return true;
}

// SPI のアンロード
void Manager::unload() { m_plugIns.clear(); }

// ファイル名からプラグインを得る
PlugIn *Manager::getPlugIn(const std::string &i_filename)
{
  for (auto &m_plugIn : m_plugIns) {
    if (m_plugIn.doesMatch(i_filename)) return &m_plugIn;
  }
  return nullptr;
}

// SPI の存在するパスを取得する
std::wstring Manager::getPath()
{
  return additions::susie_plugins::PluginManager::getPluginPath().wstring();
}
}
