#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// ������ � 3D ������������
class Vec3 {
public:
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float f) const { return Vec3(x * f, y * f, z * f); }
    Vec3 operator/(float f) const { return Vec3(x / f, y / f, z / f); }

    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    Vec3 normalize() const {
        float mag = sqrt(x * x + y * y + z * z);
        return Vec3(x / mag, y / mag, z / mag);
    }

    float length() const {
        return sqrt(x * x + y * y + z * z);
    }
};

// ��� (origin - ������, direction - �����������)
class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}
};

// �������� �������
class Material {
public:
    Vec3 color;
    float albedo;

    Material(const Vec3& c, float a) : color(c), albedo(a) {}
};

// ����� � 3D ������������
class Sphere {
public:
    Vec3 center;
    float radius;
    Material material;

    Sphere(const Vec3& c, float r, const Material& m) : center(c), radius(r), material(m) {}

    bool intersect(const Ray& ray, float& t) const {
        Vec3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return false;

        float sqrtd = sqrt(discriminant);
        float t0 = (-b - sqrtd) / (2.0f * a);
        float t1 = (-b + sqrtd) / (2.0f * a);

        t = (t0 < t1) ? t0 : t1;
        return true;
    }
};

// ������� ����������� ����
Vec3 trace(const Ray& ray, const vector<Sphere>& spheres) {
    float t = numeric_limits<float>::max();
    const Sphere* closestSphere = nullptr;

    // ������� ��������� �����, ������������ �����
    for (const auto& sphere : spheres) {
        float t_i;
        if (sphere.intersect(ray, t_i) && t_i < t) {
            t = t_i;
            closestSphere = &sphere;
        }
    }

    if (!closestSphere) return Vec3(0.2f, 0.7f, 0.8f); // ���� ����

    // ��������� ����� ����������� � �������
    Vec3 hitPoint = ray.origin + ray.direction * t;
    Vec3 normal = (hitPoint - closestSphere->center).normalize();

    // ������� ��������� (��������� ���������)
    Vec3 lightDir = Vec3(1, 1, 1).normalize();
    float diffuse = max(0.0f, normal.dot(lightDir));

    // ��������� ������� ���������
    return closestSphere->material.color * diffuse * closestSphere->material.albedo;
}

// ������ ����������� � ������� PPM
void render(const vector<Sphere>& spheres) {
    const int width = 800;
    const int height = 600;
    const float fov = 90.0f;
    const float aspectRatio = float(width) / float(height);

    ofstream out("output.ppm");
    out << "P3\n" << width << " " << height << "\n255\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // ����������� ���������� ������� � ��������������� ����������
            float px = (2.0f * (x + 0.5f) / float(width) - 1.0f) * aspectRatio * tan(fov * 0.5f * M_PI / 180.0f);
            float py = (1.0f - 2.0f * (y + 0.5f) / float(height)) * tan(fov * 0.5f * M_PI / 180.0f);

            Vec3 origin(0, 0, 0);
            Vec3 direction(px, py, -1.0f);
            direction = direction.normalize();

            Ray ray(origin, direction);
            Vec3 color = trace(ray, spheres);

            // ����������� ���� � �������� 0-255
            int r = min(255, static_cast<int>(color.x * 255));
            int g = min(255, static_cast<int>(color.y * 255));
            int b = min(255, static_cast<int>(color.z * 255));

            out << r << " " << g << " " << b << "\n";
        }
    }

    setlocale(LC_ALL, "RU");
    out.close();
    cout << "��������� ��������. ����������� ��������� � output.ppm\n";
}

int main() {
    // ������ �����
    vector<Sphere> spheres;
    spheres.emplace_back(Vec3(0, 0, -5), 1, Material(Vec3(1, 0, 0), 0.9f)); // �������
    spheres.emplace_back(Vec3(2, 0, -5), 1, Material(Vec3(0, 1, 0), 0.9f)); // ������
    spheres.emplace_back(Vec3(-2, 0, -5), 1, Material(Vec3(0, 0, 1), 0.9f)); // �����

    // �������� �����
    render(spheres);

    return 0;
}