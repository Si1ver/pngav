///////////////////////////////////////////////////////////////////////////////
// pngav

#include <algorithm>
#include <clocale>
#include <list>
#include <string>
#include <vector>

#include <png.h>

#include "misc.h"
#include "resource.h"
#include "spi.h"


HINSTANCE hInst;


class Image
{
public:
  DWORD m_width;
  DWORD m_height;
  BYTE **m_data;

private:
  static BYTE **newData(DWORD i_width, DWORD i_height)
  {
    auto data = new BYTE *[i_height];
    for (DWORD y = 0; y < i_height; y++) {
      data[y] = new BYTE[4 * i_width];
      memset(data[y], 0, 4 * i_width);
    }
    return data;
  }

  static void deleteData(BYTE **i_data, DWORD i_height)
  {
    if (!i_data) return;
    for (DWORD y = 0; y < i_height; y++) delete [] i_data[y];
    delete [] i_data;
  }

public:
  Image()
    : m_width(0), m_height(0), m_data(nullptr) {}

  ~Image() { deleteData(m_data, m_height); }

  void clear()
  {
    deleteData(m_data, m_height);
    m_data = nullptr;
    m_width = 0;
    m_height = 0;
  }

  void resize(DWORD i_width, DWORD i_height)
  {
    deleteData(m_data, m_height);
    clear();
    m_width = i_width;
    m_height = i_height;
    m_data = newData(m_width, m_height);
  }

  void addBorder()
  {
    if (!m_data) return;
    DWORD width = m_width + 2;
    DWORD height = m_height + 2;
    BYTE **data = newData(width, height);
    for (DWORD y = 0; y < m_height; y++) {
      memcpy(&data[y + 1][0], &m_data[y][0], 4);
      memcpy(&data[y + 1][4], &m_data[y][0], m_width * 4);
      memcpy(&data[y + 1][(width - 1) * 4],
        &m_data[y][(m_width - 1) * 4],
        4);
    }
    memcpy(data[0], data[1], width * 4);
    memcpy(data[height - 1], data[height - 2], width * 4);
    deleteData(m_data, m_width);
    m_width = width;
    m_height = height;
    m_data = data;
  }

  void copy(DWORD i_x, DWORD i_y, const Image *i_image)
  {
    DWORD x2 = i_x + i_image->m_width;
    DWORD y2 = i_y + i_image->m_height;

    x2 = std::min(m_width, x2);
    y2 = std::min(m_height, y2);

    if (x2 <= i_x || y2 <= i_y) return;

    DWORD width = x2 - i_x;
    for (DWORD y = i_y; y < y2; y++) memcpy(&m_data[y][i_x * 4], &i_image->m_data[y - i_y][0], width * 4);
  }
};


Image *readPng(const char *i_filename)
{
  FILE *fp = fopen(i_filename, "rb");
  if (!fp) return nullptr;

  constexpr int error = 0;

  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  Image *image = nullptr;

  try {
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) throw error;

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) throw error;

    if (setjmp(png_jmpbuf(png_ptr))) throw error;

    png_init_io(png_ptr, fp);
    png_read_png(png_ptr,
      info_ptr,
      (PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING |
       PNG_TRANSFORM_EXPAND),
      nullptr);

    uint32_t width, height;
    int bit_depth, color_type;
    png_get_IHDR(png_ptr,
      info_ptr,
      &width,
      &height,
      &bit_depth,
      &color_type,
      nullptr,
      nullptr,
      nullptr);

    image = new Image;
    image->resize(width, height);

    BYTE **row_pointers = png_get_rows(png_ptr, info_ptr);

    for (DWORD y = 0; y < height; y++) {
      BYTE *s = row_pointers[y];
      BYTE *d = image->m_data[y];
      for (DWORD x = 0; x < width; x++)
        switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
          *d++ = *s;
          *d++ = *s;
          *d++ = *s;
          *d++ = 255;
          s++;
          break;
        case PNG_COLOR_TYPE_GRAY_ALPHA: {
          BYTE v = static_cast<BYTE>(static_cast<WORD>(s[0]) * s[1] / 255);
          *d++ = v;
          *d++ = v;
          *d++ = v;
          s++;
          *d++ = *s;
          s++;
          break;
        }
        case PNG_COLOR_TYPE_PALETTE:
          printf("cannot handle palette\n");
          throw error;
          break;
        case PNG_COLOR_TYPE_RGB:
          *d++ = s[2];
          *d++ = s[1];
          *d++ = s[0];
          *d++ = 255;
          s += 3;
          break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
          *d++ = static_cast<BYTE>(static_cast<WORD>(s[2]) * s[3] / 255);
          *d++ = static_cast<BYTE>(static_cast<WORD>(s[1]) * s[3] / 255);
          *d++ = static_cast<BYTE>(static_cast<WORD>(s[0]) * s[3] / 255);
          *d++ = s[3];
          s += 4;
          break;
        }
    }
  } catch (...) {
    if (image) {
      delete image;
      image = nullptr;
    }
  }

  if (png_ptr)
    png_destroy_read_struct(&png_ptr,
      info_ptr ? &info_ptr : nullptr,
      nullptr);

  fclose(fp);

  return image;
}


//
class PNGAlphaViewer
{
  using DisplayList = std::list<std::string>;

  HWND m_hwnd;// 
  HDC m_memDc;// 
  BYTE *m_dib = nullptr;// DIB
  POINT m_point = {};// ウィンドウの位置
  SIZE m_size = {};// ウィンドウの大きさ
  DisplayList m_displayList;// 表示するリスト

  SIZE m_imageSize = {};// 絵の大きさ

  std::string m_error;// エラーメッセージ

  Image *m_image;// PNG 画像

  SPI::Manager m_spiManager;// SPI マネージャ
  HLOCAL m_hBm, m_hBInfo;// SPI で読んだ絵
  void *m_bm = nullptr;// 絵
  BITMAPINFO *m_bInfo = nullptr;// ヘッダ

  void update()
  {
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    POINT pt = { 0, 0 };
    UpdateLayeredWindow(m_hwnd,
      nullptr,
      &m_point,
      &m_size,
      m_memDc,
      &pt,
      RGB(0, 0, 0),
      &bf,
      ULW_ALPHA);
  }

  void setOpaque(RECT *i_rc)
  {
    // αを 0xff に。
    for (int y = i_rc->top; y < i_rc->bottom; y++) {
      BYTE *d = m_dib + (m_size.cx * (m_size.cy - y - 1) + i_rc->left) * 4;
      for (int x = i_rc->left; x < i_rc->right; x++) {
        d[3] = 0xff;
        d += 4;
      }
    }
  }

  void draw()
  {
    POINT cursorPt;
    GetCursorPos(&cursorPt);
    cursorPt.x -= m_point.x;
    cursorPt.y -= m_point.y;
    bool buttonLeft = GetKeyState(VK_LBUTTON) < 0;

    // 枠
    int frameW = GetSystemMetrics(SM_CXSIZEFRAME);
    int frameH = GetSystemMetrics(SM_CYSIZEFRAME);
    RECT rcFrame = { 0, 0, m_size.cx, m_size.cy };
    DrawFrameControl(m_memDc,
      &rcFrame,
      DFC_BUTTON,
      DFCS_BUTTONPUSH);

    // キャプション
    int captionH = GetSystemMetrics(SM_CYCAPTION);// なぜ？
    RECT rcCaption = { frameW, frameH,
                       m_size.cx - frameW, frameH + captionH - 1 };
    int flag = 0;
    if (GetForegroundWindow() == m_hwnd) flag = DC_ACTIVE;
    DrawCaption(m_hwnd,
      m_memDc,
      &rcCaption,
      flag | DC_ICON | DC_TEXT | DC_GRADIENT);

    // 閉じるボタン
    int closeH = captionH - 5;
    int closeW = closeH + 2;
    RECT rcClose = { rcCaption.right - 2 - closeW, rcCaption.top + 2,
                     rcCaption.right - 2, rcCaption.top + closeH + 2 };
    int pushed =
      (buttonLeft && PtInRect(&rcClose, cursorPt)) ? DFCS_PUSHED : 0;
    DrawFrameControl(m_memDc,
      &rcClose,
      DFC_CAPTION,
      DFCS_CAPTIONCLOSE | pushed);

    // 最大化ボタン
    rcClose.left -= closeW + 2;
    rcClose.right -= closeW + 2;
    pushed = (buttonLeft && PtInRect(&rcClose, cursorPt)) ? DFCS_PUSHED : 0;
    DrawFrameControl(m_memDc,
      &rcClose,
      DFC_CAPTION,
      pushed | (IsZoomed(m_hwnd)
                  ? DFCS_CAPTIONRESTORE
                  : DFCS_CAPTIONMAX));

    // 最小化ボタン
    rcClose.left -= closeW;
    rcClose.right -= closeW;
    pushed = (buttonLeft && PtInRect(&rcClose, cursorPt)) ? DFCS_PUSHED : 0;
    DrawFrameControl(m_memDc, &rcClose, DFC_CAPTION, pushed | DFCS_CAPTIONMIN);

    GdiFlush();

    // Gdi のαを設定
    RECT rcGdi = { 0, 0, m_size.cx, m_size.cy };
    setOpaque(&rcGdi);

    // 絵を描く
    if (hasImage()) {
      POINT ptDest = { rcCaption.left, rcCaption.top + captionH };
      SIZE size = { rcCaption.right - rcCaption.left,
                    m_size.cy - frameH - ptDest.y };
      SIZE imageSize = { std::min(size.cx, m_imageSize.cx),
                         std::min(size.cy, m_imageSize.cy) };

      if (m_image) {
        POINT ptSrc = { 0, 0 };
        for (int y = 0; y < imageSize.cy; y++) {
          BYTE *s = m_image->m_data[y + ptSrc.y] + ptSrc.x * 4;
          BYTE *d = m_dib + (m_size.cx * (m_size.cy - y - ptDest.y - 1)
                             + ptDest.x) * 4;
          memcpy(d, s, imageSize.cx * 4);
        }
      } else if (m_hBm && m_hBInfo) {
        StretchDIBits(m_memDc,
          ptDest.x,
          ptDest.y,
          imageSize.cx,
          imageSize.cy,
          0,
          0,
          imageSize.cx,
          imageSize.cy,
          m_bm,
          m_bInfo,
          DIB_RGB_COLORS,
          SRCCOPY);
        setOpaque(&rcGdi);
      } else if (isError()) {
        SetTextAlign(m_memDc, TA_TOP);
        RECT rc = { ptDest.x, ptDest.y,
                    ptDest.x + size.cx, ptDest.y + size.cy };
        FillRect(m_memDc, &rc, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
        TextOut(m_memDc, ptDest.x, ptDest.y, m_error.c_str(), m_error.size());
        setOpaque(&rcGdi);
      }

      if (size.cx < m_imageSize.cx ||
          size.cy < m_imageSize.cy) {
        RECT rcSize = { 0, 0, m_size.cx - frameW, m_size.cy - frameH };
        rcSize.left = rcSize.right - closeH;
        rcSize.top = rcSize.bottom - closeH;
        DrawFrameControl(m_memDc, &rcSize, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

        GdiFlush();
        setOpaque(&rcSize);
      }
    }

    update();
  }

  void resize()
  {
    RECT rc;
    GetWindowRect(m_hwnd, &rc);
    m_point.x = rc.left;
    m_point.y = rc.top;
    m_size.cx = rc.right - rc.left;
    m_size.cy = rc.bottom - rc.top;

    BITMAPINFOHEADER bi;
    memset(&bi, 0, sizeof(bi));
    bi.biSize = sizeof(bi);
    bi.biWidth = m_size.cx;
    bi.biHeight = m_size.cy;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    HBITMAP bitmap =
      CreateDIBSection(m_memDc,
        (BITMAPINFO *)&bi,
        DIB_RGB_COLORS,
        (void **)&m_dib,
        nullptr,
        0);
    DeleteObject(SelectObject(m_memDc, bitmap));

    draw();
  }

  bool load(const char *i_filename)
  {
    if (!i_filename) return false;

    // PNG のロード
    if (Image *image = readPng(i_filename)) {
      clear();
      m_image = image;
      m_imageSize.cx = m_image->m_width;
      m_imageSize.cy = m_image->m_height;
    } else {
      // PNG 以外のロード
      const SPI::PlugIn *plugIn = m_spiManager.getPlugIn(i_filename);
      if (!plugIn) {
        clear();
        m_error = "unknown format : ";
        m_error += i_filename;
        return false;
      }
      HLOCAL hBm, hBInfo;
      int error = plugIn->getPicture(i_filename, 0, 0, &hBInfo, &hBm, nullptr, 0);
      if (error != SPI::ERROR_NORMAL)
        if (plugIn->getPicture(i_filename, 128, 0, &hBInfo, &hBm, nullptr, 0)
            != SPI::ERROR_NORMAL) {
          clear();
          char buf[100];
          snprintf(buf, sizeof(buf), "(%d) ", error);
          m_error = buf;
          switch (error) {
          default:
          case SPI::ERROR_NORMAL:
          case SPI::ERROR_NOT_IMPLEMENTED:
          case SPI::ERROR_MEMORY:
          case SPI::ERROR_RESERVED:
          case SPI::ERROR_INTERNAL:
            m_error += "internal error";
            break;
          case SPI::ERROR_STOP_EXTRACT:
            m_error += "canceled by user";
            break;
          case SPI::ERROR_UNKNOWN_FORMAT:
            m_error += "unknown format";
            break;
          case SPI::ERROR_DAMAGED_DATA:
            m_error += "corrupted picture";
            break;
          case SPI::ERROR_OUT_OF_MEMORY:
            m_error += "out of memory";
            break;
          case SPI::ERROR_READ_FAILED:
            m_error += "cannot read file";
            break;
          }
          m_error += " : ";
          m_error += i_filename;
          return false;
        }
      clear();
      m_hBm = hBm;
      m_hBInfo = hBInfo;
      m_bm = LocalLock(m_hBm);
      m_bInfo = static_cast<BITMAPINFO *>(LocalLock(m_hBInfo));
      m_imageSize.cx = m_bInfo->bmiHeader.biWidth;
      m_imageSize.cy = m_bInfo->bmiHeader.biHeight;
    }
    return true;
  }

  void displayCurrent()
  {
    if (m_displayList.size()) {
      const char *filename = m_displayList.front().c_str();
      if (!load(filename) && isError())
        GetTextExtentPoint32(m_memDc,
          m_error.c_str(),
          m_error.size(),
          &m_imageSize);
      if (hasImage()) {
        if (isError())
          SetWindowText(m_hwnd, filename);
        else {
          char buf[1024];
          snprintf(buf,
            NUMBER_OF(buf),
            "%s (%dx%d)",
            filename,
            m_imageSize.cx,
            m_imageSize.cy);
          SetWindowText(m_hwnd, buf);
        }

        draw();

        MINMAXINFO mmi;
        wmGetMinMaxInfo(&mmi);
        MoveWindow(m_hwnd,
          m_point.x,
          m_point.y,
          mmi.ptMaxSize.x,
          mmi.ptMaxSize.y,
          true);
      }
    }
  }

  void displayNext()
  {
    if (isError()) {
      if (m_displayList.size()) m_displayList.pop_front();
      displayCurrent();
    } else {
      if (m_displayList.size()) {
        m_displayList.push_back(m_displayList.front());
        m_displayList.pop_front();
        displayCurrent();
      }
    }
  }

  void displayPrev()
  {
    if (isError() && m_displayList.size()) m_displayList.pop_front();
    if (m_displayList.size()) {
      m_displayList.push_front(m_displayList.back());
      m_displayList.pop_back();
      displayCurrent();
    }
  }

  bool isError() { return !m_error.empty(); }

  bool hasImage() { return m_image || (m_hBm && m_hBInfo) || isError(); }

  void clear()
  {
    m_error = "";
    delete m_image;
    m_image = nullptr;
    if (m_hBm) {
      LocalUnlock(m_hBm);
      LocalFree(m_hBm);
      m_hBm = nullptr;
    }
    if (m_hBInfo) {
      LocalUnlock(m_hBInfo);
      LocalFree(m_hBInfo);
      m_hBInfo = nullptr;
    }
    m_imageSize.cx = m_imageSize.cy = 1;
  }

  PNGAlphaViewer(HWND i_hwnd)
    : m_hwnd(i_hwnd),
      m_memDc(CreateCompatibleDC(nullptr)),
      m_image(nullptr),
      m_hBm(nullptr),
      m_hBInfo(nullptr)
  {
    m_spiManager.load();
    ASSERT(m_memDc);

    if (2 < __argc && std::string("-s") == __argv[1]) {
      std::vector<std::string> files;
      files.resize(__argc - 2);
      for (int i = 2; i < __argc; ++i) files[i - 2] = __argv[i];
      std::sort(files.begin(), files.end());
      m_displayList.insert(m_displayList.begin(), files.begin(), files.end());
    } else
      for (int i = 1; i < __argc; ++i) m_displayList.push_back(__argv[i]);
  }

  ~PNGAlphaViewer()
  {
    clear();
    DeleteDC(m_memDc);
  }

  // WM_CREATE
  int wmCreate(CREATESTRUCT * /* i_cs */)
  {
    resize();
    displayCurrent();
    return 0;
  }

  // WM_SIZE
  void wmSize(int i_flag, int i_width, int i_height) { resize(); }

  // WM_MOVE
  void wmMove(int /* i_x */, int /* i_y */)
  {
    RECT rc;
    GetWindowRect(m_hwnd, &rc);
    m_point.x = rc.left;
    m_point.y = rc.top;
  }

  // WM_MOVING
  void wmMoving(int i_flag, RECT *io_rect)
  {
    m_point.x = io_rect->left;
    m_point.y = io_rect->top;
  }

  // WM_ACTIVATE
  void wmActivate(int i_flag, HWND i_hwnd) { draw(); }

  // WM_CLOSE
  void wmClose() { PostQuitMessage(0); }

  // WM_GETMINMAXINFO
  void wmGetMinMaxInfo(MINMAXINFO *io_mmi)
  {
    if (hasImage()) {
      int frameW = GetSystemMetrics(SM_CXSIZEFRAME);
      int frameH = GetSystemMetrics(SM_CXSIZEFRAME);
      int captionH = GetSystemMetrics(SM_CYCAPTION);
      io_mmi->ptMaxSize.x = frameW * 2 + m_imageSize.cx;
      io_mmi->ptMaxSize.y = frameH * 2 + captionH + m_imageSize.cy;
      io_mmi->ptMaxTrackSize = io_mmi->ptMaxSize;
    }
  }

  // WM_DESTRY
  void wmDestroy() {}

  // WM_NCLBUTTONDOWN
  bool wmNcLButtonDown(int i_hitTest, POINT i_point)
  {
    if (i_hitTest == HTCLOSE ||
        i_hitTest == HTMAXBUTTON ||
        i_hitTest == HTMINBUTTON)
      draw();
    return false;
  }

  // WM_DROPFILES
  void wmDropFiles(HDROP i_hDrop)
  {
    size_t n = DragQueryFile(i_hDrop, -1, nullptr, 0);
    std::vector<std::string> files;
    files.resize(n);
    for (size_t i = 0; i < n; ++i) {
      char buf[1024];
      DragQueryFile(i_hDrop, i, buf, NUMBER_OF(buf));
      files[i] = buf;
    }
    std::sort(files.begin(), files.end());
    m_displayList.insert(m_displayList.begin(), files.begin(), files.end());
    displayCurrent();
    DragFinish(i_hDrop);
  }

  // WM_KEYDOWN
  bool wmKeyDown(UINT i_vkey, UINT i_flags)
  {
    switch (i_vkey) {
    case VK_SPACE:
    case VK_RETURN:
    case 'N':
      displayNext();
      return true;
    case VK_BACK:
    case VK_DELETE:
    case 'P':
      displayPrev();
      return true;
    case VK_ESCAPE:
    case 'Q':
      PostQuitMessage(0);
      return true;
    }
    return false;
  }

  //
  static LRESULT CALLBACK wndProc(HWND i_hwnd,
    UINT i_message,
    WPARAM i_wParam,
    LPARAM i_lParam)
  {
    auto This = (PNGAlphaViewer *)GetWindowLongPtr(i_hwnd, 0);
    if (!This)
      switch (i_message) {
      case WM_CREATE:
        This = new PNGAlphaViewer(i_hwnd);
        SetWindowLongPtr(i_hwnd, 0, (LONG_PTR)This);
        return This->wmCreate((CREATESTRUCT *)i_lParam);
      }
    else
      switch (i_message) {
      case WM_CLOSE:
        This->wmClose();
        return 0;
      case WM_ACTIVATE:
        This->wmActivate(i_wParam, (HWND)i_lParam);
        return 0;
      case WM_SIZE:
        This->wmSize(i_wParam, LOWORD(i_lParam), HIWORD(i_lParam));
        return 0;
      case WM_MOVE:
        This->wmMove(LOWORD(i_lParam), HIWORD(i_lParam));
        return 0;
      case WM_MOVING:
        This->wmMoving(i_wParam, (RECT *)i_lParam);
        return 0;
      case WM_GETMINMAXINFO:
        This->wmGetMinMaxInfo((MINMAXINFO *)i_lParam);
        return 0;
      case WM_DESTROY:
        This->wmDestroy();
        return 0;
      case WM_NCDESTROY:
        delete This;
        return 0;
      case WM_NCLBUTTONDOWN:
        if (This->wmNcLButtonDown(i_wParam, *(POINT *)&i_lParam)) return 0;
        break;
      case WM_KEYDOWN:
        if (This->wmKeyDown(i_wParam, i_lParam)) return 0;
        break;
      case WM_DROPFILES:
        This->wmDropFiles((HDROP)i_wParam);
        return 0;
      }
    return DefWindowProc(i_hwnd, i_message, i_wParam, i_lParam);
  }

public:
  // register class
  static ATOM registerClass()
  {
    WNDCLASS wc;
    wc.style = 0;
    wc.lpfnWndProc = wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(PNGAlphaViewer *);
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_PNGAV));
    wc.hCursor = LoadCursor(nullptr, MAKEINTRESOURCE(IDC_HAND));
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "PNGAlphaViewer";
    ATOM atom = RegisterClass(&wc);
    ASSERT(atom != 0);
    return atom;
  }
};


// main
int WINAPI WinMain(HINSTANCE i_hInstance,
  HINSTANCE /* i_hPrevInstance */,
  LPSTR /* i_lpszCmdLine */,
  int /* i_nCmdShow */)
{
  hInst = i_hInstance;

  CHECK(!!, setlocale(LC_ALL, ""));

  PNGAlphaViewer::registerClass();

  HWND hwndPNGAlphaViewer
    = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_LAYERED,
      "PNGAlphaViewer",
      nullptr,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      100,
      100,
      nullptr,
      nullptr,
      hInst,
      nullptr);
  ShowWindow(hwndPNGAlphaViewer, SW_SHOWDEFAULT);

  MSG msg;
  while (0 < GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  DestroyWindow(hwndPNGAlphaViewer);

  return msg.wParam;
}
