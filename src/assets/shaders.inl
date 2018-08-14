// Auto generated from tools/makeassets.py

static const unsigned char DATA_shaders_blit_frag[359] __attribute__((aligned(4))) =
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "uniform sampler2D u_tex;\n"
    "uniform float u_stripes;\n"
    "\n"
    "varying vec2 v_tex_pos;\n"
    "varying vec4 v_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(u_tex, v_tex_pos) * v_color;\n"
    "\n"
    "    if (u_stripes != 0.0) {\n"
    "        float k = v_tex_pos.x;\n"
    "        k = step(0.5, fract(k * u_stripes));\n"
    "        gl_FragColor.a *= k;\n"
    "    }\n"
    "}\n"
    "";

ASSET_REGISTER(shaders_blit_frag, "shaders/blit.frag", DATA_shaders_blit_frag, false)

static const unsigned char DATA_shaders_blit_vert[256] __attribute__((aligned(4))) =
    "attribute vec4 a_pos;\n"
    "attribute vec2 a_tex_pos;\n"
    "attribute vec3 a_color;\n"
    "\n"
    "uniform vec4 u_color;\n"
    "\n"
    "varying vec2 v_tex_pos;\n"
    "varying vec4 v_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = a_pos;\n"
    "    v_tex_pos = a_tex_pos;\n"
    "    v_color = vec4(a_color, 1.0) * u_color;\n"
    "}\n"
    "";

ASSET_REGISTER(shaders_blit_vert, "shaders/blit.vert", DATA_shaders_blit_vert, false)

static const unsigned char DATA_shaders_blit_tag_frag[217] __attribute__((aligned(4))) =
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "uniform sampler2D u_tex;\n"
    "\n"
    "varying vec2 v_tex_pos;\n"
    "varying vec4 v_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = v_color;\n"
    "    gl_FragColor.a *= texture2D(u_tex, v_tex_pos).r;\n"
    "}\n"
    "";

ASSET_REGISTER(shaders_blit_tag_frag, "shaders/blit_tag.frag", DATA_shaders_blit_tag_frag, false)

static const unsigned char DATA_shaders_blit_tag_vert[256] __attribute__((aligned(4))) =
    "attribute vec4 a_pos;\n"
    "attribute vec2 a_tex_pos;\n"
    "attribute vec3 a_color;\n"
    "\n"
    "uniform vec4 u_color;\n"
    "\n"
    "varying vec2 v_tex_pos;\n"
    "varying vec4 v_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = a_pos;\n"
    "    v_tex_pos = a_tex_pos;\n"
    "    v_color = vec4(a_color, 1.0) * u_color;\n"
    "}\n"
    "";

ASSET_REGISTER(shaders_blit_tag_vert, "shaders/blit_tag.vert", DATA_shaders_blit_tag_vert, false)

static const unsigned char DATA_shaders_planet_frag[5579] __attribute__((aligned(4))) =
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "#define PI 3.14159265\n"
    "\n"
    "uniform sampler2D u_tex;\n"
    "uniform sampler2D u_normal_tex;\n"
    "uniform vec3 u_light_emit;\n"
    "uniform mat4 u_mv;  // Model view matrix.\n"
    "uniform int u_has_normal_tex;\n"
    "uniform int u_material; // 0: Oren Nayar, 1: generic, 2: ring\n"
    "uniform int u_is_moon; // Set to 1 for the Moon only.\n"
    "uniform sampler2D u_shadow_color_tex; // Used for the Moon.\n"
    "uniform float u_contrast;\n"
    "\n"
    "uniform highp vec4 u_sun; // Sun pos (xyz) and radius (w).\n"
    "// Up to four spheres for illumination ray tracing.\n"
    "uniform int u_shadow_spheres_nb;\n"
    "uniform highp mat4 u_shadow_spheres;\n"
    "\n"
    "varying vec3 v_mpos;   // Pos in model coordinates.\n"
    "varying vec2 v_tex_pos;\n"
    "varying vec4 v_color;\n"
    "varying vec3 v_normal; // Normal in model coordinates.\n"
    "varying vec3 v_tangent;\n"
    "varying vec3 v_bitangent;\n"
    "\n"
    "float oren_nayar_diffuse(\n"
    "        vec3 lightDirection,\n"
    "        vec3 viewDirection,\n"
    "        vec3 surfaceNormal,\n"
    "        float roughness,\n"
    "        float albedo) {\n"
    "\n"
    "    float r2 = roughness * roughness;\n"
    "    float LdotV = dot(lightDirection, viewDirection);\n"
    "    float NdotL = dot(lightDirection, surfaceNormal);\n"
    "    float NdotV = dot(surfaceNormal, viewDirection);\n"
    "    float NaL = acos(NdotL);\n"
    "    float NaV = acos(NdotV);\n"
    "    float alpha = max(NaV, NaL);\n"
    "    float beta = min(NaV, NaL);\n"
    "    float gamma = dot(viewDirection - surfaceNormal * NdotV,\n"
    "                      lightDirection - surfaceNormal * NdotL);\n"
    "    float A = 1.0 - 0.5 * (r2 / (r2 + 0.33));\n"
    "    float B = 0.45 * r2 / (r2 + 0.09);\n"
    "    float C = sin(alpha) * tan(beta);\n"
    "    float scale = 1.6; // Empirical value!\n"
    "    return max(0.0, NdotL) * (A + B * max(0.0, gamma) * C) * scale;\n"
    "}\n"
    "\n"
    "/*\n"
    " * Compute the illumination if we only consider a single sphere in the scene.\n"
    " * Parameters:\n"
    " *   p       - The surface point where we compute the illumination.\n"
    " *   sphere  - A sphere: xyz -> pos, w -> radius.\n"
    " *   sun_pos - Position of the sun.\n"
    " *   sun_r   - Precomputed sun angular radius from the given point.\n"
    " */\n"
    "float illumination_sphere(vec3 p, vec4 sphere, vec3 sun_pos, float sun_r)\n"
    "{\n"
    "    // Sphere angular radius as viewed from the point.\n"
    "    float sph_r = asin(sphere.w / length(sphere.xyz - p));\n"
    "    // Angle <sun, pos, sphere>\n"
    "    float d = acos(min(1.0, dot(normalize(sun_pos - p),\n"
    "                                normalize(sphere.xyz - p))));\n"
    "\n"
    "    // Special case for the moon, to simulate lunar eclipses.\n"
    "    // We assume the only body that can cast shadow on the moon is the Earth.\n"
    "    if (u_is_moon == 1) {\n"
    "        if (d >= sun_r + sph_r) return 1.0; // Outside of shadow.\n"
    "        if (d <= sph_r - sun_r) return d / (sph_r - sun_r) * 0.6; // Umbra.\n"
    "        if (d <= sun_r - sph_r) // Penumbra completely inside.\n"
    "            return 1.0 - sph_r * sph_r / (sun_r * sun_r);\n"
    "        return ((d - abs(sun_r - sph_r)) /\n"
    "                (sun_r + sph_r - abs(sun_r - sph_r))) * 0.4 + 0.6;\n"
    "    }\n"
    "\n"
    "    if (d >= sun_r + sph_r) return 1.0; // Outside of shadow.\n"
    "    if (d <= sph_r - sun_r) return 0.0; // Umbra.\n"
    "    if (d <= sun_r - sph_r) // Penumbra completely inside.\n"
    "        return 1.0 - sph_r * sph_r / (sun_r * sun_r);\n"
    "\n"
    "    // Penumbra partially inside.\n"
    "    // I took this from Stellarium, even though I am not sure how it works.\n"
    "    mediump float x = (sun_r * sun_r + d * d - sph_r * sph_r) / (2.0 * d);\n"
    "    mediump float alpha = acos(x / sun_r);\n"
    "    mediump float beta = acos((d - x) / sph_r);\n"
    "    mediump float AR = sun_r * sun_r * (alpha - 0.5 * sin(2.0 * alpha));\n"
    "    mediump float Ar = sph_r * sph_r * (beta - 0.5 * sin(2.0 * beta));\n"
    "    mediump float AS = sun_r * sun_r * 2.0 * 1.57079633;\n"
    "    return 1.0 - (AR + Ar) / AS;\n"
    "}\n"
    "\n"
    "/*\n"
    " * Compute the illumination at a given point.\n"
    " * Parameters:\n"
    " *   p       - The surface point where we compute the illumination.\n"
    " */\n"
    "float illumination(vec3 p)\n"
    "{\n"
    "    mediump float ret = 1.0;\n"
    "    highp float sun_r = asin(u_sun.w / length(u_sun.xyz - p));\n"
    "    for (int i = 0; i < 4; ++i) {\n"
    "        if (u_shadow_spheres_nb > i) {\n"
    "            highp vec4 sphere = u_shadow_spheres[i];\n"
    "            ret = min(ret, illumination_sphere(p, sphere, u_sun.xyz, sun_r));\n"
    "        }\n"
    "    }\n"
    "    return ret;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 light_dir = normalize(u_sun.xyz - v_mpos);\n"
    "    // Compute N in view space\n"
    "    vec3 n = v_normal;\n"
    "    if (u_has_normal_tex != 0) {\n"
    "        n = texture2D(u_normal_tex, v_tex_pos).rgb - vec3(0.5, 0.5, 0.0);\n"
    "        // XXX: inverse the Y coordinates, don't know why!\n"
    "        n = +n.x * v_tangent - n.y * v_bitangent + n.z * v_normal;\n"
    "    }\n"
    "    n = normalize(n);\n"
    "    gl_FragColor = texture2D(u_tex, v_tex_pos) * v_color;\n"
    "    gl_FragColor.rgb = (gl_FragColor.rgb - 0.5) * u_contrast + 0.5;\n"
    "\n"
    "    if (u_material == 0) { // oren_nayar.\n"
    "        float power = oren_nayar_diffuse(light_dir,\n"
    "                                         normalize(-v_mpos),\n"
    "                                         n,\n"
    "                                         0.9, 0.12);\n"
    "        lowp float illu = illumination(v_mpos);\n"
    "        power *= illu;\n"
    "        gl_FragColor.rgb *= power;\n"
    "\n"
    "        // Earth shadow effect on the moon.\n"
    "        if (u_is_moon == 1 && illu < 0.99) {\n"
    "            vec4 shadow_col = texture2D(u_shadow_color_tex, vec2(illu, 0.5));\n"
    "            gl_FragColor.rgb = mix(\n"
    "                gl_FragColor.rgb, shadow_col.rgb, shadow_col.a);\n"
    "        }\n"
    "\n"
    "    } else if (u_material == 1) { // basic\n"
    "        vec3 light = vec3(0.0, 0.0, 0.0);\n"
    "        light += max(0.0, dot(n, light_dir));\n"
    "        light += u_light_emit;\n"
    "        gl_FragColor.rgb *= light;\n"
    "\n"
    "    } else if (u_material == 2) { // ring\n"
    "        lowp float illu = illumination(v_mpos);\n"
    "        gl_FragColor.rgb *= illu;\n"
    "    }\n"
    "}\n"
    "";

ASSET_REGISTER(shaders_planet_frag, "shaders/planet.frag", DATA_shaders_planet_frag, false)

static const unsigned char DATA_shaders_planet_vert[726] __attribute__((aligned(4))) =
    "attribute vec4 a_pos;\n"
    "attribute vec4 a_mpos;\n"
    "attribute vec2 a_tex_pos;\n"
    "attribute vec3 a_color;\n"
    "attribute vec3 a_normal;\n"
    "attribute vec3 a_tangent;\n"
    "\n"
    "uniform vec4 u_color;\n"
    "uniform vec2 u_depth_range;\n"
    "\n"
    "varying vec3 v_mpos;\n"
    "varying vec2 v_tex_pos;\n"
    "varying vec4 v_color;\n"
    "varying vec3 v_normal;\n"
    "varying vec3 v_tangent;\n"
    "varying vec3 v_bitangent;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = a_pos;\n"
    "    gl_Position.z = (gl_Position.z - u_depth_range[0]) /\n"
    "                    (u_depth_range[1] - u_depth_range[0]);\n"
    "    v_mpos = a_mpos.xyz;\n"
    "    v_tex_pos = a_tex_pos;\n"
    "    v_color = vec4(a_color, 1.0) * u_color;\n"
    "\n"
    "    v_normal = normalize(a_normal);\n"
    "    v_tangent = normalize(a_tangent);\n"
    "    v_bitangent = normalize(cross(v_normal, v_tangent));\n"
    "\n"
    "}\n"
    "";

ASSET_REGISTER(shaders_planet_vert, "shaders/planet.vert", DATA_shaders_planet_vert, false)

static const unsigned char DATA_shaders_points_frag[450] __attribute__((aligned(4))) =
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n"
    "uniform float u_smooth;\n"
    "\n"
    "varying vec2 v_tex_pos;\n"
    "varying vec4 v_color;\n"
    "\n"
    "float dist;\n"
    "float k;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    dist = 2.0 * distance(v_tex_pos, vec2(0.5, 0.5));\n"
    "    k = smoothstep(1.0 - u_smooth, 1.0, dist);\n"
    "    k = sqrt(k);\n"
    "    gl_FragColor.rgb = v_color.rgb;\n"
    "    // Saturation effect at the center.\n"
    "    gl_FragColor.rgb *= 1.0 + smoothstep(0.2, 0.0, k);\n"
    "    gl_FragColor.a = v_color.a * (1.0 - k);\n"
    "}\n"
    "";

ASSET_REGISTER(shaders_points_frag, "shaders/points.frag", DATA_shaders_points_frag, false)

static const unsigned char DATA_shaders_points_vert[318] __attribute__((aligned(4))) =
    "attribute vec4 a_pos;\n"
    "attribute vec2 a_tex_pos;\n"
    "attribute vec4 a_color;\n"
    "attribute vec2 a_shift;\n"
    "\n"
    "varying vec2 v_tex_pos;\n"
    "varying vec4 v_color;\n"
    "\n"
    "uniform vec4 u_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 pos = a_pos;\n"
    "    pos.xy += 2.0 * a_shift;\n"
    "    gl_Position = pos;\n"
    "    v_tex_pos = a_tex_pos;\n"
    "    v_color = a_color * u_color;\n"
    "}\n"
    "";

ASSET_REGISTER(shaders_points_vert, "shaders/points.vert", DATA_shaders_points_vert, false)

