#include "jpPhysicsRigidBody.h"


jpPhysicsRigidBody::jpPhysicsRigidBody(physx::PxPhysics* px_physics)
{
	//Create a default rigidbody 
	//default transform == identity
	//default geometry == box with 0.5 halfExtent
	//default material: 0.5 friction(static and dynamic) and 0.5 restitution
	physx::PxMaterial* default_material = px_physics->createMaterial(0.5f, 0.5f, 0.5f);
	body_shape = px_physics->createShape(physx::PxBoxGeometry(0.5f,0.5f,0.5f), *default_material);
	px_body = physx::PxCreateDynamic(*px_physics, physx::PxTransform(), *body_shape, 1.0f);

	//Detach the shape by default,
	//Shape must be activated using calling a diferent function
	//Made like this to separet the rigidbody from the collider
	px_body->detachShape(*body_shape);
}

jpPhysicsRigidBody::~jpPhysicsRigidBody()
{
}

void jpPhysicsRigidBody::ActivateShape()
{
	px_body->attachShape(*body_shape);
}

void jpPhysicsRigidBody::SetGeometry()
{
	
}

void jpPhysicsRigidBody::SetBoxGeometry(float x_scale, float y_scale, float z_scale)
{
	physx::PxBoxGeometry boxGeometry;
	body_shape->getBoxGeometry(boxGeometry);
	boxGeometry.halfExtents.x = x_scale/2;
	boxGeometry.halfExtents.y = y_scale/2;
	boxGeometry.halfExtents.z = z_scale/2;
}

void jpPhysicsRigidBody::SetMaterial(float static_friction, float dynamic_friction, float restitution)
{
	physx::PxMaterial* material;
	body_shape->getMaterials(&material, 1);
	material->setStaticFriction(static_friction);
	material->setDynamicFriction(dynamic_friction);
	material->setRestitution(restitution);
}

void jpPhysicsRigidBody::SetShape(physx::PxShape * new_shape)
{
	//Might not work
	px_body->detachShape(*body_shape);
	body_shape = new_shape;
}
