
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

#define DEFAULT_ASF (CGRA_SRCDIR + std::string("//res//assets//priman.asf"))
#define DEFAULT_AMC (CGRA_SRCDIR + std::string("//res//assets//TPose.amc"))
#define WALKING_AMC (CGRA_SRCDIR + std::string("//res//assets//walking.amc"))
#define SEATED_AMC (CGRA_SRCDIR + std::string("//res//assets//seated.amc"))
#define CRAWL_AMC (CGRA_SRCDIR + std::string("//res//assets//crawl.amc"))
#define INTER1_ASF (CGRA_SRCDIR + std::string("//res//assets//subject1.asf"))
#define INTER2_ASF (CGRA_SRCDIR + std::string("//res//assets//subject2.asf"))
#define INTER1_AMC (CGRA_SRCDIR + std::string("//res//assets//interaction_s1.amc"))
#define INTER2_AMC (CGRA_SRCDIR + std::string("//res//assets//interaction_s2.amc"))

void basic_model::draw(const glm::mat4 &view, const glm::mat4 proj) {
	mat4 modelview = view * modelTransform;
	
	glUseProgram(shader); // load shader and variables
	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
	glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));

	drawCone();
	//mesh.draw(); // draw
}


Application::Application(GLFWwindow *window) : m_window(window) {
	
	shader_builder sb;
    sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_vert.glsl"));
	sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_frag.glsl"));
	GLuint shader = sb.build();

	m_model.shader = shader;
	m_model.mesh = load_wavefront_data(CGRA_SRCDIR + std::string("/res//assets//teapot.obj")).build();
	m_model.color = vec3(1, 0, 0);
	m_cam_pos = vec2( 0, 0 );

    skeleton_data sd = skeleton_data(DEFAULT_ASF);
    m_anim = skeleton_animation(DEFAULT_AMC, sd);
    m_skeleton.shader = shader;
    m_skeleton.skel = sd;
    m_skeleton.pose = skeleton_pose{m_anim.poses[0]};

    skeleton_data sd2 = skeleton_data(INTER2_ASF);
    m_anim2 = skeleton_animation(INTER2_AMC, sd2);
    m_skeleton2.shader = shader;
    m_skeleton2.skel = sd;
    m_skeleton2.pose = skeleton_pose{m_anim2.poses[m_frame]};


}

chrono::time_point start_time = chrono::high_resolution_clock::now();

void Application::render() {
	// render animation
    auto current_time = chrono::high_resolution_clock::now();
    if (m_isAnim && chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count() >= 16){
        m_frame = (m_frame + 1) % int(m_anim.poses.size());
        m_skeleton.pose = m_anim.poses[m_frame];
        m_skeleton2.pose = m_anim2.poses[m_frame];
        start_time = chrono::high_resolution_clock::now();
    }

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
	mat4 view = translate(mat4(1), vec3( m_cam_pos.x, m_cam_pos.y, -m_distance))
		* rotate(mat4(1), m_pitch, vec3(1, 0, 0))
		* rotate(mat4(1), m_yaw,   vec3(0, 1, 0));


	// helpful draw options
	if (m_show_grid) drawGrid(view, proj);
	if (m_show_axis) drawAxis(view, proj);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);

    if (m_switchPose) {
        switch (m_poseIndex) {
            case 0:
                m_skeleton.skel = skeleton_data(DEFAULT_ASF);
                m_skeleton.pose = skeleton_pose{skeleton_animation(DEFAULT_AMC, m_skeleton.skel).poses[0]};
                m_isAnim = false;
                break;
            case 1:
                m_skeleton.skel = skeleton_data(DEFAULT_ASF);
                m_skeleton.pose = skeleton_pose{skeleton_animation(WALKING_AMC, m_skeleton.skel).poses[0]};
                m_isAnim = false;
                break;
            case 2:
                m_skeleton.skel = skeleton_data(DEFAULT_ASF);
                m_skeleton.pose = skeleton_pose{skeleton_animation(SEATED_AMC, m_skeleton.skel).poses[0]};
                m_isAnim = false;
                break;
            case 3:
                m_skeleton.skel = skeleton_data(DEFAULT_ASF);
                m_skeleton.pose = skeleton_pose{skeleton_animation(CRAWL_AMC, m_skeleton.skel).poses[0]};
                m_isAnim = false;
                break;
            case 4:
                m_frame = 0;
                start_time = chrono::high_resolution_clock::now();
                m_isAnim = true;
                m_skeleton.skel = skeleton_data(INTER1_ASF);
                m_anim =  skeleton_animation(INTER1_AMC, m_skeleton.skel);
                m_skeleton.pose = m_anim.poses[m_frame];
                break;
        }
        m_switchPose = false;
    }


	// draw the model
	//m_model.draw(view, proj);
    m_skeleton.draw(view, proj);
    if (m_isAnim) m_skeleton2.draw(view, proj);
}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(350, 250), ImGuiSetCond_Once);
	ImGui::Begin("Options", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Pitch", &m_pitch, -pi<float>() / 2, pi<float>() / 2, "%.2f");
	ImGui::SliderFloat("Yaw", &m_yaw, -pi<float>(), pi<float>(), "%.2f");
	ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.2f", 2.0f);

	// helpful drawing options
	ImGui::Checkbox("Show axis", &m_show_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &m_show_grid);
	ImGui::Checkbox("Wireframe", &m_showWireframe);
	ImGui::SameLine();
	if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);

    ImGui::Separator();
    static const char * poses[]{"T Pose", "Walking", "Seated", "Crawl", "Animation"};
    if(ImGui::Combo("Pose", &m_poseIndex, poses, 5)) m_switchPose = true;
	
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
	double dy = -( y - y0 );

	double x0 = glm::clamp((m_mousePosition.x - whsize.x) / whsize.x, -1.0f, 1.0f);
	double x = glm::clamp((float(xpos) - whsize.x) / whsize.x, -1.0f, 1.0f);
	double dx = x - x0;

	if (m_leftMouseDown) {
		// clamp the pitch to [-pi/2, pi/2]
		m_pitch += float( acos(y0) - acos(y) );
		m_pitch = float(glm::clamp(m_pitch, -pi<float>() / 2, pi<float>() / 2));

		// wrap the yaw to [-pi, pi]
		m_yaw += float( acos(x0) - acos(x) );
		if (m_yaw > pi<float>()) 
			m_yaw -= float(2 * pi<float>());
		else if (m_yaw < -pi<float>()) 
			m_yaw += float(2 * pi<float>());
	} else if ( m_rightMouseDown ) {
		m_distance += dy * 10;
	} else if ( m_middleMouseDown ) {
		m_cam_pos += vec2( dx, dy ) * 10.f;
	}

	// updated mouse position
	m_mousePosition = vec2(xpos, ypos);
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	(void)mods; // currently un-used

	// capture is left-mouse down
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		m_leftMouseDown = (action == GLFW_PRESS); // only other option is GLFW_RELEASE
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		m_rightMouseDown = (action == GLFW_PRESS);
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		m_middleMouseDown = (action == GLFW_PRESS);
}


void Application::scrollCallback(double xoffset, double yoffset) {
	(void)xoffset; // currently un-used
	m_distance *= pow(1.1f, -yoffset);
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void)key, (void)scancode, (void)action, (void)mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void)c; // currently un-used
}
