#include "BreakOutPostProcess.h"

void BreakOutPostProcess::Render(float time)
{
    // set uniforms/options
    this->PostProcessingShader->Use();
    this->PostProcessingShader->SetFloat(getShaderLocation_time(), time);
    this->PostProcessingShader->SetInteger(getShaderLocation_confuse(), this->Confuse);
    this->PostProcessingShader->SetInteger(getShaderLocation_chaos(), this->Chaos);
    this->PostProcessingShader->SetInteger(getShaderLocation_shake(), this->Shake);
    // render textured quad
    glActiveTexture(GL_TEXTURE0);
    this->Texture.Bind();	
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
