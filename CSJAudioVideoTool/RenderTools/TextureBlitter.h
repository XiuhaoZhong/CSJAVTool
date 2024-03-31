#pragma once

#include <cinttypes>
class TextureBlitter
{
public:
  TextureBlitter(const TextureBlitter&) = delete;
  TextureBlitter& operator=(const TextureBlitter&) = delete;
  TextureBlitter() = default;
  ~TextureBlitter();

  void setTexRect(float left, float top, float right, float bottom, bool flip = false) {
      tex_vert_[0] = left;
      tex_vert_[1] = flip ? 1.0 - top : top;
      tex_vert_[2] = right;
      tex_vert_[3] = flip ? 1.0 - bottom : bottom;
      update_vbo_ = true;
  }

  void setTargetRect(float left, float top, float right, float bottom) {
      view_vert_[0] = left;
      view_vert_[1] = top;
      view_vert_[2] = right;
      view_vert_[3] = bottom;
      update_vbo_ = true;
  }

  void setChannelMap(const int rgba[4]) {
      channel_[0] = rgba[0];
      channel_[1] = rgba[1];
      channel_[2] = rgba[2];
      channel_[3] = rgba[3];
  }

  void setOpacity(int v) {
	  src_opacity_ = v;
  }

  bool blit(unsigned texture, const float* targetMat4 = nullptr);
private:
    bool update_vbo_ = true;
    uint32_t program_ = 0;
    uint32_t vbo_ = 0;
    int a_loc_[2] = { -1, -1 }; //attribute location
    int mat_loc_ = -1;
	int opacity_loc_ = -1;
    int sampler_ = -1;
    int channel_loc_ = -1;

    int channel_[4] = { 0, 1, 2, 3 };
    float tex_vert_[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    float view_vert_[4] = { -1.0f, -1.0f, 1.0f, 1.0f };

	int src_opacity_ = 100;
};

