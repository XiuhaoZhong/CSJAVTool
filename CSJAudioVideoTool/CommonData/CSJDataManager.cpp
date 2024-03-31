#include "CSJDataManager.h"

CSJDataManager::CSJDataManager() {

}

CSJDataManager::~CSJDataManager() {

}

// ---------------------- attributes setters and getters -----------------------------
void CSJDataManager::setMainHwnd(HWND mainHwnd) {
	main_hWnd = mainHwnd;
}

HWND CSJDataManager::getMainHwnd() const {
	return main_hWnd;
}

void CSJDataManager::setRenderHwnd(HWND renderHwnd) {
	render_hWnd = renderHwnd;
}

HWND CSJDataManager::getRenderHwnd() const {
	return render_hWnd;
}

