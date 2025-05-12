///////////////////////////////////////////////////////////////////////////////
// SPI / Susie Plug Ins


#include "spi.h"
#include "registry.h"
#include <mbstring.h>
#include <shlwapi.h>


namespace SPI
{
  /////////////////////////////////////////////////////////////////////////////
  // PlugIn
  
  // SPI ���W���[������A���p�\�Ȋg���q�̃��X�g��ǂݏo��
  void PlugIn::getType()
  {
    for (int i = 0; ; ++ i)
    {
      char buf[1024];
      int len = getPluginInfo(2 * i + 2, buf, sizeof(buf));
      if (len == 0)
	break;

      m_types.push_back(Type());
      Type &type = m_types.back();

      // �p�^�[���𓾂�
      for (char *s = buf; ; )
      {
	char *b = s;
	s = (char *)_mbschr((unsigned char *)s, ';');
	if (s == NULL)
	{
	  if (*b)
	    type.m_patterns.push_back(b);
	  break;
	}
	type.m_patterns.push_back(std::string(b, s));
	++ s;
      }

      // �����𓾂�
      getPluginInfo(2 * i + 3, buf, sizeof(buf));
      type.m_description = buf;
    }
  }


  // �R���X�g���N�^
  PlugIn::PlugIn()
    : m_hModule(NULL),
      m_isPicture(true),
      getPluginInfo(NULL),
      isSupported(NULL),
      getPictureInfo(NULL),
      getPicture(NULL),
      getPreview(NULL),
      getArchiveInfo(NULL),
      getFileInfo(NULL),
      getFile(NULL)
  {
  }

  // �f�X�g���N�^
  PlugIn::~PlugIn()
  {
    unload();
  }

  // SPI ���W���[���̃��[�h
  bool PlugIn::load(const std::string &i_path)
  {
    unload();
    m_hModule = LoadLibrary(i_path.c_str());
    if (!m_hModule)
      return false;
    
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
    
    if (getPluginInfo)
    {
      char buf[1024];
      getPluginInfo(0, buf, sizeof(buf));
      if (strcmp(buf, "00IN") == 0) // �G
      {
	m_isPicture = TRUE;
	getType();
	return true;
      }
      else if (strcmp(buf, "00AM") == 0) // ����
      {
	m_isPicture = false;
	getType();
	return true;
      }
    }
    unload();
    return false;
  }
  
  // SPI ���W���[���̃A�����[�h
  void PlugIn::unload()
  {
    if (m_hModule)
    {
      FreeLibrary(m_hModule);
      m_hModule = NULL;
    }
    m_types.clear();
  }

  // ���� SPI �� filename �̃t�@�C����ǂ߂邩�ǂ��� (�g���q�Ŕ���)
  const PlugIn::Type *PlugIn::doesMatch(const std::string &i_filename)
  {
    for (Types::iterator i = m_types.begin(); i != m_types.end(); ++ i)
      for (Type::Patterns::iterator
	     j = i->m_patterns.begin(); j != i->m_patterns.end(); ++ j)
      {
	if (PathMatchSpec(i_filename.c_str(), j->c_str()))
	  return &*i;
      }
    return NULL;
  }
  

  /////////////////////////////////////////////////////////////////////////////
  // Manager

  // destructor
  Manager::~Manager()
  {
    unload();
  }

  // SPI �̃��[�h
  bool Manager::load(const std::string &i_path)
  {
    unload();

    std::string path = i_path;
    if (path.empty())
      path = getPath();
    path += "\\";

    // SPI �����[�h
    WIN32_FIND_DATA fd;
    std::string pattern = path + "*.spi";
    HANDLE hff = FindFirstFile(pattern.c_str(), &fd);
    if (hff == INVALID_HANDLE_VALUE)
      return false;
    do {
      m_plugIns.push_back(PlugIn());
      m_plugIns.back().load(path + fd.cFileName);
    } while (FindNextFile(hff, &fd));
    FindClose(hff);
    return true;
  }

  // SPI �̃A�����[�h
  void Manager::unload()
  {
    m_plugIns.clear();
  }

  // �t�@�C��������v���O�C���𓾂�
  PlugIn *Manager::getPlugIn(const std::string &i_filename)
  {
    for (PlugIns::iterator i = m_plugIns.begin(); i != m_plugIns.end(); ++ i)
      if (i->doesMatch(i_filename))
	return &*i;
    return NULL;
  }
  
  // SPI �̑��݂���p�X���擾����
  std::string Manager::getPath()
  {
    char buf[1024] = ".";
    if (GetModuleFileName(NULL, buf, sizeof(buf)))
    {
      char *s = (char *)_mbsrchr((unsigned char *)buf, '\\');
      if (s)
	*s = '\0';
    }
    std::string path;
    Registry::read(HKEY_CURRENT_USER, "Software\\Takechin\\Susie\\Plug-in",
		   "Path", &path, buf);
    return path;
  }

  // SPI �̑��݂���p�X��ݒ肷��
  bool Manager::setPath(const std::string &i_path)
  {
    return Registry::write(HKEY_CURRENT_USER,
			   "Software\\Takechin\\Susie\\Plug-in",
			   "Path", i_path);
  }
}
