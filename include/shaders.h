const std::string beltrami_klein_lib = "uniform mat3 view2world;\nuniform mat3 world2view;\n\nfloat _d(vec3 a, vec3 b) {\n\treturn a.x * b.x + a.y * b.y - a.z * b.z;\n}\n\n#ifdef frag\n\nvec3 clipToWorld(vec2 uv) {\n\tvec3 p = vec3(uv, 1);\n\tfloat mag = -_d(p, p);\n\tif(mag < 0) {\n\t\tdiscard;\n\t}\n\treturn view2world * (p / sqrt(mag));\n}\n\n#endif\n\nvec2 worldToClip(vec3 p) {\n\tp = world2view * p;\n\treturn p.xy / p.z;\n}\n";
const std::string point_vert = "#version 460\n\n#define vert\n\nlayout(location = 0) in vec4 position;\nlayout(location = 4) in vec2 uv_in;\n\nuniform float ratio;\nuniform float radius;\nuniform vec3 coords;\n\nout vec2 uv;\n\n#include \"camera.lib\"\n\nvoid main() {\n\tvec2 pos = worldToClip(coords);\n\tuv = uv_in;\n/*\n\tif(ratio > 1) {\n\t\tuv.x *= ratio;\n\t} else {\n\t\tuv.y /= ratio;\n\t}\n*/\n\tgl_Position = position;\n\t//gl_Position.z = 0;\n\t//gl_Position = vec4((position.xy - pos) * radius + position.xy, position.zw);\n\t//uv = (uv - pos) * radius + uv;\n}\n";
const std::string line_vert = "#version 460\n\n#define vert\n\nlayout(location = 0) in vec4 position;\nlayout(location = 4) in vec2 uv_in;\n\n//uniform float ratio;\n\nout vec2 uv;\n\nvoid main() {\n\tgl_Position = position;\n\t//gl_Position.z = -0.5;\n\tuv = uv_in;\n/*\n\tif(ratio > 1) {\n\t\tuv.x *= ratio;\n\t} else {\n\t\tuv.y /= ratio;\n\t}\n*/\n}\n";
const std::string line_frag = "#version 460\n\n#define frag\n\nin vec4 gl_FragCoord;\nin vec2 uv;\n\nuniform bool is_selected;\nuniform bool is_hidden;\nuniform vec3 color;\nuniform vec3 x0;\nuniform vec3 v0;\nuniform float width;\n\nout vec4 frag_color;\n\nfloat d(vec3 a, vec3 b) {\n\treturn a.x * b.x + a.y * b.y - a.z * b.z;\n}\n\n#include \"camera.lib\"\n\nfloat getGradientLength(inout float value) {\n\tfloat l = length(vec2(dFdx(value), dFdy(value)));\n\treturn clamp(value / l, 0, 1);\n}\n\nvoid main() {\n\tfrag_color = vec4(color, 1.0);\n\tif(is_hidden)\n\t\tfrag_color.w = 0.8;\n\tif(is_selected)\n\t\tfrag_color.xyz /= 1.2;\n\n\tvec3 p = clipToWorld(uv);\n\n\tvec3 x = x0 + v0 * (d(v0, p) / d(-x0, p));\n\tx /= sqrt(-d(x, x));\n\n\t//float dist = acosh(-d(x, p));\n\tvec2 uv1 = worldToClip(x) - uv;\n\tfloat dist = length(uv1) - width / 2;\n\n\tif(is_selected) {\n\t\tfrag_color = mix(frag_color, vec4(frag_color.xyz, 0.3),  getGradientLength(dist));\n\t\tdist -= width;\n\t\tif(dist < 0)\n\t\t\treturn;\n\t}\n\tfrag_color.w *= 1 - getGradientLength(dist);\n}\n";
const std::string point_frag = "#version 460\n\n#define frag\n\nin vec4 gl_FragCoord;\nin vec2 uv;\n\nuniform bool is_selected;\nuniform bool is_hidden;\nuniform vec3 color;\nuniform float radius;\nuniform vec3 coords;\n\n\nout vec4 frag_color;\n\n#include \"camera.lib\"\n\nfloat getGradientLength(inout float value) {\n\tfloat l = length(vec2(dFdx(value), dFdy(value)));\n\treturn clamp(value / l, 0, 1);\n}\n\nvoid main() {\n\tfloat opacity = 1;\n\tif(is_hidden)\n\t\topacity = 0.8;\n\n\tfrag_color = vec4(color, opacity);\n\tvec2 v = uv - worldToClip(coords);\n\tfloat dist = length(v) - radius * 0.8;\n\t\n\tif(is_selected) {\n\t\tdist -= radius * 1.0;\n\t\tfrag_color.w = 0.3;\n\t\tfrag_color = mix(frag_color, vec4(frag_color.xyz, opacity), getGradientLength(dist));\n\t\tdist -= radius * 0.4;\n\t\tif(dist < 0)\n\t\t\treturn;\n\t\tfrag_color.w *= 1 - getGradientLength(dist);\n\t\treturn;\n\t}\n\n\tfrag_color = mix(frag_color, vec4(vec3(0), opacity), getGradientLength(dist));\n\tdist -= radius * 0.2;\n\tif(dist < 0)\n\t\treturn;\n\tfrag_color.w *= 1 - getGradientLength(dist);\n}\n";
const std::string poincare_disk_lib = "uniform mat3 view2world;\nuniform mat3 world2view;\n\n\nfloat _d(vec3 a, vec3 b) {\n\treturn a.x * b.x + a.y * b.y - a.z * b.z;\n}\n\n#ifdef frag\n\nvec3 clipToWorld(vec2 uv) {\n\tvec3 p = vec3(uv, 1);\n\tfloat mag = -_d(p, p);\n\tif(mag <= 0) {\n\t\tdiscard;\n\t}\n\tmag = 2 / mag;\n\treturn view2world * vec3(p.xy * mag, mag - 1);\n}\n\n#endif\n\nvec2 worldToClip(vec3 p) {\n\tp = world2view * p;\n\treturn p.xy / (p.z + 1);\n}\n";
const std::string poincare_half_plane_lib = "uniform mat3 view2world;\nuniform mat3 world2view;\n\nfloat _d(vec3 a, vec3 b) {\n\treturn a.x * b.x + a.y * b.y - a.z * b.z;\n}\n\nfloat _sq(vec2 a) {\n\treturn a.x * a.x + a.y * a.y;\n}\n\n#ifdef frag\n\nvec3 clipToWorld(vec2 uv) {\n\tif(uv.y <= -1) {\n\t\tdiscard;\n\t}\n\tuv = uv - vec2(0, -2);\n\tuv = 2 * uv / _sq(uv) + vec2(0, -1);\n\tvec3 p = vec3(uv, 1);\n\tfloat mag = -_d(p, p);\n\tif(mag <= 0) {\n\t\tdiscard;\n\t}\n\tmag = 2 / mag;\n\treturn view2world * vec3(p.xy * mag, mag - 1);\n}\n\n#endif\n\nvec2 worldToClip(vec3 p) {\n\tp = world2view * p;\n\tvec2 uv = p.xy / (p.z + 1);\n\tuv = uv - vec2(0, -1);\n\treturn 2 * uv / _sq(uv) + vec2(0, -2);\n}\n";
