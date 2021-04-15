// Another Color aimbot to identify shapes

#include "Windows.h"
#include "vector"

using namespace std;

HWND hwnd = 0;


POINT a, b; // top left and bottom right corners
int screenWidth; //width
int screenHeight; //height
const int width = 400; //aimfov width
const int height = 320; //aimfov height

RGBQUAD* capture(POINT a, POINT b) {
	// copy screen to bitmap
	HDC     hScreen = GetDC(hwnd);
	//HDC     hScreen = GetDC(NULL); //when using desktop
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, abs(b.x - a.x), abs(b.y - a.y));
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, abs(b.x - a.x), abs(b.y - a.y), hScreen, a.x, a.y, SRCCOPY); // BitBlt does the copying

	/*
	// save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();
	*/

	// Array conversion:
	RGBQUAD* pixels = new RGBQUAD[width * height];

	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = width;
	bmi.biHeight = -height;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;// 3 * ScreenX * ScreenY;

	GetDIBits(hDC, hBitmap, 0, height, pixels, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(hwnd, hScreen);
	//ReleaseDC(NULL, hScreen); //when using desktop
	DeleteObject(hBitmap);
	return pixels;
}

void shoot() {
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); // start left click
	Sleep(10);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); // finish Left click
	Sleep(10);
}

int targetRed, targetGreen, targetBlue;
bool checkColour(RGBQUAD sample, vector<RGBQUAD> targets) {
	bool result = false;
	int tolerance = 50; //color tolerance
	int sampleRed = (int)sample.rgbRed;
	int sampleGreen = (int)sample.rgbGreen;
	int sampleBlue = (int)sample.rgbBlue;
	//int targetRed, targetGreen, targetBlue;
	//for (int i = 0; i < targets.size(); i++) {
	//	targetRed = targets[i].rgbRed;
	//	targetGreen = targets[i].rgbGreen;
	//	targetBlue = targets[i].rgbBlue;
	//	if (abs(sampleRed - targetRed) < tolerance && 
	//		abs(sampleGreen - targetGreen) < tolerance &&
	//		abs(sampleBlue - targetBlue) < tolerance ) {
	//		result = true;
	//		break;
	//	}
	//}

	//scan for magenta color (red + blue = magenta) <--------------
	if (sampleRed > 120 && sampleRed <= 255 &&//red color 
		sampleGreen > 0 && sampleGreen < 80 &&
		sampleBlue > 120 && sampleBlue <= 255) { //blue color
		result = true;
	}

	//scan for red color
	//if (sampleRed > 120 && sampleRed <= 255 && //scan for red color <--------------
		//sampleGreen > 0 && sampleGreen < 80 &&
		//sampleBlue > 0 && sampleBlue < 80) {
		//result = true;
	//}

	//scan for green color
	//if (sampleRed > 0 && sampleRed < 80 && //scan for green color <--------------
		//sampleGreen > 120 && sampleGreen <= 255 &&
		//sampleBlue > 0 && sampleBlue < 80) {
		//result = true;
	//}

	return result;
}

RGBQUAD target;
void Aim() {
	RGBQUAD* pixels;
	POINT targetPos; // centered at top left corner of capture zone

	vector<RGBQUAD> targets = {};
	//target.rgbRed = 161, target.rgbGreen = 22, target.rgbBlue = 42;
	//targets.push_back(target);
	//target.rgbRed = 165, target.rgbGreen = 19, target.rgbBlue = 19;
	//targets.push_back(target);
	//target.rgbRed = 144, target.rgbGreen = 35, target.rgbBlue = 39;
	//targets.push_back(target);
	//target.rgbRed = 122, target.rgbGreen = 35, target.rgbBlue = 32;
	//targets.push_back(target);

	const int sampleCount = 16, reboundMax = 3;
	double radius, angle;
	int reboundCount = 0;
	int x, y, index, xAdjust, yAdjust; // 0 indexed from top left
	// int red, green, blue;

	bool targetAcquired = false;
	//bool evadeCrosshairColour = false;
	//int brightest = 0;
	while (true) {
		// angle = 2 * 3.141592654 / 8;
		angle = 2 * 3.141592654 * 3 / 4;
		radius = 1;

		if (GetKeyState(VK_SHIFT) & 0x8000) { //<- SHIFT is aimkey
		//if ((GetKeyState(VK_SHIFT) & 0x100) != 0) {
			pixels = capture(a, b);
			targetAcquired = false;
			//evadeCrosshairColour = false;
			for (int i = 0; i < sampleCount * width; i++) {
				x = (int)(radius * cos(angle) + width / 2);
				y = (int)(radius * sin(angle) + height / 2);
				if (i % sampleCount == 0) { // if ring is complete
					radius++; // increment radius
				}
				angle += 2 * 3.141592654 / sampleCount; // increment angle per iteration
				if (x < 0 || x > width - 1 || y < 0 || y > height - 1) { // boundary check
					break;
				}
				//if (radius > 45 && (abs(width / 2 - x) <= 2 || abs(height / 2 - y) <= 2)) { 
				//	evadeCrosshairColour = true;
				//}
				index = y * width + x; // get 1d array index
				//red = (int)pixels[index].rgbRed;
				//green = (int)pixels[index].rgbGreen;
				//blue = (int)pixels[index].rgbBlue;

				if (checkColour(pixels[index], targets)) {
					// brightest = red + green + blue;
					targetPos.x = index % width;
					targetPos.y = index / width;
					targetAcquired = true;
				}

				if (i % sampleCount == 0 && targetAcquired) { // if ring is complete and targetAcquired
					xAdjust = (targetPos.x - width / 2);
					yAdjust = (targetPos.y - height / 2);
					mouse_event(MOUSEEVENTF_MOVE, xAdjust, yAdjust, 0, 0); // x and y are deltas, not abs coordinates

					if ((GetKeyState(VK_SHIFT) & 0x100) != 0 && xAdjust < 3 && yAdjust < 3) {
						CreateThread(0, 0, (LPTHREAD_START_ROUTINE)shoot, 0, 0, 0);
					}
					break;
				}
			}
			delete[] pixels;
		}
		Sleep(1);
	}

}


void updateResolution() {
	while (1) {

		Sleep(4000);
		//get hwnd
		hwnd = GetForegroundWindow(); // get handle of currently active window
		//hwnd = FindWindowA(0, "Quake Live");
		//hwnd = FindWindowA("Lovecraft", NULL); //quake champions
		//hwnd = FindWindowA("diabotical", NULL);

		//get window size
		RECT windowsize;
		GetClientRect(hwnd, &windowsize);
		HDC monitor = GetDC(hwnd);
		int current = GetDeviceCaps(monitor, VERTRES);
		int total = GetDeviceCaps(monitor, DESKTOPVERTRES);

		//use hwnd window size
		screenWidth = (windowsize.right - windowsize.left) * total / current;
		screenHeight = (windowsize.bottom - windowsize.top) * total / current;

		//or use desktop size
		//screenWidth = GetSystemMetrics(SM_CXSCREEN);
		//screenHeight = GetSystemMetrics(SM_CYSCREEN);

		a.x = screenWidth / 2 - width / 2;
		a.y = screenHeight / 2 - height / 2;
		b.x = screenWidth / 2 + width / 2;
		b.y = screenHeight / 2 + height / 2;
		Sleep(6000);
	}
}

int main() {
	printf("Ready");
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)updateResolution, 0, 0, 0);
	Aim();
	return 0;
}
