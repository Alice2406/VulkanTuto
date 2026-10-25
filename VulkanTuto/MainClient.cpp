#include "Application.h"

int main()
{
    std::vector<Vertex> triangleVertices = {
            { { 0.0f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.5f, 0.0f } },
            { { 0.5f,  0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
            { { -0.5f,  0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } }
    };
    std::vector<uint16_t> triangleIndices = { 0, 1, 2 };

    Application app;
    app.Init(triangleVertices, triangleIndices);
    app.Run();
    app.Destroy();

    return 0;
}
