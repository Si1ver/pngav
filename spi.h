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
    long left;					// 画像を展開する位置
    long top;					// 	〃
    long width;					// 画像の幅(pixel)
    long height;				// 画像の高さ(pixel)
    WORD x_density;				// 画素の水平方向密度
    WORD y_density;				// 画素の垂直方向密度
    short colorDepth;				// １画素当たりのbit数
    HLOCAL hInfo;				// 画像内のテキスト情報
  };
#  pragma pack(pop)

  struct FileInfo
  {
    unsigned char method[8];			// 圧縮法の種類
    unsigned long position;			// ファイル上での位置
    unsigned long compsize;			// 圧縮されたサイズ
    unsigned long filesize;			// 元のファイルサイズ
    time_t timestamp;				// ファイルの更新日時
    char path[200];				// 相対パス
    char filename[200];				// ファイルネーム
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
    ERROR_NORMAL		= 0,	// 正常終了
    ERROR_NOT_IMPLEMENTED	= -1,	// その機能はインプリメントされていない
    ERROR_STOP_EXTRACT		= 1,	// コールバック関数が非0を返したので展
    					// 開を中止した
    ERROR_UNKNOWN_FORMAT	= 2,	// 未知のフォーマット
    ERROR_DAMAGED_DATA		= 3,	// データが壊れている
    ERROR_OUT_OF_MEMORY		= 4,	// メモリーが確保出来ない
    ERROR_MEMORY		= 5,	// メモリーエラー（Lock出来ない、等）
    ERROR_READ_FAILED		= 6,	// ファイルリードエラー
    ERROR_RESERVED		= 7,	// （予約）
    ERROR_INTERNAL		= 8,	// 内部エラー
  };

  // プラグイン
  class PlugIn
  {
  public:
    class Type
    {
    public:
      typedef std::list<std::string> Patterns;
      
    public:
      Patterns m_patterns;			// 拡張子パターン
      std::string m_description;		// 拡張子パターンの説明
    };
    typedef std::list<Type> Types;

  private:
    HMODULE m_hModule;				// SPI のハンドル
    bool m_isPicture;				// 00IN:TRUE, 00AM:FALSE
    Types m_types;				// 拡張子

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
    // SPI モジュールから、利用可能な拡張子のリストを読み出す
    void getType();

  public:
    // コンストラクタ
    PlugIn();

    // デストラクタ
    ~PlugIn();
  
    // SPI モジュールのロード
    bool load(const std::string &i_path);

    // SPI モジュールのアンロード
    void unload();

    // この SPI が filename のファイルを読めるかどうか (拡張子で判別)
    const Type *doesMatch(const std::string &i_filename);
    
    // SPI モジュールがロードされているかどうか
    bool isLoaded() { return !!m_hModule; }
    
    // この SPI モジュールは絵を展開するものか？書庫を展開するものか？
    bool isPicture() { return m_isPicture; }
  };

  // プラグインマネージャ
  class Manager
  {
  public:
    typedef std::list<PlugIn> PlugIns;

  private:
    PlugIns m_plugIns;

  public:
    // destructor
    ~Manager();
    
    // SPI のロード
    bool load(const std::string &i_path = "");
    
    // SPI のアンロード
    void unload();
    
    // ファイル名からプラグインを得る
    PlugIn *getPlugIn(const std::string &i_filename);

    // SPI の存在するパスを取得する
    static std::string getPath();

    // SPI の存在するパスを設定する
    static bool setPath(const std::string &i_path);
  };
}

#endif // _SPI_H
