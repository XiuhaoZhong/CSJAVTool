#include "Render.h"

bool Render::Draw(std::shared_ptr<IRenderInfo> info, std::shared_ptr<CSJOutVideoBuf> buf) {
	RECT rc;
	GetWindowRect(info->GetHwnd(), &rc);
	draw(rc.right - rc.left, rc.bottom - rc.top);
	//if (info->source_preview) {
	//	draw(info->GetSources().at(0), rc.right - rc.left, rc.bottom - rc.top);
	//} else {
	//	if (info->b_draw_outlines) {
	//		//id_ = id;
	//		int w = 600;
	//		int h = 800;
	//		tex_ = updateSceneTexture(w, h);
	//		
	//		//get data,push and to virtual camera;
	//		if (buf) {
	//			/*if (b_mic)
	//				tex = mixWithSceneTexture(mic_, &w, &h, rc.right - rc.left, rc.bottom - rc.top, CommonData::IsMicUseSmallPic());*/
	//			//static GLConverter cvt;
	//			auto wb = buf->get_write_buf();
	//			if (wb) {
	//				//cvt.nv12FromRGB(tex_, w, h, wb->vedio_data.data());
	//				buf->set_read_buf(wb);
	//			}
	//		}
	//	}

	//	draw(rc.right - rc.left, rc.bottom - rc.top);
	//}

	tex_ = updateSceneTexture(800, 600);
	return true;

}