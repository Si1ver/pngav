///////////////////////////////////////////////////////////////////////////////
// registry.cpp


#include "registry.h"
#include <malloc.h>


using namespace std;


// remove
bool Registry::remove(HKEY i_root, const string &i_path, const string &i_name)
{
  if (i_name.empty())
    return RegDeleteKey(i_root, i_path.c_str()) == ERROR_SUCCESS;
  HKEY hkey;
  if (ERROR_SUCCESS !=
      RegOpenKeyEx(i_root, i_path.c_str(), 0, KEY_SET_VALUE, &hkey))
    return false;
  LONG r = RegDeleteValue(hkey, i_name.c_str());
  RegCloseKey(hkey);
  return r == ERROR_SUCCESS;
}


// does exist the key ?
bool Registry::doesExist(HKEY i_root, const string &i_path)
{
  HKEY hkey;
  if (ERROR_SUCCESS !=
      RegOpenKeyEx(i_root, i_path.c_str(), 0, KEY_READ, &hkey))
    return false;
  RegCloseKey(hkey);
  return true;
}


// read DWORD
bool Registry::read(HKEY i_root, const string &i_path,
		    const string &i_name, int *o_value, int i_defaultValue)
{
  HKEY hkey;
  if (ERROR_SUCCESS ==
      RegOpenKeyEx(i_root, i_path.c_str(), 0, KEY_READ, &hkey))
  {
    DWORD type = REG_DWORD;
    DWORD size = sizeof(*o_value);
    LONG r = RegQueryValueEx(hkey, i_name.c_str(), NULL,
			     &type, (BYTE *)o_value, &size);
    RegCloseKey(hkey);
    if (r == ERROR_SUCCESS)
      return true;
  }
  *o_value = i_defaultValue;
  return false;
}


// write DWORD
bool Registry::write(HKEY i_root, const string &i_path, const string &i_name,
		     int i_value)
{
  HKEY hkey;
  DWORD disposition;
  std::string c_name("REG_SZ");

  if (ERROR_SUCCESS !=
      RegCreateKeyEx(i_root, i_path.c_str(), 0, c_name.data(),
		     REG_OPTION_NON_VOLATILE,
		     KEY_ALL_ACCESS, NULL, &hkey, &disposition))
    return false;
  LONG r = RegSetValueEx(hkey, i_name.c_str(), NULL, REG_DWORD,
			 (BYTE *)&i_value, sizeof(i_value));
  RegCloseKey(hkey);
  return r == ERROR_SUCCESS;
}


// read string
bool Registry::read(HKEY i_root, const string &i_path, const string &i_name,
		    string *o_value, const string &i_defaultValue)
{
  HKEY hkey;
  if (ERROR_SUCCESS ==
      RegOpenKeyEx(i_root, i_path.c_str(), 0, KEY_READ, &hkey))
  {
    DWORD type = REG_SZ;
    DWORD size = 0;
    BYTE dummy;
    if (ERROR_MORE_DATA ==
	RegQueryValueEx(hkey, i_name.c_str(), NULL, &type, &dummy, &size))
    {
      BYTE *buf = (BYTE *)_alloca(size);
      if (ERROR_SUCCESS ==
	  RegQueryValueEx(hkey, i_name.c_str(), NULL, &type, buf, &size))
      {
	*o_value = (char *)buf;
	RegCloseKey(hkey);
	return true;
      }
    }
    RegCloseKey(hkey);
  }
  if (!i_defaultValue.empty())
    *o_value = i_defaultValue;
  return false;
}


// write string
bool Registry::write(HKEY i_root, const string &i_path,
		     const string &i_name, const string &i_value)
{
  HKEY hkey;
  DWORD disposition;
  std::string c_name("REG_SZ");

  if (ERROR_SUCCESS !=
      RegCreateKeyEx(i_root, i_path.c_str(), 0, c_name.data(),
		     REG_OPTION_NON_VOLATILE,
		     KEY_ALL_ACCESS, NULL, &hkey, &disposition))
    return false;
  RegSetValueEx(hkey, i_name.c_str(), NULL, REG_SZ,
		(BYTE *)i_value.c_str(), i_value.size() + 1);
  RegCloseKey(hkey);
  return true;
}


// read binary
bool Registry::read(HKEY i_root, const string &i_path,
		    const string &i_name, BYTE *o_value, DWORD i_valueSize,
		    const BYTE *i_defaultValue, DWORD i_defaultValueSize)
{
  if (o_value && 0 < i_valueSize)
  {
    HKEY hkey;
    if (ERROR_SUCCESS ==
	RegOpenKeyEx(i_root, i_path.c_str(), 0, KEY_READ, &hkey))
    {
      DWORD type = REG_BINARY;
      LONG r = RegQueryValueEx(hkey, i_name.c_str(), NULL, &type,
			       (BYTE *)o_value, &i_valueSize);
      RegCloseKey(hkey);
      if (r == ERROR_SUCCESS)
	return true;
    }
  }
  if (i_defaultValue)
    CopyMemory(o_value, i_defaultValue,
	       min(i_defaultValueSize, i_valueSize));
  return false;
}


// write binary
bool Registry::write(HKEY i_root, const string &i_path, const string &i_name,
		     const BYTE *i_value, DWORD i_valueSize)
{
  if (!i_value)
    return false;
  HKEY hkey;
  DWORD disposition;
  std::string c_name("REG_BINARY");

  if (ERROR_SUCCESS !=
      RegCreateKeyEx(i_root, i_path.c_str(), 0, c_name.data(),
		     REG_OPTION_NON_VOLATILE,
		     KEY_ALL_ACCESS, NULL, &hkey, &disposition))
    return false;
  RegSetValueEx(hkey, i_name.c_str(), NULL, REG_BINARY, i_value, i_valueSize);
  RegCloseKey(hkey);
  return true;
}
