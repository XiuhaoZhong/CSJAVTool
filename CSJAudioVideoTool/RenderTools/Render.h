#pragma once

#include "CSJOutVideoBuf.h"
#include "SceneRenderer.h"

class Render : public IRenderCallback,
			   public SceneRenderer {
public:
	bool Draw(std::shared_ptr<IRenderInfo> info, std::shared_ptr<CSJOutVideoBuf> buf);

private:
	uint32_t tex_ = 0;
};

