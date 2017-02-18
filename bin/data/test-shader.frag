// Author:
// Title:

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec3 gradient(float t) {

	vec3 offset = vec3(.0, .1, .2) * 12.;

  return vec3(cos(t * 10. + offset)) / 2. + .5;
}

void main() {
    vec2 uv = gl_FragCoord.xy/u_resolution.xy;
    uv.x *= u_resolution.x/u_resolution.y;

    uv.x += u_time;

    vec3 color = gradient(uv.x) * uv.y;

    gl_FragColor = vec4(color,1.0);
}