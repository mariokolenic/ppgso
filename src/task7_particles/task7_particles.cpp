// Task 7 - Dynamically generate objects in a 3D scene
//        - Implement a particle system where particles have position and speed
//        - Any object can be a generator and can add objects to the scene
//        - Create dynamic effect such as fireworks, rain etc.
//        - Encapsulate camera in a class

#include <iostream>
#include <vector>
#include <map>
#include <list>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

const unsigned int SIZE = 512;

class Camera {
public:
  // TODO: Add parameters
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;
  glm::vec3 position = {1, 1, -15};
  glm::vec3 center = {0, 0, 0};

  /// Representaiton of
  /// \param fov - Field of view (in degrees)
  /// \param ratio - Viewport ratio (width/height)
  /// \param near - Distance of the near clipping plane
  /// \param far - Distance of the far clipping plane
  Camera(float fov = 45.0f, float ratio = 1.0f, float near = 0.1f, float far = 10.0f) {
    // TODO: Initialize perspective projection (hint: glm::perspective)
    projectionMatrix = glm::perspective(fov * ppgso::PI / 180.0f, ratio, near, far);
  }

  /// Recalculate viewMatrix from position, rotation and scale
  void update() {
    // TODO: Update viewMatrix (hint: glm::lookAt)
      viewMatrix = glm::lookAt(position,
                               center,
                               glm::vec3(0.0f, 1.0f, 0.0f));
  }
};

/// Abstract renderable object interface
class Renderable; // Forward declaration for Scene
using Scene = std::list<std::unique_ptr<Renderable>>; // Type alias

class Renderable {
public:
  // Virtual destructor is needed for abstract interfaces
  virtual ~Renderable() = default;

  /// Render the object
  /// \param camera - Camera to use for rendering
  virtual void render(const Camera& camera) = 0;

  /// Update the object. Useful for specifing animation and behaviour.
  /// \param dTime - Time delta
  /// \param scene - Scene reference
  /// \return - Return true to keep object in scene
  virtual bool update(float dTime, Scene &scene) = 0;

  virtual char getType() = 0;
  virtual glm::vec3 getPosition() = 0;
  virtual glm::vec3 getSpeed() = 0;
};

/// Basic particle that will render a sphere
/// TODO: Implement Renderable particle
class CovidParticle final : public Renderable {
    // Static resources shared between all particles
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;

    // TODO: add more parameters as needed
public:
    glm::vec3 position;
    glm::vec3 speed;
    glm::vec3 color;
    glm::vec3 scale = {.3, .3, .3};

    glm::mat4 modelMatrix;
    float age = 0;
    /// Construct a new Particle
    /// \param p - Initial position
    /// \param s - Initial speed
    /// \param c - Color of particle
    CovidParticle(glm::vec3 p, glm::vec3 s, glm::vec3 c) {
        // First particle will initialize resources
        if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
        if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
        position = p;
        speed = s;
        color = c;
    }

    glm::vec3 getPosition() override {}

    glm::vec3 getSpeed() override {}

    char getType() override {}


    bool update(float dTime, Scene &scene) override {
        // TODO: Animate position using speed and dTime.
        // - Return true to keep the object alive
        // - Returning false removes the object from the scene
        // - hint: you can add more particles to the scene here also
        age+=dTime;
        if(age >= 2.0f)
            return false;

        speed = (speed / (age)) * 0.075f;
        position = position + (speed * dTime);
        modelMatrix = translate(glm::mat4{1}, position) *
                glm::scale(glm::mat4{1},scale);
        return true;
    }

    void render(const Camera& camera) override {
        // TODO: Render the object
        // - Use the shader
        // - Setup all needed shader inputs
        // - hint: use OverallColor in the color_vert_glsl shader for color
        // - Render the mesh
        shader->use();
        shader->setUniform("ProjectionMatrix", camera.projectionMatrix);
        shader->setUniform("ViewMatrix", camera.viewMatrix);
        shader->setUniform("ModelMatrix", modelMatrix);
        shader->setUniform("OverallColor", color);
        mesh->render();
    }


};
// Static resources need to be instantiated outside of the class as they are globals
std::unique_ptr<ppgso::Mesh> CovidParticle::mesh;
std::unique_ptr<ppgso::Shader> CovidParticle::shader;

/// Basic particle that will render a sphere
/// TODO: Implement Renderable particle
class HumanParticle final : public Renderable {
  // Static resources shared between all particles
  static std::unique_ptr<ppgso::Mesh> mesh;
  static std::unique_ptr<ppgso::Shader> shader;

  // TODO: add more parameters as needed
public:
    glm::vec3 position;
    glm::vec3 speed;
    glm::vec3 color;
    glm::vec3 scale = {5, 5, 5};
    glm::vec3 rotation;

    glm::mat4 modelMatrix;
    char type = 'H';
    float age = 0;
  /// Construct a new Particle
  /// \param p - Initial position
  /// \param s - Initial speed
  /// \param c - Color of particle
  HumanParticle(glm::vec3 p, glm::vec3 s, glm::vec3 c) {
    // First particle will initialize resources
    if (!shader) shader = std::make_unique<ppgso::Shader>(color_vert_glsl, color_frag_glsl);
    if (!mesh) mesh = std::make_unique<ppgso::Mesh>("sphere.obj");
    position = p;
    speed = s;
    color = c;
  }

    glm::vec3 getPosition() override {
        return position;
    }

    glm::vec3 getSpeed() override {
        return speed;
    }

    char getType() override {
        return type;
    }

    bool update(float dTime, Scene &scene) override {
    // TODO: Animate position using speed and dTime.
    // - Return true to keep the object alive
    // - Returning false removes the object from the scene
    // - hint: you can add more particles to the scene here also
    age+=dTime;
    if(age >= 3.0f)
        return false;

    position = position + (speed * dTime);
    modelMatrix = translate(glm::mat4{1}, position) *
                rotate(glm::mat4{1}, rotation.z, {0,0,1}) *
                glm::scale(glm::mat4{1},scale);


    return true;
  }

  void render(const Camera& camera) override {
    // TODO: Render the object
    // - Use the shader
    // - Setup all needed shader inputs
    // - hint: use OverallColor in the color_vert_glsl shader for color
    // - Render the mesh
    shader->use();
    shader->setUniform("ProjectionMatrix", camera.projectionMatrix);
    shader->setUniform("ViewMatrix", camera.viewMatrix);
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("OverallColor", color);
    mesh->render();
  }
};
// Static resources need to be instantiated outside of the class as they are globals
std::unique_ptr<ppgso::Mesh> HumanParticle::mesh;
std::unique_ptr<ppgso::Shader> HumanParticle::shader;

class ParticleWindow : public ppgso::Window {
private:
  // Scene of objects
  Scene scene;

  // Create camera
  Camera camera = {120.0f, (float)width/(float)height, 1.0f, 400.0f};

  // Store keyboard state
  std::map<int, int> keys;
public:
  ParticleWindow() : Window{"task7_particles", SIZE, SIZE} {
    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
  }

  void onKey(int key, int scanCode, int action, int mods) override {
    // Collect key state in a map
    keys[key] = action;

    if (keys[GLFW_KEY_SPACE]) {
        printf("Pressed: Space\n");
      // TODO: Add renderable object to the scene
      glm::vec3 newPosition = {glm::linearRand(-10.0f,10.0f),glm::linearRand(-10.0f,10.0f),0};
      glm::vec3 newSpeed = {glm::linearRand(-15.0f,15.0f),glm::linearRand(-15.0f,15.0f),0};
      glm::vec3 newColor = {1,1,0};

      auto particle=std::make_unique<HumanParticle>(newPosition, newSpeed, newColor);
      scene.push_back(std::move(particle));
    }
    if (keys[GLFW_KEY_C]) {
        printf("Pressed: C\n");
        for(auto& object : scene) {
            if(object->getType() == 'H') {
                for(int i = 0; i < 20; i++) {
                    glm::vec3 newPosition = object->getPosition();
                    glm::vec3 newSpeed = {glm::linearRand(object->getSpeed().x - 1.5f, object->getSpeed().x + 1.5f),
                                          glm::linearRand(object->getSpeed().y - 1.5f, object->getSpeed().y + 1.5f),
                                          glm::linearRand(object->getSpeed().z - 1.5f, object->getSpeed().z + 1.5f)};
                    // glm::vec3 newSpeed = {glm::linearRand(5.0f, 10.0f), glm::linearRand(5.0f, 10.0f), glm::linearRand(5.0f, 10.0f)};
                    glm::vec3 newColor = {1,0,0};

                    auto particle=std::make_unique<CovidParticle>(newPosition, newSpeed, newColor);
                    scene.push_back(std::move(particle));
                }
            }
        }
    }

  }

  void onIdle() override {
    // Track time
    static auto time = (float) glfwGetTime();
    // Compute time delta
    float dTime = (float)glfwGetTime() - time;
    time = (float) glfwGetTime();

    // Set gray background
    glClearColor(0,0,0,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update all objects in scene
    // Because we need to delete while iterating this is implemented using c++ iterators
    // In most languages mutating the container during iteration is undefined behaviour
    auto i = std::begin(scene);
    while (i != std::end(scene)) {
      // Update object and remove from list if needed
      auto obj = i->get();
      if (!obj->update(dTime, scene))
        i = scene.erase(i);
      else
        ++i;
    }

    // Render every object in scene
    for(auto& object : scene) {
      object->render(camera);
    }

    camera.update();
  }
};

int main() {
  // Create new window
  auto window = ParticleWindow{};

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
