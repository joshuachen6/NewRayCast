uniform float time;
uniform float scale;
uniform sampler2D u_texture;
uniform vec2 resolution;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233)) + time) * 43758.5453);
}

void main(){
  vec2 uv = gl_FragCoord.xy / resolution;
  float noise = random(uv);
  vec4 texColor = texture(u_texture, uv);
  gl_FragColor = vec4(texColor.rgb + vec3(noise * scale), texColor.a);
}
