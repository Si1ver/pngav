///////////////////////////////////////////////////////////////////////////////
// SPI / Susie Plug Ins


#ifndef _SPI_H
#  define _SPI_H

#  include <Windows.h>
#  include <sys/types.h>
#  include <list>
#  include <string>


namespace SPI
{
#  pragma pack(push, 1)
  struct PictureInfo
  {
    long left;					// �摜��W�J����ʒu
    long top;					// 	�V
    long width;					// �摜�̕�(pixel)
    long height;				// �摜�̍���(pixel)
    WORD x_density;				// ��f�̐����������x
    WORD y_density;				// ��f�̐����������x
    short colorDepth;				// �P��f�������bit��
    HLOCAL hInfo;				// �摜���̃e�L�X�g���
  };
#  pragma pack(pop)

  struct FileInfo
  {
    unsigned char method[8];			// ���k�@�̎��
    unsigned long position;			// �t�@�C����ł̈ʒu
    unsigned long compsize;			// ���k���ꂽ�T�C�Y
    unsigned long filesize;			// ���̃t�@�C���T�C�Y
    time_t timestamp;				// �t�@�C���̍X�V����
    char path[200];				// ���΃p�X
    char filename[200];				// �t�@�C���l�[��
    unsigned long crc;				// CRC
  };

#  define DllExport __declspec(dllexport)

  typedef int DllExport (FAR PASCAL *GetPluginInfo_t)(
    int infono, LPSTR buf, int buflen);
  
  typedef int DllExport (FAR PASCAL *IsSupported_t)(
    LPSTR filename, DWORD dw);
  
  typedef int DllExport (FAR PASCAL *GetPictureInfo_t)(
    LPSTR buf, long len, unsigned int flag, struct PictureInfo FAR *lpInfo);
  
  typedef int DllExport (FAR PASCAL *GetPicture_t)(
    LPCSTR buf, long len, unsigned int flag, HLOCAL FAR *pHBInfo,
    HLOCAL FAR *pHBm, FARPROC lpPrgressCallback, long lData);
  
  typedef int DllExport (FAR PASCAL *GetPreview_t)(
    LPSTR buf, long len, unsigned int flag, HLOCAL *pHBInfo, HLOCAL FAR *pHBm,
    FARPROC lpPrgressCallback, long lData);
  
  typedef int DllExport (FAR PASCAL *GetArchiveInfo_t)(
    LPSTR buf, long len, unsigned int flag, HGLOBAL FAR *lphInf);
  
  typedef int DllExport (FAR PASCAL *GetFileInfo_t)(
    LPSTR buf, long len, LPSTR filename, unsigned int flag,
    FileInfo FAR *lpInfo);
  
  typedef int DllExport (FAR PASCAL *GetFile_t)(
    LPSTR src, long len, LPSTR dest, unsigned int flag,
    FARPROC prgressCallback, long lData);

  enum {
    ERROR_NORMAL		= 0,	// ����I��
    ERROR_NOT_IMPLEMENTED	= -1,	// ���̋@�\�̓C���v�������g����Ă��Ȃ�
    ERROR_STOP_EXTRACT		= 1,	// �R�[���o�b�N�֐�����0��Ԃ����̂œW
    					// �J�𒆎~����
    ERROR_UNKNOWN_FORMAT	= 2,	// ���m�̃t�H�[�}�b�g
    ERROR_DAMAGED_DATA		= 3,	// �f�[�^�����Ă���
    ERROR_OUT_OF_MEMORY		= 4,	// �������[���m�ۏo���Ȃ�
    ERROR_MEMORY		= 5,	// �������[�G���[�iLock�o���Ȃ��A���j
    ERROR_READ_FAILED		= 6,	// �t�@�C�����[�h�G���[
    ERROR_RESERVED		= 7,	// �i�\��j
    ERROR_INTERNAL		= 8,	// �����G���[
  };

  // �v���O�C��
  class PlugIn
  {
  public:
    class Type
    {
    public:
      typedef std::list<std::string> Patterns;
      
    public:
      Patterns m_patterns;			// �g���q�p�^�[��
      std::string m_description;		// �g���q�p�^�[���̐���
    };
    typedef std::list<Type> Types;

  private:
    HMODULE m_hModule;				// SPI �̃n���h��
    bool m_isPicture;				// 00IN:TRUE, 00AM:FALSE
    Types m_types;				// �g���q

  public:
    GetPluginInfo_t  getPluginInfo;
    IsSupported_t    isSupported;
    GetPictureInfo_t getPictureInfo;
    GetPicture_t     getPicture;
    GetPreview_t     getPreview;
    GetArchiveInfo_t getArchiveInfo;
    GetFileInfo_t    getFileInfo;
    GetFile_t        getFile;

  private:
    // SPI ���W���[������A���p�\�Ȋg���q�̃��X�g��ǂݏo��
    void getType();

  public:
    // �R���X�g���N�^
    PlugIn();

    // �f�X�g���N�^
    ~PlugIn();
  
    // SPI ���W���[���̃��[�h
    bool load(const std::string &i_path);

    // SPI ���W���[���̃A�����[�h
    void unload();

    // ���� SPI �� filename �̃t�@�C����ǂ߂邩�ǂ��� (�g���q�Ŕ���)
    const Type *doesMatch(const std::string &i_filename);
    
    // SPI ���W���[�������[�h����Ă��邩�ǂ���
    bool isLoaded() { return !!m_hModule; }
    
    // ���� SPI ���W���[���͊G��W�J������̂��H���ɂ�W�J������̂��H
    bool isPicture() { return m_isPicture; }
  };

  // �v���O�C���}�l�[�W��
  class Manager
  {
  public:
    typedef std::list<PlugIn> PlugIns;

  private:
    PlugIns m_plugIns;

  public:
    // destructor
    ~Manager();
    
    // SPI �̃��[�h
    bool load(const std::string &i_path = "");
    
    // SPI �̃A�����[�h
    void unload();
    
    // �t�@�C��������v���O�C���𓾂�
    PlugIn *getPlugIn(const std::string &i_filename);

    // SPI �̑��݂���p�X���擾����
    static std::string getPath();

    // SPI �̑��݂���p�X��ݒ肷��
    static bool setPath(const std::string &i_path);
  };
}

#endif // _SPI_H
