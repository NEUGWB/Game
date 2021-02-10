#pragma once
#include "post_processor.h"

class BreakOutPostProcess : public PostProcessor
{
public:
	BreakOutPostProcess(Shader *shader, unsigned int width, unsigned int height) 
		: PostProcessor(shader, width, height), Confuse(false), Chaos(false), Shake(false) {}
	bool Confuse, Chaos, Shake;
	virtual void Render(float time);

	DECLARE_GET_SHADER_UNIFORM_LOCATION(time, PostProcessingShader)
	DECLARE_GET_SHADER_UNIFORM_LOCATION(confuse, PostProcessingShader)
	DECLARE_GET_SHADER_UNIFORM_LOCATION(chaos, PostProcessingShader)
	DECLARE_GET_SHADER_UNIFORM_LOCATION(shake, PostProcessingShader)
};
