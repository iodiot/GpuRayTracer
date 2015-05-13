#version 140

// Definitions
struct Sphere
{
	vec3 center;
	float radius;
	vec3 color;
};

struct Plane
{
	vec3 point;
	vec3 normal;
	vec3 color;
};

struct Cylinder
{
	vec3 center;	
	float radius;
	vec3 color;
};

struct ShadeData
{
	float t;
	vec3 normal;
	vec3 color;
};

// In
in vec3 texCoord;
in vec3 cameraU;
in vec3 cameraW;
in vec3 cameraV;
in vec3 cameraRotatedPosition;

// Out
out vec4 fragColor;

// Consts
const float BIG_FLOAT = 100500.0;

// Uniforms
uniform mat4 rotationMatrix;
uniform float cameraViewPlaneDistance;

// Objects
Sphere firstSphere = Sphere(vec3(0.0, 0.0, 0.0), 20.0, vec3(1.0, 0.0, 1.0));
Sphere secondSphere = Sphere(vec3(50.0, 30.0, 50.0), 20.0, vec3(1.0, 1.0, 0.0));
Sphere thirdSphere = Sphere(vec3(40.0, 10.0, -40.0), 10.0, vec3(0.0, 1.0, 0.0));
Plane firstPlane = Plane(vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.5, 0.6, 0.7));
Cylinder firstCylinder = Cylinder(vec3(-100.0, 0, -100.0), 10.0, vec3(0.0, 0.0, 1.0));

// Screen
uniform int screenWidth;
uniform int screenHeight;

// Lighting
const vec3 lightPosition = vec3(10.0, 50.0, 10.0);

bool IntersectsSphere(in Sphere s, in vec3 ro, in vec3 rd, in float tm, out ShadeData sd)
{
	vec3 temp = ro - s.center;
	float a = dot(rd, rd);
	float b = 2.0 * dot(temp, rd);
	float c = dot(temp, temp) - s.radius * s.radius;
	float disc = b * b - 4.0 * a * c;

	// No roots?
	if (disc < 0)
		return false;

	float e = sqrt(disc);
	float denom = 2.0 * a;

	// First root
	sd.t = (-b - e) / denom;	
	if (sd.t > 0 && sd.t < tm)
	{
		sd.normal = (temp + rd * sd.t) / s.radius;
		sd.color = s.color;
		return true;
	}

	// Second root
	sd.t = (-b + e) / denom;		
	if (sd.t > 0 && sd.t < tm)
	{
		sd.normal = (temp + rd * sd.t) / s.radius;
		sd.color = s.color;
		return true;
	}

	return false;
}

bool IntersectsPlane(in Plane p, in vec3 ro, in vec3 rd, in float tm, out ShadeData sd)
{
	sd.t = dot(p.point - ro, p.normal) / dot(rd, p.normal);

	if (sd.t > 0 && sd.t < tm)
	{
		vec3 hitPoint = ro + rd * sd.t;

		sd.normal = p.normal;

		// Chess plates
		bool f1 = mod(hitPoint.x, 50.0) < 25.0;
		bool f2 = mod(hitPoint.z, 50.0) < 25.0;
		if (f1 ^^ f2)
			sd.color = vec3(1.0, 1.0, 1.0);
		else
			sd.color = vec3(0.3, 0.3, 0.3);
		
		return true;
	}

	return false;
}

bool IntersectsCylinder(in Cylinder c, in vec3 ro, in vec3 rd, in float tm, out ShadeData sd)
{
	ro.y = 0; 
	rd.y = 0;

	return IntersectsSphere(Sphere(vec3(c.center.x, 0, c.center.y), c.radius, c.color), ro, rd, tm, sd);
}

bool IntersectsSomething(in vec3 ro, in vec3 rd, out ShadeData sd)
{
	float tm = BIG_FLOAT;
	ShadeData rsd;
	bool r = false;

	if (IntersectsSphere(firstSphere, ro, rd, tm, rsd))
	{
		tm = rsd.t;
		sd = rsd;
		r = true;
	}

	if (IntersectsSphere(secondSphere, ro, rd, tm, rsd))
	{
		tm = rsd.t;
		sd = rsd;
		r = true;
	}

	if (IntersectsSphere(thirdSphere, ro, rd, tm, rsd))
	{
		tm = rsd.t;
		sd = rsd;
		r = true;
	}

	if (IntersectsPlane(firstPlane, ro, rd, tm, rsd))
	{
		tm = rsd.t;
		sd = rsd;
		r = true;
	}

	if (IntersectsCylinder(firstCylinder, ro, rd, tm, rsd))
	{
		tm = rsd.t;
		sd = rsd;
		r = true;
	}

	return r;
}

vec3 CalcRayDirection()
{
	vec3 r;

	float x = (texCoord.x - 0.5) * float(screenWidth);
	float y = (texCoord.y - 0.5) * float(screenHeight);

	r = x * cameraU + y * cameraV - cameraViewPlaneDistance * cameraW;
	
	return normalize(r); 
}

vec3 CalcShadow(in vec3 lp, in vec3 ro, in vec3 rd, in ShadeData sd)
{
	// In shadow?
	vec3 tro = ro + rd * (sd.t - 0.01);
	vec3 trd = normalize(lightPosition - tro);
	ShadeData tsd;
	if (IntersectsSomething(tro, trd, tsd) && tsd.t <= length(lightPosition - tro))
		return sd.color * 0.1;

	// Diffuse
	vec3 s = normalize(lp - (ro + rd * sd.t));
	return sd.color * max(dot(s, sd.normal), 0.0);
}

void main()
{
	// Define ray
	vec3 ro = cameraRotatedPosition;
	vec3 rd = CalcRayDirection();

	ShadeData sd;

	if (IntersectsSomething(ro, rd, sd))
	{
		vec3 finalColor = CalcShadow(lightPosition, ro, rd, sd);
	
		// Cast reflection ray
		ro = ro + rd * (sd.t - 0.01);
		rd = reflect(rd, sd.normal);

		if (IntersectsSomething(ro, rd, sd))
		{
			finalColor += 0.5 * CalcShadow(lightPosition, ro, rd, sd);
			
			// Cast secondary reflection ray
			ro = ro + rd * (sd.t - 0.01);
			rd = reflect(rd, sd.normal);
			if (IntersectsSomething(ro, rd, sd))
				finalColor += 0.25 * CalcShadow(lightPosition, ro, rd, sd);
		}

		fragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
	}
}