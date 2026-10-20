#version 140
uniform float u_time;
out vec4 pixel;
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main() {
    float hue = mod(u_time * 0.2 , 1.0);
    vec3 rainbowColor = hsv2rgb(vec3(hue, 1.0, 1.0));
    pixel = vec4(rainbowColor, 1.0);
}
