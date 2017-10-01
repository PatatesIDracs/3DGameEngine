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

// ------------------------------------------------------------
void Primitive::Render() const
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

	/*if(wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/

	InnerRender();

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

// CUBE ============================================
oldCube::oldCube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

oldCube::oldCube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void oldCube::InnerRender() const
{	
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f( sx, -sy, sz);
	glVertex3f( sx,  sy, sz);
	glVertex3f(-sx,  sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f( sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx,  sy, -sz);
	glVertex3f( sx,  sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy,  sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx,  sy, -sz);
	glVertex3f(sx,  sy,  sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy,  sz);
	glVertex3f(-sx,  sy,  sz);
	glVertex3f(-sx,  sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy,  sz);
	glVertex3f( sx, sy,  sz);
	glVertex3f( sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f( sx, -sy, -sz);
	glVertex3f( sx, -sy,  sz);
	glVertex3f(-sx, -sy,  sz);

	glEnd();
}

// SPHERE ============================================
oldSphere::oldSphere() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

oldSphere::oldSphere(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
	geo_sphere = Sphere(math::vec(-5.f,0.f,0.f),radius);
	vertices3 = new float3[36];
	//geo_sphere.Triangulate(vertices3, nullptr, nullptr, 344, false);

	stacks = 10;
	slices = 10;




	for (int t = 0; t < stacks; t++)
	{
		float theta1 = ((float)t / stacks)*2*pi;
		float theta2 = ((float)(t + 1) / stacks)*2*pi;

		for (int p = 0; p < slices; p++)
		{
			float phi1 = ((float)p / slices)* pi;
			float phi2 = ((float)(p + 1) / slices) * pi;

			vec3 vertex1;
			vec3 vertex2;
			vec3 vertex3;
			vec3 vertex4;

			vertex1.z = radius * Sin(theta1) * Cos(phi1);
			vertex1.x = radius * Sin(theta1) * Sin(phi1);
			vertex1.y = radius * Cos(theta1);

			vertex2.z = radius * Sin(theta1) * Cos(phi2);
			vertex2.x = radius * Sin(theta1) * Sin(phi2);
			vertex2.y = radius * Cos(theta1);


			vertex3.z = radius * Sin(theta2) * Cos(phi2);
			vertex3.x = radius * Sin(theta2) * Sin(phi2);
			vertex3.y = radius * Cos(theta2);


			vertex4.z = radius * Sin(theta2) * Cos(phi1);
			vertex4.x = radius * Sin(theta2) * Sin(phi1);
			vertex4.y = radius * Cos(theta2);


			if (t == 0)
			{
				vertex_array.push_back(vertex1.x);
				vertex_array.push_back(vertex1.y);
				vertex_array.push_back(vertex1.z);

				vertex_array.push_back(vertex4.x);
				vertex_array.push_back(vertex4.y);
				vertex_array.push_back(vertex4.z);


				vertex_array.push_back(vertex3.x);
				vertex_array.push_back(vertex3.y);
				vertex_array.push_back(vertex3.z);
			}
			else if (t + 1 == stacks)
			{
				vertex_array.push_back(vertex1.x);
				vertex_array.push_back(vertex1.y);
				vertex_array.push_back(vertex1.z);

				vertex_array.push_back(vertex3.x);
				vertex_array.push_back(vertex3.y);
				vertex_array.push_back(vertex3.z);

				vertex_array.push_back(vertex2.x);
				vertex_array.push_back(vertex2.y);
				vertex_array.push_back(vertex2.z);
			}
			else
			{
				vertex_array.push_back(vertex1.x);
				vertex_array.push_back(vertex1.y);
				vertex_array.push_back(vertex1.z);

				vertex_array.push_back(vertex4.x);
				vertex_array.push_back(vertex4.y);
				vertex_array.push_back(vertex4.z);

				vertex_array.push_back(vertex2.x);
				vertex_array.push_back(vertex2.y);
				vertex_array.push_back(vertex2.z);


				vertex_array.push_back(vertex2.x);
				vertex_array.push_back(vertex2.y);
				vertex_array.push_back(vertex2.z);

				vertex_array.push_back(vertex4.x);
				vertex_array.push_back(vertex4.y);
				vertex_array.push_back(vertex4.z);

				vertex_array.push_back(vertex3.x);
				vertex_array.push_back(vertex3.y);
				vertex_array.push_back(vertex3.z);
			}
		}
	}




}

oldSphere::~oldSphere()
{
	delete vertices3;
}

void oldSphere::InnerRender() const
{
	GLuint sphere_id = 0;
	GLfloat magic_vertices[344 * 3];




	glGenBuffers(1, (GLuint*)&sphere_id);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_id);
	glBufferData(GL_ARRAY_BUFFER, vertex_array.size() * sizeof(float), &vertex_array[0], GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_TRIANGLES, 0, vertex_array.size()/3);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*
	for (int i = 0; i < 344; i++)
	{
		magic_vertices[i * 3] = vertices3[i].x;
		magic_vertices[i * 3 + 1] = vertices3[i].y;
		magic_vertices[i * 3 + 2] = vertices3[i].z;
	}
	*/
	/*
	glGenBuffers(1, (GLuint*)&sphere_id);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 344 * 3, &magic_vertices[0], GL_STATIC_DRAW);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_TRIANGLES, 0, 344);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/

	
}


// CYLINDER ============================================
oldCylinder::oldCylinder() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

oldCylinder::oldCylinder(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void oldCylinder::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
oldLine::oldLine() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

oldLine::oldLine(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void oldLine::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
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