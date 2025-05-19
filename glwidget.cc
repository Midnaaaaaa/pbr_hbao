// Author: Imanol Munoz-Pandiella 2023 based on Marc Comino 2020

#include <glwidget.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#include "./mesh_io.h"
#include "./triangle_mesh.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {

const double kFieldOfView = 60;
const double kZNear = 0.0001;
const double kZFar = 20;

const std::vector<std::vector<std::string>> kShaderFiles = {
    {"../shaders/phong.vert",        "../shaders/phong.frag"},
    {"../shaders/texMap.vert",       "../shaders/texMap.frag"},
    {"../shaders/reflection.vert",   "../shaders/reflection.frag"},
    {"../shaders/pbs.vert",          "../shaders/pbs.frag"},
    {"../shaders/ibl-pbs.vert",      "../shaders/ibl-pbs.frag"},
    {"../shaders/conv-shader.vert",  "../shaders/conv-shader.frag"},
    {"../shaders/prefilter.vert",    "../shaders/prefilter.frag"},
    {"../shaders/brdf-lut.vert",     "../shaders/brdf-lut.frag"},
    {"../shaders/g-pass.vert",       "../shaders/g-pass.frag"},
    {"../shaders/ssao-pass.vert", "../shaders/ssao-pass.frag"},
    {"../shaders/shading-pass.vert", "../shaders/shading-pass.frag"},
    {"../shaders/sky.vert",          "../shaders/sky.frag"}};//sky needs to be the last one

const int kVertexAttributeIdx = 0;
const int kNormalAttributeIdx = 1;
const int kTexCoordAttributeIdx = 2;


bool ReadFile(const std::string filename, std::string *shader_source) {
    std::ifstream infile(filename.c_str());

    if (!infile.is_open() || !infile.good()) {
        std::cerr << "Error " + filename + " not found." << std::endl;
        return false;
    }

    std::stringstream stream;
    stream << infile.rdbuf();
    infile.close();

    *shader_source = stream.str();
    return true;
}

bool LoadImage(const std::string &path, GLuint cube_map_pos) {
    QImage image;
    bool res = image.load(path.c_str());
    if (res) {
        QImage gl_image = image.mirrored();
        glTexImage2D(cube_map_pos, 0, GL_RGBA, image.width(), image.height(), 0,
                     GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
    }
    return res;
}

bool LoadCubeMap(const QString &dir) {
    std::string path = dir.toUtf8().constData();
    bool res = LoadImage(path + "/right.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    res = res && LoadImage(path + "/left.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    res = res && LoadImage(path + "/top.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    res = res && LoadImage(path + "/bottom.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    res = res && LoadImage(path + "/back.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    res = res && LoadImage(path + "/front.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    if (res) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    return res;
}

bool LoadProgram(const std::string &vertex, const std::string &fragment,
                 QOpenGLShaderProgram *program) {
    std::string vertex_shader, fragment_shader;
    bool res =
        ReadFile(vertex, &vertex_shader) && ReadFile(fragment, &fragment_shader);

    if (res) {
        program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                         vertex_shader.c_str());
        program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                         fragment_shader.c_str());
        program->bindAttributeLocation("vertex", kVertexAttributeIdx);
        program->bindAttributeLocation("normal", kNormalAttributeIdx);
        program->bindAttributeLocation("texCoord", kTexCoordAttributeIdx);
        program->link();
    }

    return res;
}

}  // namespace

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
    initialized_(false),
    width_(0.0),
    height_(0.0),
    currentShader_(0),
    fresnel_(0.2, 0.2, 0.2),
    currentTexture_(0),
    skyVisible_(true),
    metalness_(0),
    roughness_(0),
    two_step_rendering(false),
    currentBuffer_(0)
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget() {
    if (initialized_) {
        glDeleteTextures(1, &specular_map_);
        glDeleteTextures(1, &diffuse_map_);
    }
}

bool GLWidget::LoadModel(const QString &filename) {
    std::string file = filename.toUtf8().constData();
    size_t pos = file.find_last_of(".");
    std::string type = file.substr(pos + 1);

    std::unique_ptr<data_representation::TriangleMesh> mesh =
        std::make_unique<data_representation::TriangleMesh>();

    bool res = false;
    if (type.compare("ply") == 0) {
        res = data_representation::ReadFromPly(file, mesh.get());
    } else if (type.compare("obj") == 0) {
        res = data_representation::ReadFromObj(file, mesh.get());
    } else if(type.compare("null") == 0) {
        res = data_representation::CreateSphere(mesh.get());
    }

    if (res) {
        mesh_.reset(mesh.release());
        camera_.UpdateModel(mesh_->min_, mesh_->max_);
        //mesh_->computeNormals();

        // TODO(students): Create / Initialize buffers.
        //MESH: You need to create 1 VAO and 4 VBO
        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO_v);
        glGenBuffers(1, &VBO_n);
        glGenBuffers(1, &VBO_tc);
        glGenBuffers(1, &VBO_i);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_v);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_->vertices_.size(), &mesh_->vertices_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_n);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_->normals_.size(), &mesh_->normals_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_tc);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_->texCoords_.size(), &mesh_->texCoords_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_i);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mesh_->faces_.size(), &mesh_->faces_[0], GL_STATIC_DRAW);

        glBindVertexArray(0);




        //SKY BOX: You need to create 1 VAO and 2 VBO:
        glGenVertexArrays(1, &VAO_sky);

        glGenBuffers(1, &VBO_v_sky);
        glGenBuffers(1, &VBO_i_sky);

        skyVertices_ = {
            -1.f, -1.f, -1.f,
            +1.f, -1.f, -1.f,
            -1.f, -1.f, +1.f,
            +1.f, -1.f, +1.f,
            -1.f, +1.f, -1.f,
            +1.f, +1.f, -1.f,
            -1.f, +1.f, +1.f,
            +1.f, +1.f, +1.f,
        };

        skyFaces_ = {
            0, 1, 2,
            1, 3, 2,

            2, 7, 6,
            2, 3, 7,

            0, 5, 4,
            0, 1, 5,

            6, 4, 5,
            6, 5, 7,

            3, 5, 7,
            3, 1, 5,

            0, 6, 4,
            0, 2, 6
        };

        glBindVertexArray(VAO_sky);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_v_sky);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skyVertices_.size(), &skyVertices_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_i_sky);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * skyFaces_.size(), &skyFaces_[0], GL_STATIC_DRAW);
        glBindVertexArray(0);

        // TODO END.

        emit SetFaces(QString(std::to_string(mesh_->faces_.size() / 3).c_str()));
        emit SetVertices(
            QString(std::to_string(mesh_->vertices_.size() / 3).c_str()));
        return true;
    }

    return false;
}

void GLWidget::DrawQuad(){
    static GLuint quadVAO = 0;
    static GLuint quadVBO = 0;

    // x,y coords and uvs
    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Texture coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }

    // Draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void GLWidget::DrawFloor(){
    GLuint floorVAO = 0;
    GLuint floorVBO_v = 0;
    GLuint floorVBO_n = 0;
    GLuint floorVBO_i = 0;


    if (floorVAO == 0) {
        float vertexs[] = {
            -4.0f, -1.0f, -4.0f,
            4.0f, -1.0f, -4.0f,
            4.0f, -1.0f,  4.0f,
            -4.0f, -1.0f,  4.0f
        };

        float normals[] = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };

        // Indices for two triangles
        unsigned int indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        glGenVertexArrays(1, &floorVAO);
        glGenBuffers(1, &floorVBO_v);
        glGenBuffers(1, &floorVBO_n);
        glGenBuffers(1, &floorVBO_i);

        glBindVertexArray(floorVAO);

        glBindBuffer(GL_ARRAY_BUFFER, floorVBO_v);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, floorVBO_n);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorVBO_i);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



void GLWidget::DrawCube(){
    static GLuint cubeVAO = 0;
    static GLuint cubeVBO = 0;

    if (cubeVAO == 0) {
        float vertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


void GLWidget::GenerateIrradianceAndPrefilteredMaps(const int cubemap_width, const int cubemap_height, const int mip_levels){
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    GLuint captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemap_width, cubemap_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_map_);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB,
                     cubemap_width, cubemap_height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

    int diffuse_irradiance_shader = 5;
    programs_[diffuse_irradiance_shader]->bind();

    //--------------------------DRAW CALL FOR RENDERING IRRADIANCE DIFFUSE MAP TO FRAMEBUFFER ------------------------------------

    GLuint environment_map_location, capture_projection_location, capture_view_location;

    environment_map_location = programs_[diffuse_irradiance_shader]->uniformLocation("environmentMap");
    capture_projection_location = programs_[diffuse_irradiance_shader]->uniformLocation("projection");
    capture_view_location = programs_[diffuse_irradiance_shader]->uniformLocation("view");
    glUniform1i(environment_map_location, 0);
    glUniformMatrix4fv(capture_projection_location, 1, GL_FALSE, &captureProjection[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);

    glViewport(0, 0, cubemap_width, cubemap_height);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(capture_view_location, 1, GL_FALSE, &captureViews[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuse_map_, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //--------------------------DRAW CALL FOR PREFILTERED MAPS ------------------------------------
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefiltered_map_);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, cubemap_width, cubemap_height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemap_width, cubemap_height);

    int prefilter_shader = 6;
    programs_[prefilter_shader]->bind();

    GLuint roughness_location;
    roughness_location = programs_[prefilter_shader]->uniformLocation("roughness");
    environment_map_location = programs_[prefilter_shader]->uniformLocation("environmentMap");
    capture_projection_location = programs_[prefilter_shader]->uniformLocation("projection");
    capture_view_location = programs_[prefilter_shader]->uniformLocation("view");
    glUniform1i(environment_map_location, 0);
    glUniformMatrix4fv(capture_projection_location, 1, GL_FALSE, &captureProjection[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (int i = 0; i < mip_levels; i++)
    {
        int mipWidth = cubemap_width >> i;
        int mipHeight = cubemap_height >> i;
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = i / (float)mip_levels;
        glUniform1f(roughness_location, roughness);
        for (unsigned int j = 0; j < 6; ++j)
        {
            glUniformMatrix4fv(capture_view_location, 1, GL_FALSE, &captureViews[j][0][0]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, prefiltered_map_, i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            DrawCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //--------------------------DRAW CALL FOR BRDF LUT ------------------------------------
    glBindTexture(GL_TEXTURE_2D, brdf_lut_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 1024, 1024, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_lut_, 0);

    glViewport(0, 0, 1024, 1024);

    int brdf_lut_shader = 7;
    programs_[brdf_lut_shader]->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_lut_, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    std::vector<float> data(1024 * 1024 * 2); // 2 componentes: R y G
    glReadPixels(0, 0, 1024, 1024, GL_RG, GL_FLOAT, data.data());

    QImage img(1024, 1024, QImage::Format_RGB32);
    for (int y = 0; y < 1024; ++y) {
        for (int x = 0; x < 1024; ++x) {
            int idx = ((1024 - 1 - y) * 1024 + x) * 2;
            float r = data[idx];
            float g = data[idx + 1];
            QColor color(
                int(std::clamp(r, 0.0f, 1.0f) * 255.0f),
                int(std::clamp(g, 0.0f, 1.0f) * 255.0f),
                0 // B = 0
                );
            img.setPixelColor(x, y, color);
        }
    }

    img.save("../output_irradiance/brdf-lut.png");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);




    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (int mip = 0; mip < mip_levels; ++mip) {
        int mip_width = std::max(1, cubemap_width >> mip);
        int mip_height = std::max(1, cubemap_height >> mip);

        std::vector<float> data(mip_width * mip_height * 3);

        for (int face = 0; face < 6; ++face) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, prefiltered_map_, mip);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, mip_width, mip_height, GL_RGB, GL_FLOAT, data.data());

            QImage img(mip_width, mip_height, QImage::Format_RGB32);
            for (int y = 0; y < mip_height; ++y) {
                for (int x = 0; x < mip_width; ++x) {
                    int idx = ((mip_height - 1 - y) * mip_width + x) * 3;
                    QColor color(
                        int(std::min(1.0f, data[idx]) * 255.0f),
                        int(std::min(1.0f, data[idx + 1]) * 255.0f),
                        int(std::min(1.0f, data[idx + 2]) * 255.0f)
                        );
                    img.setPixelColor(x, y, color);
                }
            }

            img.save(QString("../output_irradiance/face_%1_mip_%2.png").arg(face).arg(mip));
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (int i = 0; i < 6; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuse_map_, 0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        std::vector<float> data(cubemap_width * cubemap_height * 3);
        glReadPixels(0, 0, cubemap_width, cubemap_height, GL_RGB, GL_FLOAT, data.data());

        QImage img(cubemap_width, cubemap_height, QImage::Format_RGB32);
        for (int y = 0; y < cubemap_height; ++y) {
            for (int x = 0; x < cubemap_width; ++x) {
                int idx = ((cubemap_height - 1 - y) * cubemap_width + x) * 3;
                QColor color(
                    int(std::min(1.0f, data[idx]) * 255.0f),
                    int(std::min(1.0f, data[idx + 1]) * 255.0f),
                    int(std::min(1.0f, data[idx + 2]) * 255.0f)
                    );
                img.setPixelColor(x, y, color);
            }
        }

        img.save("../output_irradiance/" + QString::number(i) + ".png");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

bool GLWidget::LoadSpecularMap(const QString &dir) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);
    bool res = LoadCubeMap(dir);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    if(res){
        int cubemap_width, cubemap_height;
        glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_WIDTH, &cubemap_width);
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_HEIGHT, &cubemap_height);

        num_mips = (int)glm::log2((float)cubemap_width);
        GenerateIrradianceAndPrefilteredMaps(cubemap_width, cubemap_height, num_mips);
    }

    update();
    return res;
}

bool GLWidget::LoadDiffuseMap(const QString &dir) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_map_);
    bool res = LoadCubeMap(dir);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    update();
    return res;
}

bool GLWidget::LoadColorMap(const QString &filename)
{
    //TODO Students
    //Configure the texture with identifier color_map_. Take advantage of LoadImage("path", GL_TEXTURE_2D).
    //Remember to configure the texture parameters.
    bool res;
    glBindTexture(GL_TEXTURE_2D, color_map_);
    res = LoadImage(filename.toStdString(), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    //TODO END
    update();
    using_color_map = res;
    return res;

}

bool GLWidget::LoadRoughnessMap(const QString &filename)
{
    //TODO Students
    //Configure the texture with identifier roughness_map_. Take advantage of LoadImage("path", GL_TEXTURE_2D)
    //Remember to configure the texture parameters.
    bool res;
    glBindTexture(GL_TEXTURE_2D, roughness_map_);
    res = LoadImage(filename.toStdString(), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    //TODO END
    update();
    using_roughness_map = res;
    return res;
}

bool GLWidget::LoadMetalnessMap(const QString &filename)
{
    //TODO Students
    //Configure the texture with identifier metalness_map_. Take advantage of LoadImage("path", GL_TEXTURE_2D)
    //Remember to configure the texture parameters.
    bool res;
    glBindTexture(GL_TEXTURE_2D, metalness_map_);
    res = LoadImage(filename.toStdString(), GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    //TODO END
    update();
    using_metalness_map = res;
    return res;
}

void GLWidget::initializeGBufferTextures() {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &gAlbedoTex);
    glBindTexture(GL_TEXTURE_2D, gAlbedoTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width_, this->height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedoTex, 0);

    glGenTextures(1, &gNormalTex);
    glBindTexture(GL_TEXTURE_2D, gNormalTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width_, this->height_, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalTex, 0);

    glGenTextures(1, &gDepthTex);
    glBindTexture(GL_TEXTURE_2D, gDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->width_, this->height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepthTex, 0);

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLWidget::initializeSSAOTex(){
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    glGenTextures(1, &ssaoTex);
    glBindTexture(GL_TEXTURE_2D, ssaoTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTex, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GLWidget::initializeGL ()
{
    // Cal inicialitzar l'ús de les funcions d'OpenGL
    initializeOpenGLFunctions();

    //initializing opengl state
    glEnable(GL_NORMALIZE);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    //generating needed textures
    glGenTextures(1, &specular_map_);
    glGenTextures(1, &diffuse_map_);
    glGenTextures(1, &color_map_);
    glGenTextures(1, &roughness_map_);
    glGenTextures(1, &metalness_map_);
    glGenTextures(1, &prefiltered_map_);
    glGenTextures(1, &brdf_lut_);


    //create shader programs
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//phong
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//texture mapping
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//reflection
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//simple pbs
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//ibl pbs
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//conv-shader
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//prefilter-shader
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//brdf-lut-shader
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//g-pass
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//ssao-pass
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//shading-pass
    programs_.push_back(std::make_unique<QOpenGLShaderProgram>());//sky

    //load vertex and fragment shader files
    bool res =   LoadProgram(kShaderFiles[0][0],   kShaderFiles[0][1],    programs_[0].get());
    res = res && LoadProgram(kShaderFiles[1][0],   kShaderFiles[1][1],    programs_[1].get());
    res = res && LoadProgram(kShaderFiles[2][0],   kShaderFiles[2][1],    programs_[2].get());
    res = res && LoadProgram(kShaderFiles[3][0],   kShaderFiles[3][1],    programs_[3].get());
    res = res && LoadProgram(kShaderFiles[4][0],   kShaderFiles[4][1],    programs_[4].get());
    res = res && LoadProgram(kShaderFiles[5][0],   kShaderFiles[5][1],    programs_[5].get());
    res = res && LoadProgram(kShaderFiles[6][0],   kShaderFiles[6][1],    programs_[6].get());
    res = res && LoadProgram(kShaderFiles[7][0],   kShaderFiles[7][1],    programs_[7].get());
    res = res && LoadProgram(kShaderFiles[8][0],   kShaderFiles[8][1],    programs_[8].get());
    res = res && LoadProgram(kShaderFiles[9][0],   kShaderFiles[9][1],    programs_[9].get());
    res = res && LoadProgram(kShaderFiles[10][0],   kShaderFiles[10][1],    programs_[10].get());
    res = res && LoadProgram(kShaderFiles[11][0],   kShaderFiles[11][1],    programs_[11].get());




    if (!res) exit(0);

    LoadModel(".null");//create an sphere

    initialized_ = true;
}

void GLWidget::resizeGL (int w, int h)
{
    if (h == 0) h = 1;
    width_ = w;
    height_ = h;

    camera_.SetViewport(0, 0, w, h);
    camera_.SetProjection(kFieldOfView, kZNear, kZFar);
    initializeGBufferTextures();
    initializeSSAOTex();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        camera_.StartRotating(event->x(), event->y());
    }
    if (event->button() == Qt::RightButton) {
        camera_.StartZooming(event->x(), event->y());
    }
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    camera_.SetRotationX(event->y());
    camera_.SetRotationY(event->x());
    camera_.SafeZoom(event->y());
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        camera_.StopRotating(event->x(), event->y());
    }
    if (event->button() == Qt::RightButton) {
        camera_.StopZooming(event->x(), event->y());
    }
    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up) camera_.Zoom(-1);
    if (event->key() == Qt::Key_Down) camera_.Zoom(1);

    if (event->key() == Qt::Key_Left) camera_.Rotate(-1);
    if (event->key() == Qt::Key_Right) camera_.Rotate(1);

    if (event->key() == Qt::Key_W) camera_.Zoom(-1);
    if (event->key() == Qt::Key_S) camera_.Zoom(1);

    if (event->key() == Qt::Key_A) camera_.Rotate(-1);
    if (event->key() == Qt::Key_D) camera_.Rotate(1);

    if (event->key() == Qt::Key_R) {
        for(auto i = 0; i < programs_.size(); ++i) {
            programs_[i].reset();
            programs_[i] = std::make_unique<QOpenGLShaderProgram>();
            LoadProgram(kShaderFiles[i][0], kShaderFiles[i][1], programs_[i].get());
        }
    }

    update();
}


void GLWidget::paintGL ()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (initialized_) {
        camera_.SetViewport();

        glm::mat4x4 projection = camera_.SetProjection();
        glm::mat4x4 view = camera_.SetView();
        glm::mat4x4 model = camera_.SetModel();

        //compute normal matrix
        glm::mat4x4 t = view * model;
        glm::mat3x3 normal;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                normal[i][j] = t[i][j];
        normal = glm::transpose(glm::inverse(normal));

        if (mesh_ != nullptr) {
            GLint projection_location, view_location, model_location,
                normal_matrix_location, specular_map_location, diffuse_map_location,
                fresnel_location, color_map_location, roughness_map_location, metalness_map_location,
                current_text_location, light_location, roughness_location, metalness_location, prefiltered_map_location,
                brdf_lut_location, max_mips_location, using_color_map_location, using_roughness_map_location, using_metalness_map_location;

            //MESH-----------------------------------------------------------------------------------------
            //general shader setting

            programs_[currentShader_]->bind();

            projection_location       = programs_[currentShader_]->uniformLocation("projection");
            view_location             = programs_[currentShader_]->uniformLocation("view");
            model_location            = programs_[currentShader_]->uniformLocation("model");
            normal_matrix_location    = programs_[currentShader_]->uniformLocation("normal_matrix");
            specular_map_location     = programs_[currentShader_]->uniformLocation("specular_map");
            diffuse_map_location      = programs_[currentShader_]->uniformLocation("diffuse_map");
            color_map_location        = programs_[currentShader_]->uniformLocation("color_map");
            roughness_map_location    = programs_[currentShader_]->uniformLocation("roughness_map");
            metalness_map_location    = programs_[currentShader_]->uniformLocation("metalness_map");
            current_text_location     = programs_[currentShader_]->uniformLocation("current_texture");
            fresnel_location          = programs_[currentShader_]->uniformLocation("fresnel");
            light_location            = programs_[currentShader_]->uniformLocation("light");
            roughness_location        = programs_[currentShader_]->uniformLocation("roughness");
            metalness_location        = programs_[currentShader_]->uniformLocation("metalness");
            prefiltered_map_location  = programs_[currentShader_]->uniformLocation("prefiltered_map");
            brdf_lut_location         = programs_[currentShader_]->uniformLocation("brdf_lut");
            max_mips_location         = programs_[currentShader_]->uniformLocation("max_mips");
            using_color_map_location         = programs_[currentShader_]->uniformLocation("using_color_map");
            using_roughness_map_location         = programs_[currentShader_]->uniformLocation("using_roughness_map");
            using_metalness_map_location         = programs_[currentShader_]->uniformLocation("using_metalness_map");

            if(two_step_rendering){
                //FIRST PASS
                glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                int g_pass_shader = 8;
                programs_[g_pass_shader]->bind();

                GLint projection_location, view_location, model_location,normal_matrix_location, color_map_location, using_color_map_location;
                projection_location       = programs_[g_pass_shader]->uniformLocation("projection");
                view_location             = programs_[g_pass_shader]->uniformLocation("view");
                model_location            = programs_[g_pass_shader]->uniformLocation("model");
                normal_matrix_location    = programs_[g_pass_shader]->uniformLocation("normal_matrix");
                using_color_map_location  = programs_[g_pass_shader]->uniformLocation("using_color_map");
                color_map_location        = programs_[g_pass_shader]->uniformLocation("color_map");

                glUniformMatrix4fv(projection_location, 1, GL_FALSE, &projection[0][0]);
                glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
                glUniformMatrix4fv(model_location, 1, GL_FALSE, &model[0][0]);
                glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, &normal[0][0]);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, color_map_);
                glUniform1i(color_map_location, 2);
                glUniform1i(using_color_map_location, using_color_map);

                DrawFloor();

                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, mesh_->faces_.size(), GL_UNSIGNED_INT, (GLvoid*)0);
                glBindVertexArray(0);

                //SECOND PASS
                glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDisable(GL_DEPTH_TEST);
                int ssao_pass_shader = 9;
                programs_[ssao_pass_shader]->bind();

                GLint gAlbedo_location, gNormal_location, gDepth_location, current_buffer_location, near_plane_location, far_plane_location,
                    radius_location, n_samples_location, n_dirs_location;
                gAlbedo_location = programs_[ssao_pass_shader]->uniformLocation("gAlbedo");
                gNormal_location = programs_[ssao_pass_shader]->uniformLocation("gNormal");
                gDepth_location = programs_[ssao_pass_shader]->uniformLocation("gDepth");
                current_buffer_location = programs_[ssao_pass_shader]->uniformLocation("current_texture");
                near_plane_location = programs_[ssao_pass_shader]->uniformLocation("near_plane");
                far_plane_location = programs_[ssao_pass_shader]->uniformLocation("far_plane");
                radius_location = programs_[ssao_pass_shader]->uniformLocation("radius");
                n_samples_location = programs_[ssao_pass_shader]->uniformLocation("n_samples");
                n_dirs_location = programs_[ssao_pass_shader]->uniformLocation("n_dirs");

                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_2D, gAlbedoTex);
                glUniform1i(gAlbedo_location, 7);

                glActiveTexture(GL_TEXTURE8);
                glBindTexture(GL_TEXTURE_2D, gNormalTex);
                glUniform1i(gNormal_location, 8);

                glActiveTexture(GL_TEXTURE9);
                glBindTexture(GL_TEXTURE_2D, gDepthTex);
                glUniform1i(gDepth_location, 9);

                glUniform1i(current_buffer_location, currentBuffer_);

                glUniform1f(near_plane_location, kZNear);
                glUniform1f(far_plane_location, kZFar);

                glUniform1f(radius_location, radius);

                glUniform1i(n_samples_location, n_samples);
                glUniform1i(n_dirs_location, n_directions);

                DrawQuad();

                //SHADING PASS
                glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
                int shading_pass_shader = 10;
                programs_[shading_pass_shader]->bind();

                GLint ssao_texture_location;

                ssao_texture_location = programs_[shading_pass_shader]->uniformLocation("ssaoTex");

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, ssaoTex);
                glUniform1i(ssao_texture_location, 0);

                DrawQuad();

            }
            else{
                glUniformMatrix4fv(projection_location, 1, GL_FALSE, &projection[0][0]);
                glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
                glUniformMatrix4fv(model_location, 1, GL_FALSE, &model[0][0]);
                glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, &normal[0][0]);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);
                glUniform1i(specular_map_location, 0);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_map_);
                glUniform1i(diffuse_map_location, 1);

                //TODO(students): active texture location for the following textures:
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, color_map_);
                glUniform1i(color_map_location, 2);

                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, roughness_map_);
                glUniform1i(roughness_map_location, 3);

                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, metalness_map_);
                glUniform1i(metalness_map_location, 4);

                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_CUBE_MAP, prefiltered_map_);
                glUniform1i(prefiltered_map_location, 5);

                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_2D, brdf_lut_);
                glUniform1i(brdf_lut_location, 6);

                //TODO END

                glUniform1i(max_mips_location, num_mips);
                glUniform1i(current_text_location, currentTexture_);
                glUniform3f(fresnel_location, fresnel_[0], fresnel_[1], fresnel_[2]);
                glUniform3f(light_location, 0, 2, 0);
                glUniform1f(roughness_location, roughness_);
                glUniform1f(metalness_location, metalness_);
                glUniform1i(using_color_map_location, using_color_map);
                glUniform1i(using_roughness_map_location, using_roughness_map);
                glUniform1i(using_metalness_map_location, using_metalness_map);



                // TODO(students): Implement draw call of the mesh
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, mesh_->faces_.size(), GL_UNSIGNED_INT, (GLvoid*)0);
                glBindVertexArray(0);
                // TODO END.


                //SKY-----------------------------------------------------------------------------------------
                if(skyVisible_) {
                    //model = camera_.SetIdentity();

                    programs_[programs_.size()-1]->bind();

                    projection_location     = programs_[programs_.size()-1]->uniformLocation("projection");
                    view_location           = programs_[programs_.size()-1]->uniformLocation("view");
                    model_location          = programs_[programs_.size()-1]->uniformLocation("model");
                    normal_matrix_location  = programs_[programs_.size()-1]->uniformLocation("normal_matrix");
                    specular_map_location   = programs_[programs_.size()-1]->uniformLocation("specular_map");
                    diffuse_map_location   = programs_[programs_.size()-1]->uniformLocation("diffuse_map");


                    glUniformMatrix4fv(projection_location, 1, GL_FALSE, &projection[0][0]);
                    glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
                    glUniformMatrix4fv(model_location, 1, GL_FALSE, &model[0][0]);
                    glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, &normal[0][0]);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, specular_map_);
                    glUniform1i(specular_map_location, 0);

                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_map_);
                    glUniform1i(diffuse_map_location, 1);

                    glActiveTexture(GL_TEXTURE5);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, prefiltered_map_);
                    glUniform1i(prefiltered_map_location, 5);

                    glDepthFunc(GL_LEQUAL); //BECAUSE DEFAULT VALUE OF THE DEPTH BUFFER IS 1 SO WE HAVE TO ENSURE TO PAINT THE DEPTH BUFFER INDEPENDENTLY OF THE DEFAULT VALUE
                    // TODO(students): implement the draw call of the sky box
                    glBindVertexArray(VAO_sky);
                    glDrawElements(GL_TRIANGLES, skyFaces_.size(), GL_UNSIGNED_INT, (GLvoid*)0);
                    glBindVertexArray(0);
                    // TODO END.
                    glDepthFunc(GL_LESS);
                }
            }
        }
    }
}

void GLWidget::SetReflection(bool set) {
    if(set) currentShader_ = 2;
    update();
}

void GLWidget::SetPBS(bool set) {
    if(set) currentShader_ = 3;
    update();
}

void GLWidget::SetIBLPBS(bool set) {
    if(set) currentShader_ = 4;
    update();
}

void GLWidget::SetPhong(bool set)
{
    if(set) currentShader_ = 0;
    update();
}

void GLWidget::SetTexMap(bool set)
{
    if(set) currentShader_ = 1;
    update();
}

void GLWidget::SetFresnelR(double r) {
    fresnel_[0] = r;
    update();
}

void GLWidget::SetFresnelG(double g) {
    fresnel_[1] = g;
    update();
}

void GLWidget::SetCurrentTexture(int i)
{
    currentTexture_ = i;
    update();
}

void GLWidget::SetSkyVisible(bool set)
{
    skyVisible_ = set;
    update();
}

void GLWidget::SetFresnelB(double b) {
    fresnel_[2] = b;
    update();
}

void GLWidget::SetMetalness(double d) {
    metalness_ = d;
    update();
}

void GLWidget::SetRoughness(double d) {
    roughness_ = d;
    update();
}

void GLWidget::Set2StepRenderer(bool set){
    two_step_rendering = set;
    update();
}

void GLWidget::SetCurrentBuffer(int i)
{
    currentBuffer_ = i;
    update();
}

void GLWidget::SetN_Samples(int i){
    n_samples = i;
    update();
}
void GLWidget::SetN_Directions(int i){
    n_directions = i;
    update();
}
void GLWidget::SetRadius(double r){
    radius = r;
    update();
}
