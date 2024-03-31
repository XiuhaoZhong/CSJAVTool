#include "SceneRenderer.h"

#include <windows.h>

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>

#include "TextureBlitter.h"
#include "GLTool/util/bmem.h"
#include "GLTool/matrix4.h"
#include "GL/glew.h"

using namespace std;

class SceneRenderer::Private {
public:
	static inline void *operator new(size_t size) {
		return bmalloc(size);
	}

	static inline void operator delete(void *ptr) {
		bfree(ptr);
	}

	~Private() {
		if (tex_)
			glDeleteTextures(1, &tex_);
		if (mixed_tex_)
			glDeleteTextures(1, &mixed_tex_);
		if (fbo_)
			glDeleteFramebuffers(1, &fbo_);
		if (program_)
			glDeleteProgram(program_);
		if (program_lines_)
			glDeleteProgram(program_lines_);
	}

	void composite();

	uint32_t tex_ = 0; // scene texture
	uint32_t fbo_ = 0;
	uint32_t w_ = 0; // scene texture width
	uint32_t h_ = 0; // scene texture height
	uint32_t mixed_tex_ = 0;
	bool remote_on_local_ = true;

	TextureBlitter blitter_;

	mutex infos_mtx_;
	atomic_flag infos_updated_ = ATOMIC_FLAG_INIT;

	vector<float> selected_vbo_;
	vector<uint8_t> selected_ibo_;
	vector<float> hover_vbo_;
	vector<uint8_t> hover_ibo_;

	GLuint program_lines_ = 0;
	int line_mat_loc_ = -1;
	int square_mat_loc = -1;

	GLuint program_ = 0;
	GLuint vbo_ = 0;
	GLuint ibo_ = 0;

	int remote_w_ = 0;
	int remote_h_ = 0;
	GLuint remote_tex_ = 0;

	matrix4	projection_matrix_;		// Projection Matrix
};

void SceneRenderer::Private::composite() {
	// bind scene fbo
	if (!fbo_)
		glGenFramebuffers(1, &fbo_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_, 0);
	GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE) {
	}
	float w_ = 800;
	float h_ = 600;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, w_, h_);
	//setProjectionMatrix(0, w_, 0, h_, -100.0f, 100.0f);
	//	transform_pipeline_.SetMatrixStacks(model_view_matrix_, projection_matrix_);

	glClearColor(0, 0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//for (const auto& i : infos_) {
	//	auto& sr = renderers_[i];
	//	// texture blitter. or tex atlas
	//	auto src = sr->source();

	//	if (!src->GetVisible())
	//		continue;
	//	auto tex = sr->texture();
	//	if (!tex)
	//		continue;

	//	float transform[16];
	//	GetTransformMatrix(src, transform);
	//	// auto src_in_scene = src->GetCutRect(); // in scene coordinates

	//	 //blitter_.setTargetRect(to_norm(src_in_scene.GetLeft(), w_), to_norm(src_in_scene.GetTop(), h_), to_norm(src_in_scene.GetRight(), w_), to_norm(src_in_scene.GetBottom(), h_));
	//	 //blitter_.setTargetRect(0, 0, src_in_scene.GetWidth(), src_in_scene.GetHeight());

	//	 //const auto cut_rect = src->GetCutRect();
	//	// const auto rect = src->GetActualRect();

	//	const auto cut_rect = src->GetLastCutRect();
	//	const auto rect = src->GetBaseRect();

	//	blitter_.setTargetRect(0, 0, cut_rect.GetWidth(), cut_rect.GetHeight());

	//	blitter_.setTexRect((cut_rect.GetLeft() - rect.GetLeft()) / rect.GetWidth()
	//						, (cut_rect.GetTop() - rect.GetTop()) / rect.GetHeight()
	//						, (cut_rect.GetRight() - rect.GetLeft()) / rect.GetWidth()
	//						, (cut_rect.GetBottom() - rect.GetTop()) / rect.GetHeight()
	//						, sr->flip());
	//	blitter_.setChannelMap(sr->channelMap());
	//	blitter_.setOpacity(src->GetOpacity());
	//	blitter_.blit(tex, transform);
	//};
	glDisable(GL_BLEND);
}

SceneRenderer::SceneRenderer():d(new Private()) {

}

SceneRenderer::~SceneRenderer() {

}

uint32_t SceneRenderer::updateSceneTexture(int w, int h) {
	//if (d->tmp_.expired()) {
	//	d->tmp_render_.reset(); // must destroy in render context
	//	d->tmp_.reset();
	//}

	// TODO: global fbo for all sources and effects?
	if (!d->tex_) {
		glGenTextures(1, &d->tex_);
		glBindTexture(GL_TEXTURE_2D, d->tex_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// allocate scene texture memory
	if (d->w_ != w || d->h_ != h) {
		glBindTexture(GL_TEXTURE_2D, d->tex_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (d->mixed_tex_) {
			glBindTexture(GL_TEXTURE_2D, d->mixed_tex_);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, 2 * h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		d->w_ = w;
		d->h_ = h;
	}

	d->composite();

	// display callbacks
	return d->tex_;
}

void SceneRenderer::draw(int w, int h, uint32_t fbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, w, h);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const int cm[] = { 0, 1, 2, 3 }; //restore the default channel map for scene texture(RGBA)
	d->blitter_.setChannelMap(cm);
	d->blitter_.setTargetRect(-1, -1, 1, 1);
	d->blitter_.setTexRect(0, 0, 1, 1, true);
	auto scale_x = float(d->w_) / float(w);
	auto scale_y = float(d->h_) / float(h);
	auto s = std::max<float>(scale_x, scale_y);
	float mat[16]{};
	mat[0] = scale_x / s;
	mat[5] = scale_y / s;
	mat[10] = mat[15] = 1;
	d->blitter_.blit(d->tex_, mat);
	
	return;
}

