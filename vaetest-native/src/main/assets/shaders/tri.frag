#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (location = 0) in vec2 texcoord;
layout (location = 0) out vec4 uFragColor;

layout (binding = 0) uniform vec3      iResolution;           // viewport resolution (in pixels)
layout (binding = 1) uniform float     iTime;                 // shader playback time (in seconds)
layout (binding = 2) uniform float     iTimeDelta;            // render time (in seconds)
layout (binding = 3) uniform float     iFrameRate;            // shader frame rate
layout (binding = 4) uniform int       iFrame;                // shader playback frame
layout (binding = 5) uniform float     iChannelTime[4];       // channel playback time (in seconds)
layout (binding = 6) uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
layout (binding = 7) uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
layout (binding = 8) uniform sampler2D iChannel0;          // input channel. XX = 2D/Cube
layout (binding = 9) uniform vec4      iDate;                 // (year, month, day, time in seconds)

__mainImage__

void main() {
   //uFragColor = texture(tex, texcoord);
   vec2 fragCoord = iResolution.xy - gl_FragCoord.xy;
   mainImage(uFragColor,fragCoord);
}
