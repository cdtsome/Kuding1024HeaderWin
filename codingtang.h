
#ifdef WIN10
#define WINVER 0x0A000003
#define _WIN32_WINNT 0x0A000003
#endif

#if __cplusplus < 201103L
#error should use C++ 11 implementation
#endif

#include <map>
#include <iostream>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <vector>
#include <thread>
#include <gdiplus.h>
#include <queue>
#include <mutex>
#include <windowsx.h>

using namespace std;

void log(const string& msg) {
	cout << msg;
}

struct Rect {
	int x, y;       // 坐标，x,y
	int w, h;       // 宽高
};
typedef Rect rect_type;

struct Color {
	int r, g, b, a;
};
typedef Color color_type;

struct Point {
	short x, y;
};
typedef Point point_type;

/* ********************************Event start******************************** */

#define CDT_KEYDOWN 0
#define CDT_KEYUP 1
#define CDT_MOUSEBUTTONDOWN 2
#define CDT_MOUSEBUTTONUP 3
#define CDT_QUIT 114514

#define CDT_KEY_0 '0'
#define CDT_KEY_1 '1'
#define CDT_KEY_2 '2'
#define CDT_KEY_3 '3'
#define CDT_KEY_4 '4'
#define CDT_KEY_5 '5'
#define CDT_KEY_6 '6'
#define CDT_KEY_7 '7'
#define CDT_KEY_8 '8'
#define CDT_KEY_9 '9'

#define CDT_KEY_a 'A'
#define CDT_KEY_b 'B'
#define CDT_KEY_c 'C'
#define CDT_KEY_d 'D'
#define CDT_KEY_e 'E'
#define CDT_KEY_f 'F'
#define CDT_KEY_g 'G'
#define CDT_KEY_h 'H'
#define CDT_KEY_i 'I'
#define CDT_KEY_j 'J'
#define CDT_KEY_k 'K'
#define CDT_KEY_l 'L'
#define CDT_KEY_m 'M'
#define CDT_KEY_n 'N'
#define CDT_KEY_o 'O'
#define CDT_KEY_p 'P'
#define CDT_KEY_q 'Q'
#define CDT_KEY_r 'R'
#define CDT_KEY_s 'S'
#define CDT_KEY_t 'T'
#define CDT_KEY_u 'U'
#define CDT_KEY_v 'V'
#define CDT_KEY_w 'W'
#define CDT_KEY_x 'X'
#define CDT_KEY_y 'Y'
#define CDT_KEY_z 'Z'

#define CDT_KEY_RIGHT VK_RIGHT
#define CDT_KEY_LEFT VK_LEFT
#define CDT_KEY_DOWN VK_DOWN
#define CDT_KEY_UP VK_UP
#define CDT_KEY_BACKSPACE VK_BACK
#define CDT_KEY_ESCAPE VK_ESCAPE
#define CDT_KEY_RETURN VK_RETURN
#define CDT_KEY_TAB VK_TAB
#define CDT_KEY_SPACE VK_SPACE
#define CDT_KEY_CTRLDOWN VK_RCONTROL
#define CDT_KEY_CTRLUP VK_LCONTROL
#define CDT_KEY_ALTDOWN VK_RMENU
#define CDT_KEY_ALTUP VK_LMENU
#define CDT_KEY_SHIFTDOWN VK_RSHIFT
#define CDT_KEY_SHIFTUP VK_LSHIFT

#define CDT_BUTTON_LEFT VK_LBUTTON
#define CDT_BUTTON_RIGHT VK_RBUTTON
#define CDT_PRESSED 0x11
#define CDT_RELEASED 0x12

#define KEY_0 '0'
#define KEY_1 '1'
#define KEY_2 '2'
#define KEY_3 '3'
#define KEY_4 '4'
#define KEY_5 '5'
#define KEY_6 '6'
#define KEY_7 '7'
#define KEY_8 '8'
#define KEY_9 '9'

#define KEY_a 'A'
#define KEY_b 'B'
#define KEY_c 'C'
#define KEY_d 'D'
#define KEY_e 'E'
#define KEY_f 'F'
#define KEY_g 'G'
#define KEY_h 'H'
#define KEY_i 'I'
#define KEY_j 'J'
#define KEY_k 'K'
#define KEY_l 'L'
#define KEY_m 'M'
#define KEY_n 'N'
#define KEY_o 'O'
#define KEY_p 'P'
#define KEY_q 'Q'
#define KEY_r 'R'
#define KEY_s 'S'
#define KEY_t 'T'
#define KEY_u 'U'
#define KEY_v 'V'
#define KEY_w 'W'
#define KEY_x 'X'
#define KEY_y 'Y'
#define KEY_z 'Z'

#define KEY_RIGHT VK_RIGHT
#define KEY_LEFT VK_LEFT
#define KEY_DOWN VK_DOWN
#define KEY_UP VK_UP
#define KEY_BACKSPACE VK_BACK
#define KEY_ESCAPE VK_ESCAPE
#define KEY_RETURN VK_RETURN
#define KEY_TAB VK_TAB
#define KEY_SPACE VK_SPACE
#define KEY_CTRLDOWN VK_RCONTROL
#define KEY_CTRLUP VK_LCONTROL
#define KEY_ALTDOWN VK_RMENU
#define KEY_ALTUP VK_LMENU
#define KEY_SHIFTDOWN VK_RSHIFT
#define KEY_SHIFTUP VK_LSHIFT

typedef unsigned __int32 Uint32;
typedef unsigned __int8 Uint8;
typedef __int32 Sint32;
typedef __int32 int32_t;

struct MouseButtonEvent {
	Uint32 type;   // CDT_MOUSEBUTTONDOWN or CDT_MOUSEBUTTONUP
	Uint32 timestamp;   /**当前时间,毫秒 */
	Uint8 button;       /**< 按键索引,CDT_BUTTON_LEFT:左键, CDT_BUTTON_RIGHT:右键 */
	Uint8 state;        /**< ::CDT_PRESSED or ::CDT_RELEASED */
	Uint8 clicks;       /**< 1 for single-click, 2 for double-click, etc. */
	Sint32 x;           /**< X coordinate, relative to window */
	Sint32 y;           /**< Y coordinate, relative to window */
};

struct Event {
	Uint32 type;                // 事件类型
	int32_t keyCode;            // 按键
	MouseButtonEvent button;    // 按钮事件
};

namespace {
	queue<Event> eq;
	mutex em;
}

// 字符串转为宽字符，为了打印单个汉字
wstring toWstring(const string& str) {
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}

// 宽字符转为字符串
string toString(const wchar_t chr) {
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	wstring wstr;
	wstr += chr;
	return converter.to_bytes(wstr);
}


/* ********************************Window start******************************** */

struct Window {
	private:
		struct Items {
			string Type;
			Color color;
			struct __Item {
				struct __Point {
					int x, y;
				} Point;
				struct __Line {
					int x1, y1, x2, y2;
				} Line;
				struct __Rect {
					int x, y, w, h;
				} Rect;
				struct __Text {
					string text;
					int fontSize;
					Color color;
					int x, y;
				} Text;
				struct __TextEx {
					string fontPath;
					string text;
					int fontSize;
					Color color;
					int x, y;
				} TextEx;
				struct __Image {
					string imgPath;
					int x, y;
				} Image;
				struct __ImageEx {
					string imgPath;
					rect_type src;
					rect_type dst;
				} ImageEx;
				struct ImageEx2 {
					string imgPath;
					rect_type src;
					rect_type dst;
					double angle;
					point_type center;
				};
			} Item;
		};
		static vector<Items>* items;
		static vector<Items> static_items;
		static HWND hwnd;
		static Color color;
		static map<string, Gdiplus::Bitmap*> bmps;
		static map<string, HBITMAP> hbmps;
		static mutex im;
		static void print(HWND hwnd) {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			HDC hMemDc = CreateCompatibleDC(hdc);
			HBITMAP hBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
			HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDc, hBmp);
			HDC hMemDC = CreateCompatibleDC(0);

			im.lock();
			vector<Items> a = static_items;
			im.unlock();
			for (Items i : a) {
				HDC hdc = hMemDc;
				if (i.Type == "Point") {
					SetPixel(hdc, i.Item.Point.x, i.Item.Point.y, RGB(i.color.r, i.color.g, i.color.b));
				} else if (i.Type == "Line") {
					HPEN gPen = CreatePen(PS_SOLID, 1, RGB(i.color.r, i.color.g, i.color.b));
					HPEN oPen = (HPEN)SelectObject(hdc, gPen);
					MoveToEx(hdc, i.Item.Line.x1, i.Item.Line.y1, NULL);
					LineTo(hdc, i.Item.Line.x2, i.Item.Line.y2);
				} else if (i.Type == "Text") {
					LOGFONT logfont;
					ZeroMemory(&logfont, sizeof(LOGFONT));
					logfont.lfHeight = i.Item.Text.fontSize;
					logfont.lfCharSet = DEFAULT_CHARSET;
					HFONT hFont = CreateFontIndirect(&logfont);
					SelectObject(hdc, hFont);
					SetTextColor(hdc, RGB(i.Item.Text.color.r, i.Item.Text.color.g, i.Item.Text.color.b));
					SetBkColor(hdc, RGB(0, 0, 0));
#ifdef OPAQUETEXT
					SetBkMode(hdc, OPAQUE);
#else
					SetBkMode(hdc, TRANSPARENT);
#endif
					TextOut(hdc, i.Item.Text.x, i.Item.Text.y, i.Item.Text.text.c_str(), i.Item.Text.text.size());
					DeleteObject(hFont);
				} else if (i.Type == "Rect") {
					HBRUSH hbr = CreateSolidBrush(RGB(i.color.r, i.color.g, i.color.b));
					RECT rect;
					rect.left = i.Item.Rect.x, rect.top = i.Item.Rect.y;
					rect.right = i.Item.Rect.w + i.Item.Rect.x;
					rect.bottom = i.Item.Rect.h + i.Item.Rect.y;
					FillRect(hdc, &rect, hbr);
					DeleteObject(hbr);
				} else if (i.Type == "TextEx") {
					LOGFONT logfont;
					ZeroMemory(&logfont, sizeof(LOGFONT));
					logfont.lfHeight = i.Item.TextEx.fontSize;
					strcpy(logfont.lfFaceName, i.Item.TextEx.fontPath.c_str());
					HFONT hFont = CreateFontIndirect(&logfont);
					SelectObject(hdc, hFont);
					SetTextColor(hdc, RGB(i.Item.TextEx.color.r, i.Item.TextEx.color.g, i.Item.TextEx.color.b));
					SetBkColor(hdc, RGB(0, 0, 0));
					SetBkMode(hdc, TRANSPARENT);
					TextOut(hdc, i.Item.TextEx.x, i.Item.TextEx.y, i.Item.TextEx.text.c_str(), i.Item.TextEx.text.size());
				} else if (i.Type == "Image") {
					Gdiplus::Bitmap* bmp = bmps[i.Item.Image.imgPath];
					SIZE sBmp = {LONG(bmp->GetWidth()), LONG(bmp->GetHeight())};
					HBITMAP hBmp = hbmps[i.Item.Image.imgPath];
					BLENDFUNCTION bf = {0, 0, 255, AC_SRC_ALPHA};
					SelectObject(hMemDC, hBmp);
					AlphaBlend(hdc, i.Item.Image.x, i.Item.Image.y, sBmp.cx, sBmp.cy,
					           hMemDC, 0, 0, sBmp.cx, sBmp.cy, bf);
				} else if (i.Type == "ImageEx") {
					Gdiplus::Bitmap* bmp = bmps[i.Item.ImageEx.imgPath];
					SIZE sBmp = {LONG(bmp->GetWidth()), LONG(bmp->GetHeight())};
					HBITMAP hBmp = hbmps[i.Item.ImageEx.imgPath];
					BLENDFUNCTION bf = {0, 0, 255, AC_SRC_ALPHA};
					SelectObject(hMemDC, hBmp);
					if (i.Item.ImageEx.dst.h == 0 && i.Item.ImageEx.dst.w == 0) {
						AlphaBlend(hdc, i.Item.ImageEx.dst.x, i.Item.ImageEx.dst.y, i.Item.ImageEx.src.w,
						           i.Item.ImageEx.src.h, hMemDC, i.Item.ImageEx.src.x, i.Item.ImageEx.src.y,
						           min((int)sBmp.cx, i.Item.ImageEx.src.w), min((int)sBmp.cy, i.Item.ImageEx.src.h), bf);
					} else {
						AlphaBlend(hdc, i.Item.ImageEx.dst.x, i.Item.ImageEx.dst.y, i.Item.ImageEx.dst.w,
						           i.Item.ImageEx.dst.h, hMemDC, i.Item.ImageEx.src.x, i.Item.ImageEx.src.y,
						           min((int)sBmp.cx, i.Item.ImageEx.src.w), min((int)sBmp.cy, i.Item.ImageEx.src.h), bf);
					}
				}
			}

			DeleteDC(hMemDC);
			BitBlt(hdc, 0, 0, rc.right, rc.bottom, hMemDc, 0, 0, SRCCOPY);
			SelectObject(hMemDc, hOldBmp);
			DeleteObject(hBmp);
			DeleteObject(hMemDc);
			EndPaint(hwnd, &ps);
		}
		static clock_t lastPrint;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			switch (uMsg) {
				case WM_DESTROY: //关闭
					PostQuitMessage(0);
					Gdiplus::GdiplusShutdown(m_gdiplusToken);
					exit(0);
					return 0;
				case WM_LBUTTONDOWN: //左键按下
					em.lock();
					eq.push(Event{CDT_MOUSEBUTTONDOWN, 0,
						{CDT_MOUSEBUTTONDOWN, 0, CDT_BUTTON_LEFT, CDT_PRESSED, 1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}});
					em.unlock();
					return 0;
				case WM_LBUTTONUP: //左键抬起
					em.lock();
					eq.push(Event{CDT_MOUSEBUTTONDOWN, 0,
						{CDT_MOUSEBUTTONDOWN, 0, CDT_BUTTON_LEFT, CDT_RELEASED, 1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}});
					em.unlock();
					return 0;
				case WM_LBUTTONDBLCLK: //左键双击
					em.lock();
					eq.push(Event{CDT_MOUSEBUTTONDOWN, 0,
						{CDT_MOUSEBUTTONDOWN, 0, CDT_BUTTON_LEFT, CDT_PRESSED, 2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}});
					em.unlock();
					return 0;
				case WM_RBUTTONDOWN: //右键按下
					em.lock();
					eq.push(Event{CDT_MOUSEBUTTONDOWN, 0,
						{CDT_MOUSEBUTTONDOWN, 0, CDT_BUTTON_RIGHT, CDT_PRESSED, 1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}});
					em.unlock();
					return 0;
				case WM_RBUTTONUP: //右键抬起
					em.lock();
					eq.push(Event{CDT_MOUSEBUTTONDOWN, 0,
						{CDT_MOUSEBUTTONDOWN, 0, CDT_BUTTON_RIGHT, CDT_RELEASED, 1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}});
					em.unlock();
					return 0;
				case WM_RBUTTONDBLCLK: //右键双击
					em.lock();
					eq.push(Event{CDT_MOUSEBUTTONDOWN, 0,
						{CDT_MOUSEBUTTONDOWN, 0, CDT_BUTTON_RIGHT, CDT_PRESSED, 2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}});
					em.unlock();
					return 0;
				case WM_KEYDOWN: //键盘按下
					em.lock();
					eq.push(Event{CDT_KEYDOWN, int(wParam)});
					em.unlock();
					return 0;
				case WM_KEYUP: //键盘抬起
					em.lock();
					eq.push(Event{CDT_KEYUP, int(wParam)});
					em.unlock();
					return 0;
				case WM_ERASEBKGND: //防止重绘背景
					return 0;
				case WM_PAINT: //重绘
					print(hwnd);
					return 0;
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		static ULONG_PTR m_gdiplusToken;
		static void Message(int x, int y) {
			WNDCLASS wc = {};
			wc.lpfnWndProc   = WindowProc;
			wc.hInstance     = GetModuleHandle(NULL);
			wc.lpszClassName = "Codingtang Apps";
			wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
			wc.style = CS_DBLCLKS;
			RegisterClass(&wc);
			hwnd = CreateWindowEx(0, "Codingtang Apps", "Codingtang App",
			                      WS_VISIBLE | WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
			                      CW_USEDEFAULT, CW_USEDEFAULT, x, y, NULL, NULL, GetModuleHandle(NULL), NULL);
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0) > 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	public:
		void __start(int x, int y) {
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
			items = new vector<Items>;
			thread msg(Message, x, y);
			msg.detach();
		}

		// 设置画笔颜色
		void drawColor(int r, int g, int b, int a) {
			color = Color{r, g, b, a};
		}

		// 画一个点
		void drawPoint(int x, int y) {
			Items a;
			a.Type = "Point";
			a.color = color;
			a.Item.Point.x = x;
			a.Item.Point.y = y;
			items->push_back(a);
#ifdef NOPRESENT
			present();
#endif
		}

		// 画一条直线
		void drawLine(int x1, int y1, int x2, int y2) {
			Items a;
			a.Type = "Line";
			a.color = color;
			a.Item.Line.x1 = x1;
			a.Item.Line.y1 = y1;
			a.Item.Line.x2 = x2;
			a.Item.Line.y2 = y2;
			items->push_back(a);
#ifdef NOPRESENT
			present();
#endif
		}

		// 画一个矩形
		void fillRect(int x, int y, int w, int h) {
			Items a;
			a.Type = "Rect";
			a.color = color;
			a.Item.Rect.x = x, a.Item.Rect.y = y;
			a.Item.Rect.w = w, a.Item.Rect.h = h;
			items->push_back(a);
#ifdef NOPRESENT
			present();
#endif
		}

		void drawText(const char* text, int fontSize, const Color& color, int x, int y) {
			Items a;
			a.Type = "Text";
			a.color = color;
			a.Item.Text.text = text;
			a.Item.Text.fontSize = fontSize;
			a.Item.Text.color = color;
			a.Item.Text.x = x, a.Item.Text.y = y;
			items->push_back(a);
#ifdef NOPRESENT
			present();
#endif
		}

		void drawText(const char* fontPath, const char* text, int fontSize, const Color& color, int x, int y) {
			Items a;
			a.Type = "TextEx";
			a.color = color;
			a.Item.TextEx.text = text;
			a.Item.TextEx.fontSize = fontSize;
			a.Item.TextEx.color = color;
			a.Item.TextEx.x = x, a.Item.TextEx.y = y;
			a.Item.TextEx.fontPath = fontPath;
			items->push_back(a);
#ifdef NOPRESENT
			present();
#endif
		}

		void drawImage(const char* imgPath, int x, int y) {
			Items a;
			a.Type = "Image";
			a.color = color;
			a.Item.Image.imgPath = imgPath;
			a.Item.Image.x = x, a.Item.Image.y = y;
			if (bmps[a.Item.Image.imgPath] == NULL) {
				Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(toWstring(a.Item.Image.imgPath).c_str());
				bmps[a.Item.Image.imgPath] = bmp;
				HBITMAP hBmp;
				bmp->GetHBITMAP(Gdiplus::Color(Gdiplus::Color::AlphaMask), &hBmp);
				hbmps[a.Item.Image.imgPath] = hBmp;
			}
			items->push_back(a);
#ifdef NOPRESENT
			present();
#endif
		}

		/*
		* 画图
		* @param imgPath 图片名称
		* @param src 图片要渲染的区域, NULL表示整个图片
		* @param dst 屏幕的区域,NULL表示从左上角渲染整个图片
		*/
		void drawImageEx(const char* imgPath, Rect* src, Rect* dst) {
			Items a;
			a.Type = "ImageEx";
			a.color = color;
			a.Item.ImageEx.imgPath = imgPath;
			if (bmps[a.Item.ImageEx.imgPath] == NULL) {
				Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(toWstring(a.Item.ImageEx.imgPath).c_str());
				bmps[a.Item.ImageEx.imgPath] = bmp;
				HBITMAP hBmp;
				bmp->GetHBITMAP(Gdiplus::Color(Gdiplus::Color::AlphaMask), &hBmp);
				hbmps[a.Item.ImageEx.imgPath] = hBmp;
			}
			if (src == NULL) {
				a.Item.ImageEx.src = Rect{0, 0, int(bmps[a.Item.ImageEx.imgPath]->GetWidth()),
				                          int(bmps[a.Item.ImageEx.imgPath]->GetHeight())};
			} else {
				a.Item.ImageEx.src = *src;
			}
			if (dst == NULL) {
				a.Item.ImageEx.dst = Rect{0, 0, int(bmps[a.Item.ImageEx.imgPath]->GetWidth()),
				                          int(bmps[a.Item.ImageEx.imgPath]->GetHeight())};
			} else {
				a.Item.ImageEx.dst = *dst;
			}
			items->push_back(a);
#ifdef NOPRESENT
			present();
#endif
		}
		void drawImageExt(const char* imgPath, Rect src, Rect dst) {
			drawImageEx(imgPath, &src, &dst);
		}

		/*
		* 画图+旋转
		* @param imgPath 图片名称
		* @param src 图片要渲染的区域, NULL表示整个图片
		* @param dst 屏幕的区域,NULL表示从左上角渲染整个图片
		* @param angle 图片渲染至屏幕以后，旋转的角度
		* @param center 图片旋转的中心点，为NULL默认中心点为是图片的宽度/2, 图片的高度/2
		*/
		void drawImageEx2(const char* imgPath, Rect* src, Rect* dst, double angle, Point* center) {

		}

		void playMusic(const char* musicPath, int loops) {

		}

		void playSound(const char* soundPath,  int loops) {

		}

		void present() {
			im.lock();
			static_items = *items;
			im.unlock();
			InvalidateRect(hwnd, NULL, 1);
			UpdateWindow(hwnd);
#ifndef NOPRESENT
#ifdef AUTOCLEAR
			clear();
#endif
#endif
		}

		void clear() {
			delete items;
			items = new vector<Items>();
		}

		void quit() {
			DestroyWindow(hwnd);
			delete items;
			Gdiplus::GdiplusShutdown(m_gdiplusToken);
			exit(0);
		}
};
vector<Window::Items>* Window::items;
vector<Window::Items> Window::static_items;
HWND Window::hwnd;
Color Window::color;
ULONG_PTR Window::m_gdiplusToken;
map<string, Gdiplus::Bitmap*> Window::bmps;
map<string, HBITMAP> Window::hbmps;
clock_t Window::lastPrint = 0;
mutex Window::im;

// 获取窗口创建以后的毫秒数
clock_t st = clock();
int getTicks() {
	return clock() - st;
}

/*
 * 创建一个窗口
 * width: 窗口宽度
 * height: 窗口高度
 * */
namespace {
	bool __created__ = false;
}
Window createWindow(int width, int height) {
	Window w;
	if (!__created__) {
		w.__start(width, height);
		__created__ = true;
	}
	return w;
}

/* ********************************Window end******************************** */

bool pollEvent(Event* event) {
	em.lock();
	if (eq.empty()) {
		em.unlock();
		return false;
	}
	*event = eq.front();
	eq.pop();
	em.unlock();
	return true;
}

bool waitEventTimeout(Event* event, int timeout) {
	clock_t st = clock();
	while (true) {
		if (clock() - st >= timeout) return false;
		em.lock();
		if (!eq.empty()) {
			*event = eq.front();
			eq.pop();
			em.unlock();
			return true;
		}
		em.unlock();
		Sleep(50);
	}
}


/* ********************************Console start******************************** */

const Color COLOR_RED = Color{ 255, 0, 0};
const Color COLOR_GREEN = Color{ 0, 255, 0};
const Color COLOR_YELLOW = Color{ 255, 255, 0};
const Color COLOR_BLUE = Color{ 0, 0, 255};
const Color COLOR_WHITE = Color{ 255, 255, 255 };
const Color COLOR_BLACK = Color{ 0, 0, 0};
const Color COLOR_CYAN = Color{ 0, 255, 255};

// 创建窗口
void createConsole(const char* title) {
	SetConsoleTitle(title);
#ifdef WIN10
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwOriginalOutMode = 0;
	GetConsoleMode(hOut, &dwOriginalOutMode);
	DWORD dwOriginalInMode = 0;
	GetConsoleMode(hIn, &dwOriginalInMode);
	DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	DWORD dwRequestedInModes = ENABLE_VIRTUAL_TERMINAL_INPUT;
	DWORD dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
	SetConsoleMode(hOut, dwOutMode);
	DWORD dwInMode = dwOriginalInMode | dwRequestedInModes;
	SetConsoleMode(hIn, dwInMode);
#endif
	system("color 0F");
}

// 设置文字颜色
void setTextColor(const Color& color) {
#ifdef WIN10
	if (color.a == 0) {
		string cmd = "\x1b[38;2;" + to_string(color.r) + ";" + to_string(color.g) + ";" + to_string(color.b) + "m";
		printf(cmd.c_str());
	}
#endif
}

// 设置背景颜色
void setBackgroundColor(const Color& color) {
#ifdef WIN10
	if (color.a == 0) {
		string cmd = "\x1b[48;2;" + to_string(color.r) + ";" + to_string(color.g) + ";" + to_string(color.b) + "m";
		printf(cmd.c_str());
	}
#endif
}

// 休眠
void sleep(int millionSeconds) {
	Sleep(millionSeconds);
}

// 清屏
void cls() {
	system("cls");
}

// 打印
void print(const char* str, ...) {
	va_list v;
	va_start(v, str);
	vprintf(str, v);
	va_end(v);
}

// 打印
void print(const string& str) {
	cout << str;
}

// 打印
void print(const wchar_t* str, ...) {
	va_list v;
	va_start(v, str);
	vwprintf(str, v);
	va_end(v);
}

// cin的改版，获取int
int getInt() {
	int ret;
	cin >> ret;
	return ret;
}

// cin的改版，获取sring
string getString() {
	string ret;
	cin >> ret;
	return ret;
}

/**
 * 获取单个字符
 * @return 获取单个字符
 */
char getChar() {
	return getchar();
}

// 结束窗口
void quit() {
	exit(0);
}

/* ********************************Console end******************************** */
