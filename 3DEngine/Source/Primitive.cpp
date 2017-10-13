#include "Glew\include\glew.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"




// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

vec3 Primitive::GetPosition() const
{
	float3 box_pos = bounding_box.CenterPoint();
	return vec3(transform.M[12] + box_pos.x, transform.M[13] + box_pos.y, transform.M[14] + box_pos.z);
}

void Primitive::CalculateFaceNormals(float* vertices)
{
	float3 normal_temp;
	float3 u_temp;
	float3 v_temp;

	render_data.face_normals_pointer = new float[2*render_data.num_vertices];

	//We are calculating face normals
	for (uint i = 0; i < render_data.num_vertices/3; i++) //divided by 3 since each face has 3 vertices
	{
		float3 vertex_1(vertices[9 * i], vertices[9 * i + 1], vertices[9 * i + 2]);
		float3 vertex_2(vertices[9 * i + 3], vertices[9 * i + 4], vertices[9 * i + 5]);
		float3 vertex_3(vertices[9 * i + 6], vertices[9 * i + 7], vertices[9 * i + 8]);


		//Calculate the dir vector of the normal face
		u_temp = vertex_2 - vertex_1;
		v_temp = vertex_3 - vertex_1;
		normal_temp = math::Cross(u_temp, v_temp);
		normal_temp = normal_temp.Normalized() * 0.25; //Multply by 0.25 since the unit vector is too big for small figures

		//Get the face center (to print the normal from it)
		float3 face_center = (vertex_1 + vertex_2 + vertex_3) / 3;

		//Push booth face render and the normal dir vector to the array
		//Makes the drawing part easier to have both values in the same array



		//First push the face_center data
		render_data.face_normals_pointer[6 * i] = face_center.x;
		render_data.face_normals_pointer[6 * i + 1] = face_center.y;
		render_data.face_normals_pointer[6 * i + 2] = face_center.z;

		//Push the face_center + normal dir vector to the array
		render_data.face_normals_pointer[6 * i + 3] = (face_center.x + normal_temp.x);
		render_data.face_normals_pointer[6 * i + 4] = (face_center.y + normal_temp.y);
		render_data.face_normals_pointer[6 * i + 5] = (face_center.z + normal_temp.z);
	
	}

	//Bind the data to the buffer
	
	render_data.num_face_normals = render_data.num_vertices/3;
}

// ------------------------------------------------------------
void Primitive::Render(bool face_normals) const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	InnerRender();

	if (face_normals)
		FaceNormalsRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

void Primitive::FaceNormalsRender() const
{
}

void Primitive::PrepareToRender()
{
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}




// PLANE ==================================================
oldPlane::oldPlane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

oldPlane::oldPlane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void oldPlane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for(float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}