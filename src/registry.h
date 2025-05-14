///////////////////////////////////////////////////////////////////////////////
// registry.h

#ifndef _REGISTRY_H
#  define _REGISTRY_H

#  include <string>
#  include <windows.h>


//
class Registry
{
  HKEY m_root;// registry m_root
  std::string m_path;// m_path from registry m_root

public:
  // constructor
  Registry()
    : m_root(nullptr) {}

  // constructor
  Registry(HKEY i_root, const std::string &i_path)
    : m_root(i_root), m_path(i_path) {}

  // set registry m_root and m_path
  void setM_Root(HKEY i_root, const std::string &i_path)
  {
    m_root = i_root;
    m_path = i_path;
  }

  // remvoe
  bool remove(const std::string &i_name = "") const { return remove(m_root, m_path, i_name); }

  // does exist the key ?
  bool doesExist() const { return doesExist(m_root, m_path); }

  // read DWORD
  bool read(const std::string &i_name,
    int *o_value,
    int i_defaultValue = 0) const { return read(m_root, m_path, i_name, o_value, i_defaultValue); }

  // write DWORD
  bool write(const std::string &i_name, int i_value) const { return write(m_root, m_path, i_name, i_value); }

  // read std::string
  bool read(const std::string &i_name,
    std::string *o_value,
    const std::string &i_defaultValue = "") const { return read(m_root, m_path, i_name, o_value, i_defaultValue); }

  // write std::string
  bool write(const std::string &i_name, const std::string &i_value) const
  {
    return write(m_root, m_path, i_name, i_value);
  }

  // read binary
  bool read(const std::string &i_name,
    BYTE *o_value,
    DWORD i_valueSize,
    const BYTE *i_defaultValue = nullptr,
    DWORD i_defaultValueSize = 0) const
  {
    return read(m_root,
      m_path,
      i_name,
      o_value,
      i_valueSize,
      i_defaultValue,
      i_defaultValueSize);
  }

  // write binary
  bool write(const std::string &i_name,
    const BYTE *i_value,
    DWORD i_valueSize) const { return write(m_root, m_path, i_name, i_value, i_valueSize); }

#  define REGISTRY_PATH							\
  HKEY i_root, const std::string &i_path, const std::string &i_name

  // remove
  static bool remove(REGISTRY_PATH = "");

  // does exist the key ?
  static bool doesExist(HKEY i_root, const std::string &i_path);

  // read DWORD
  static bool read(REGISTRY_PATH, int *o_value, int i_defaultValue = 0);
  // write DWORD
  static bool write(REGISTRY_PATH, int i_value);

  // read std::string
  static bool read(REGISTRY_PATH,
    std::string *o_value,
    const std::string &i_defaultValue = "");
  // write std::string
  static bool write(REGISTRY_PATH, const std::string &i_value);

  // read binary
  static bool read(REGISTRY_PATH,
    BYTE *o_value,
    DWORD i_valueSize,
    const BYTE *i_defaultValue = nullptr,
    DWORD i_defaultValueSize = 0);
  // write binary
  static bool write(REGISTRY_PATH, const BYTE *i_value, DWORD i_valueSize);
#  undef REGISTRY_PATH
};


#endif // _REGISTRY_H
