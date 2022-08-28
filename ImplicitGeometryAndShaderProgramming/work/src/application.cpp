
// std
#include <iostream>
#include <string>
#include <chrono>

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

// project
#include "application.hpp"
#include "cgra/cgra_geometry.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "cgra/cgra_wavefront.hpp"


using namespace std;
using namespace cgra;
using namespace glm;


void basic_model::draw(const glm::mat4 &view, const glm::mat4 proj) {
    mat4 modelview = view * modelTransform;

    glUseProgram(shader); // load shader and variables
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
    glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));
    glUniform1i(glGetUniformLocation(shader, "uRenderMode"), render_mode);
    glUniform1f(glGetUniformLocation(shader, "uRoughness"), roughness);
    glUniform3fv(glGetUniformLocation(shader, "uLightPos"), 1, value_ptr(lightPos));
    glUniform1f(glGetUniformLocation(shader, "uRefAtNormIncidence"), refAtNormIncidence);
    glUniform1f(glGetUniformLocation(shader, "uDiffRoughness"), diffRoughness);
    glUniform1f(glGetUniformLocation(shader, "uAlbedo"), albedo);
    glUniform1i(glGetUniformLocation(shader, "uShowTex"), showTex);
    glUniform1i(glGetUniformLocation(shader, "uShowNorm"), showNorm);
    glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(shader, "uNormal"), 1);

    //drawCylinder();
    mesh.draw(); // draw
}


mesh_builder Application::sphere_latlong() {
    mesh_builder mb;
    float ele_step = 180.f / float(m_elevation);
    float azi_step = 360.f / float(m_azimuthal);
    float radius = 1;

    for (int i = 0; i <= m_azimuthal; i++) {
        for (int j = 0; j <= m_elevation; j++) {
            mesh_vertex vertex;

            // position
            vec3 position{0};
            position.x = radius * sin(radians(i * float(azi_step))) * sin(radians(j * float(ele_step)));
            position.y = radius * cos(radians(j * float(ele_step)));
            position.z = radius * cos(radians(i * float(azi_step))) * sin(radians(j * float(ele_step)));
            vertex.pos = position;

            // normal
            vec3 normal{0};
/*          normal.x = sin(radians(i * float(azi_step))) * sin(radians(j * float(ele_step)));
            normal.y = cos(radians(j * float(ele_step)));
            normal.z = cos(radians(i * float(azi_step))) * sin(radians(j * float(ele_step)));*/
            normal.x = position.x / radius;
            normal.y = position.y / radius;
            normal.z = position.z / radius;
            vertex.norm = normal;

            //uv
            vec2 uv{0};
            uv.x = float(j) / float(m_elevation);
            uv.y = 1 - float(i) / float(m_azimuthal);
            vertex.uv = uv;

            mb.push_vertex(vertex);

            // triangle going down
            if (i != 0 && j != 0) {
                mb.push_index(get_sphere_index(i, j));
                mb.push_index(get_sphere_index(i, j - 1));
                mb.push_index(get_sphere_index(i - 1, j));
            }

            // triangle going up
            if (i != m_azimuthal && j != m_elevation) {
                mb.push_index(get_sphere_index(i, j));
                mb.push_index(get_sphere_index(i, j + 1));
                mb.push_index(get_sphere_index(i + 1, j));
            }
        }
    }

    return mb;
}

int Application::get_sphere_index(int i, int j) {
    return i * (m_elevation + 1) + j;
}

mesh_builder Application::sphere_from_cube() {
    mesh_builder mb;
    float radius = 1;
    float step = radius * 2.0f / float(m_subdivision);

    for (int i = 0; i < 6; i++) { // 6 sides
        int offset = i * (m_subdivision + 1) * (m_subdivision + 1);
        for (int j = 0; j <= m_subdivision; j++) {
            for (int k = 0; k <= m_subdivision; k++) {
                mesh_vertex v;
                vec3 pos{0};

                switch (i) {
                    case 0: // front
                        v.pos = vec3(-radius + (float) k * step, -radius + (float) j * step, radius);
                        v.norm = vec3(0.f, 0.f, 1.f);
                        break;
                    case 1: // right
                        v.pos = vec3(radius, -radius + (float) j * step, radius - (float) k * step);
                        v.norm = vec3(1.f, 0.f, 0.f);
                        break;
                    case 2: // back
                        v.pos = vec3(radius - (float) k * step, -radius + (float) j * step, -radius);
                        v.norm = vec3(0.f, 0.f, -1.f);
                        break;
                    case 3: // left
                        v.pos = vec3(-radius, -radius + (float) j * step, -radius + (float) k * step);
                        v.norm = vec3(-1.f, 0.f, 0.f);
                        break;
                    case 4: // top
                        v.pos = vec3(-radius + (float) k * step, radius, radius - (float) j * step);
                        v.norm = vec3(0.f, 1.f, 0.f);
                        break;
                    case 5: // bottom
                        v.pos = vec3(-radius + (float) k * step, -radius, -radius + (float) j * step);
                        v.norm = vec3(0.f, -1.f, 0.f);
                        break;
                }
                // v.norm = normalize(v.pos);
                v.uv = vec2(float(k) / float(m_subdivision), float(j) / float(m_subdivision));
                mb.push_vertex(v);

                if (j < m_subdivision && k < m_subdivision) {
                    mb.push_index(get_cube_index(offset, j, k));
                    mb.push_index(get_cube_index(offset, j, k + 1));
                    mb.push_index(get_cube_index(offset, j + 1, k));
                }
                if (j > 0 && k > 0) {
                    mb.push_index(get_cube_index(offset, j, k));
                    mb.push_index(get_cube_index(offset, j, k - 1));
                    mb.push_index(get_cube_index(offset, j - 1, k));
                }
            }
        }
    }

    // transform to sphere
    if (m_subdivision > 1) {
        for (mesh_vertex &v: mb.vertices) {
            vec3 p = v.pos;
            vec3 new_pos{0};

            new_pos.x = p.x * (sqrt(1.f - (p.y * p.y / 2.f) - (p.z * p.z / 2.f) + (p.y * p.y * p.z * p.z / 3.f)));
            new_pos.y = p.y * (sqrt(1.f - (p.z * p.z / 2.f) - (p.x * p.x / 2.f) + (p.z * p.z * p.x * p.x / 3.f)));
            new_pos.z = p.z * (sqrt(1.f - (p.x * p.x / 2.f) - (p.y * p.y / 2.f) + (p.x * p.x * p.y * p.y / 3.f)));

            vec3 new_norm = normalize(p - new_pos);

            v.pos = new_pos;
            v.norm = normalize(v.pos);
        }
    }
    return mb;
}

int Application::get_cube_index(int offset, int j, int k) const {
    return offset + j * (m_subdivision + 1) + k;
}

// reference - https://www.cs.ucdavis.edu/~amenta/s06/findnorm.pdf
mesh_builder Application::torus_latlong() {
    mesh_builder mb;
    float u_step = radians(360.f / float(m_torus_u));
    float v_step = radians(360.f / float(m_torus_v));

    for (int i = 0; i <= m_torus_v; i++) {
        for (int j = 0; j <= m_torus_u; j++) {
            mesh_vertex v;

            v.pos.x = (float(m_torus_c) + float(m_torus_a) * cos(float(j) * u_step)) * sin(float(i) * v_step);
            v.pos.y = float(m_torus_a) * sin(float(j) * u_step);
            v.pos.z = (float(m_torus_c) + float(m_torus_a) * cos(float(j) * u_step)) * cos(float(i) * v_step);

            float tx = cos(float(i) * v_step);
            float ty = 0;
            float tz = -sin(float(i) * v_step);

            float sx = sin(float(i) * v_step) * (-sin(float(j) * u_step));
            float sy = cos(float(j) * u_step);
            float sz = cos(float(i) * v_step) * (-sin(float(j) * u_step));

            v.norm.x = ty * sz - tz * sy;
            v.norm.y = tz * sx - tx * sz;
            v.norm.z = tx * sy - ty * sx;
            v.norm = normalize(v.norm);

            v.uv = vec2(float(j) / float(m_torus_u), float(i) / float(m_torus_v));

            mb.push_vertex(v);

            // triangle going down
            if (i != 0 && j != 0) {
                mb.push_index(get_torus_index(i, j));
                mb.push_index(get_torus_index(i, j - 1));
                mb.push_index(get_torus_index(i - 1, j));
            }

            // triangle going up
            if (i != m_torus_v && j != m_torus_u) {
                mb.push_index(get_torus_index(i, j));
                mb.push_index(get_torus_index(i, j + 1));
                mb.push_index(get_torus_index(i + 1, j));
            }
        }
    }
    return mb;
}

int Application::get_torus_index(int i, int j) const {
    return i * (m_torus_u + 1) + j;
}


Application::Application(GLFWwindow *window) : m_window(window) {
    question();
    shader_builder sb;
    // load and upload the texture
    cgra::rgba_image texture_data(CGRA_SRCDIR + std::string("//res//textures//Texture.png"));
    cgra::rgba_image normal_data(CGRA_SRCDIR + std::string("//res//textures//NormalMap.png"));

    GLuint texture = 0;
    GLuint normal = 0;

    m_model.texture = texture_data.uploadTexture(GL_RGBA8, texture, 0);
    normal_data.uploadTexture(GL_RGBA8, normal, 1);

    sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_vert.glsl"));
    sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_frag.glsl"));

    GLuint shader = sb.build();

    m_model.shader = shader;
    m_model.mesh = load_wavefront_data(CGRA_SRCDIR + std::string("/res//assets//teapot.obj")).build();
    m_model.color = vec3(1, 0, 0);
    m_cam_pos = vec2(0, 0);
}


void Application::render() {
    // retrieve the window height
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    m_windowsize = vec2(width, height); // update window size
    glViewport(0, 0, width, height); // set the viewport to draw to the entire window

    // clear the back-buffer
    glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable flags for normal/forward rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // projection matrix
    mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);

    // view matrix
    mat4 view = translate(mat4(1), vec3(m_cam_pos.x, m_cam_pos.y, -m_distance))
                * rotate(mat4(1), m_pitch, vec3(1, 0, 0))
                * rotate(mat4(1), m_yaw, vec3(0, 1, 0));


    // helpful draw options
    if (m_show_grid) drawGrid(view, proj);
    if (m_show_axis) drawAxis(view, proj);
    glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);


    // draw the model
    if (m_update) {
        m_model.mesh.destroy();
        if (m_currentModel == 0) m_model.mesh = sphere_latlong().build();
        else if (m_currentModel == 1) m_model.mesh = sphere_from_cube().build();
        else if (m_currentModel == 2) m_model.mesh = torus_latlong().build();
        m_update = false;
    }

    m_model.draw(view, proj);
}


void Application::renderGUI() {

    // setup window
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_Once);
    ImGui::Begin("Options", 0);

    // display current camera parameters
    ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::SliderFloat("Pitch", &m_pitch, -pi<float>() / 2, pi<float>() / 2, "%.2f");
    ImGui::SliderFloat("Yaw", &m_yaw, -pi<float>(), pi<float>(), "%.2f");
    ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.2f", 2.0f);

    // display sphere elevation and azimuthal parameters
    ImGui::Separator();
    ImGui::Text("Sphere");
    if (ImGui::SliderInt("Elevation", &m_elevation, 1, 45)) m_update = true;
    if (ImGui::SliderInt("Azimuthal", &m_azimuthal, 1, 45)) m_update = true;

    ImGui::Separator();
    ImGui::Text("Sphere from Cube");
    if (ImGui::SliderInt("Subdivision", &m_subdivision, 1, 10)) m_update = true;

    ImGui::Separator();
    ImGui::Text("Torus");
    if (ImGui::SliderInt("Torus_Elevation", &m_torus_v, 1, 30)) m_update = true;
    if (ImGui::SliderInt("Torus_Azimuthal", &m_torus_u, 1, 30)) m_update = true;
    if (ImGui::SliderFloat("Ring Size", &m_torus_a, 1, 5, "%.2f")) m_update = true;
    if (ImGui::SliderFloat("Radius", &m_torus_c, 2, 10, "%.2f")) m_update = true;

    if (ImGui::Button("LatLong")) {
        m_currentModel = 0;
        m_update = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cube2Sphere")) {
        m_currentModel = 1;
        m_update = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Torus")) {
        m_currentModel = 2;
        m_update = true;
    }

    // render
    ImGui::Separator();
    ImGui::Text("Render Mode");
    ImGui::Checkbox("Show Texture", &m_model.showTex);
    ImGui::Checkbox("Show Normal Map", &m_model.showNorm);
    static const char * modes[]{ "Default", "Cook-Torrance", "Oven-Nayar"};
    ImGui::Combo("Render Mode", &m_model.render_mode, modes, 3);
    ImGui::SliderFloat3("Light Pos", &m_model.lightPos[0], -20.f, 20.f, "%.2f");
    ImGui::Separator();
    ImGui::Text("Cook-Torrance");
    ImGui::SliderFloat("Roughness", &m_model.roughness, 0.01, 1, "%.2f");
    ImGui::SliderFloat("RefAtNormIncidence", &m_model.refAtNormIncidence, 0, 1, "%.2f");
    ImGui::Separator();
    ImGui::Text("Oren-Nayar");
    ImGui::SliderFloat("Diffuse Roughness", &m_model.diffRoughness, 0, 1, "%.2f");
    ImGui::SliderFloat("Albedo", &m_model.albedo, 0, 5, "%.2f");


    // helpful drawing options
    ImGui::Checkbox("Show axis", &m_show_axis);
    ImGui::SameLine();
    ImGui::Checkbox("Show grid", &m_show_grid);
    ImGui::Checkbox("Wireframe", &m_showWireframe);
    ImGui::SameLine();
    if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);


    ImGui::Separator();

    // example of how to use input boxes
    static float exampleInput;
    if (ImGui::InputFloat("example input", &exampleInput)) {
        cout << "example input changed to " << exampleInput << endl;
    }

    // finish creating window
    ImGui::End();
}


void Application::cursorPosCallback(double xpos, double ypos) {
    vec2 whsize = m_windowsize / 2.0f;

    double y0 = glm::clamp((m_mousePosition.y - whsize.y) / whsize.y, -1.0f, 1.0f);
    double y = glm::clamp((float(ypos) - whsize.y) / whsize.y, -1.0f, 1.0f);
    double dy = -(y - y0);

    double x0 = glm::clamp((m_mousePosition.x - whsize.x) / whsize.x, -1.0f, 1.0f);
    double x = glm::clamp((float(xpos) - whsize.x) / whsize.x, -1.0f, 1.0f);
    double dx = x - x0;

    if (m_leftMouseDown) {
        // clamp the pitch to [-pi/2, pi/2]
        m_pitch += float(acos(y0) - acos(y));
        m_pitch = float(glm::clamp(m_pitch, -pi<float>() / 2, pi<float>() / 2));

        // wrap the yaw to [-pi, pi]
        m_yaw += float(acos(x0) - acos(x));
        if (m_yaw > pi<float>())
            m_yaw -= float(2 * pi<float>());
        else if (m_yaw < -pi<float>())
            m_yaw += float(2 * pi<float>());
    } else if (m_rightMouseDown) {
        m_distance += dy * 10;
    } else if (m_middleMouseDown) {
        m_cam_pos += vec2(dx, dy) * 10.f;
    }

    // updated mouse position
    m_mousePosition = vec2(xpos, ypos);
}


void Application::mouseButtonCallback(int button, int action, int mods) {
    (void) mods; // currently un-used

    // capture is left-mouse down
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        m_leftMouseDown = (action == GLFW_PRESS); // only other option is GLFW_RELEASE
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        m_rightMouseDown = (action == GLFW_PRESS);
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        m_middleMouseDown = (action == GLFW_PRESS);
}


void Application::scrollCallback(double xoffset, double yoffset) {
    (void) xoffset; // currently un-used
    m_distance *= pow(1.1f, -yoffset);
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
    (void) key, (void) scancode, (void) action, (void) mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
    (void) c; // currently un-used
}


void Application::question(){
    vec3 C(0, 0, 0);
    vec3 O (5, 5, 4);
    vec3 D = normalize(vec3(0, 0, 0.5) - O);
    float refraction = 1.5;
    float R = 1;

    float a = dot(D, D);
    float b = 2.0f * dot((O - C), D);
    float c = dot((O - C), (O - C)) - pow(R, 2.0f);

    float discriminant = b * b - 4.0f * a * c;
    float t = (-b - sqrt(discriminant)) / (2.0f * a);
    vec3 p = O + t * D;
    cout << endl << "Question answer(normalized D):" << endl;
    cout << "intersection point: " << p.x << " " << p.y << " " << p.z << " " << endl;

    vec3 reflect_dir = reflect(D, normalize(p));
    vec3 refract_dir = refract(D, normalize(p), refraction);

    cout << "reflect vector: " << reflect_dir.x << " " << reflect_dir.y << " " << reflect_dir.z << " " << endl;
    cout << "refract vector: " << refract_dir.x << " " << refract_dir.y << " " << refract_dir.z << " " << endl;
}


// not working
void basic_model::light_space_transform() {
    GLuint depthMapFramebuff = 0;
    glGenFramebuffers(1, &depthMapFramebuff);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuff);
    // depth texture
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    glDrawBuffer(GL_NONE); // no color buffer is drawn to

    shader_builder sb;
    sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//shadow_vert.glsl"));
    sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//shadow_frag.glsl"));
    shader = sb.build();

    vec3 lightInvDir (0.5, 2, 2);
    float near_plane = -10, far_plane = 20;
    mat4 depthProjectionMatrix = ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

    mat4 depthViewMatrix = lookAt(lightInvDir, vec3(0), vec3(0, 1, 0));
    mat4 depthModelMatrix = mat4(1.0);
    mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shader, "uLightSpaceMatrix"), 1, GL_FALSE, &depthMVP[0][0]);
    glViewport(0,0,1024,1024);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuff);
    glClear(GL_DEPTH_BUFFER_BIT);
    //RenderScene(shader);
    mesh.draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}
