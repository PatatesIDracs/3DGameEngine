#include "jpPhysicsRigidBody.h"


jpPhysicsRigidBody::jpPhysicsRigidBody(physx::PxPhysics* px_physics)
{
	//Create a default rigidbody 
	//default transform == identity
	//default geometry == box with 0.5 halfExtent
	//default material: 0.5 friction(static and dynamic) and 0.5 restitution
	body_material = px_physics->createMaterial(0.5f, 0.5f, 0.5f);
	body_shape = px_physics->createShape(physx::PxBoxGeometry(0.5f,0.5f,0.5f), *body_material);
	px_body = physx::PxCreateDynamic(*px_physics, physx::PxTransform(physx::PxIDENTITY()), *body_shape, 1.0f);
	
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
	if (body_shape != nullptr)
		px_body->attachShape(*body_shape);
}

void jpPhysicsRigidBody::SetDynamic(bool is_dynamic)
{
	if (is_dynamic)
	{
		px_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);
	}
	else
	{
		px_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	}
}

void jpPhysicsRigidBody::SetTransform(float * trans_mat)
{
	physx::PxMat44 mat = physx::PxMat44(trans_mat);
	px_body->setGlobalPose(physx::PxTransform(mat));
}

void jpPhysicsRigidBody::SetGeometry(physx::PxGeometry new_geometry)
{
	px_body->detachShape(*body_shape);
	body_shape = px_body->createShape(new_geometry, *body_material);
	px_body->attachShape(*body_shape);
}

void jpPhysicsRigidBody::SetBoxGeometry(float x_scale, float y_scale, float z_scale)
{
	physx::PxBoxGeometry boxGeometry;
	body_shape->getBoxGeometry(boxGeometry);
	boxGeometry.halfExtents.x = x_scale/2;
	boxGeometry.halfExtents.y = y_scale/2;
	boxGeometry.halfExtents.z = z_scale/2;
	body_shape->setGeometry(boxGeometry);
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

void jpPhysicsRigidBody::GetTransform(physx::PxVec3& pos, physx::PxQuat& quat)
{
	physx::PxTransform transf = px_body->getGlobalPose();
	pos = transf.p;
	quat = transf.q;
}