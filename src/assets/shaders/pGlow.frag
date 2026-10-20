uniform sampler2D u_texture;
uniform vec2 u_resolution;
uniform float u_radius;
uniform float u_intensity;
uniform float u_threshold;
uniform int u_N;

float luma(vec3 c) { return dot(c, vec3(0.2126, 0.7152, 0.0722)); }
vec3 bright(vec3 c) {
    float l = luma(c);
    float k = u_threshold * 0.5 + 1e-5;
    float soft = clamp(l - u_threshold + k, 0.0, 2.0 * k);
    soft = soft * soft / (4.0 * k);
    return c * max(soft, l - u_threshold) / max(l, 1e-5);
}
void main() {
    vec2 uv    = gl_TexCoord[0].xy;
    vec2 texel = 1.0 / u_resolution;
    float rnd = fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
    float a   = rnd * 6.2831853;
    mat2  rot = mat2(cos(a), -sin(a), sin(a), cos(a));
    vec3  sum  = vec3(0.0);
    float wsum = 0.0;
    for (int i = 0; i < u_N; ++i) {
        float fi  = float(i);
        float ang = fi * 2.39996323;
        float r   = sqrt((fi + 0.5) / float(u_N));
        vec2  off = rot * vec2(cos(ang), sin(ang)) * r * u_radius * texel;
        float w   = exp(-2.0 * r * r);
        sum  += bright(texture2D(u_texture, uv + off).rgb) * w;
        wsum += w;
    }
    sum /= wsum;
    vec3 base = texture2D(u_texture, uv).rgb;
    gl_FragColor = vec4(base + sum * u_intensity, 1.0);
}
