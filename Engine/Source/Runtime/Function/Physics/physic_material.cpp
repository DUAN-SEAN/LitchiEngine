
#include "physic_material.h"
namespace LitchiRuntime
{

    PhysicMaterial::PhysicMaterial(float static_friction, float dynamic_friction, float restitution) :
        static_friction_(static_friction),
        dynamic_friction_(dynamic_friction),
        restitution_(restitution)
    {

    }
}