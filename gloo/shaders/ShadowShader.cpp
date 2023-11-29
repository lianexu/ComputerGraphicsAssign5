#include "ShadowShader.hpp"

#include <stdexcept>

#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/PointLight.hpp"

namespace GLOO {
ShadowShader::ShadowShader()
    : ShaderProgram(std::unordered_map<GLenum, std::string>(
          {{GL_VERTEX_SHADER, "shadow.vert"},
           {GL_FRAGMENT_SHADER, "shadow.frag"}})) {
}

void ShadowShader::AssociateVertexArray(VertexArray& vertex_array) const {
  if (!vertex_array.HasPositionBuffer()) {
    throw std::runtime_error("Shadow shader requires vertex positions!");
  }
  vertex_array.LinkPositionBuffer(GetAttributeLocation("vertex_position"));
}

void ShadowShader::SetTargetNode(const SceneNode& node,
                                 const glm::mat4& model_matrix) const {
  // Associate the right VAO before rendering.
  AssociateVertexArray(node.GetComponentPtr<RenderingComponent>()
                           ->GetVertexObjectPtr()
                           ->GetVertexArray());

  // Set transform.
  SetUniform("model_matrix", model_matrix);

  // Set material.
  MaterialComponent* material_component_ptr =
      node.GetComponentPtr<MaterialComponent>();
  if (material_component_ptr == nullptr) {
    // Default material: greenish.
    SetUniform("material_color", glm::vec3(0.0f, 0.7f, 0.2f));
  } else {
    SetUniform("material_color",
               material_component_ptr->GetMaterial().GetDiffuseColor());
  }
}

void ShadowShader::SetCamera2(const glm::mat4& world_to_light_ndc) const {
  SetUniform("world_to_light_ndc_matrix", world_to_light_ndc);
}

}  // namespace GLOO
