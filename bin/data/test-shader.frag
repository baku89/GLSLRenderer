// Author:
// Title:

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec3 circle(vec2 uv) {

  float inner = step(.5, length(uv - .5));

  return mix(vec3(0.), vec3(1.), inner);
}

void main() {
    vec2 uv = gl_FragCoord.xy/u_resolution.xy;
    uv.x *= u_resolution.x/u_resolution.y;

    vec3 color = circle(uv);

    gl_FragColor = vec4(color,1.0);
}