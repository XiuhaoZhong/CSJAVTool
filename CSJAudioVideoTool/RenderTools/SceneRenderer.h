#pragma once

#include <memory>

class SceneRenderer {

public:
	SceneRenderer();

	virtual ~SceneRenderer();

	// update texture with source data, and apply filters
	// w, h: scene size
	// return texture with all sources rendered, can be used as encoder input
	uint32_t updateSceneTexture(int w, int h);

	// draw scene texture to current fbo. set viewport before draw by user
	// w, h: viewport size
	void draw(int w, int h, uint32_t fbo = 0);

private:
	class Private;
	std::unique_ptr<Private> d;
};
