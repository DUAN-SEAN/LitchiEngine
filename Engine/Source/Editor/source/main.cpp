#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include "Runtime/Core/App/application.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Platform/App/application_standalone.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_filter.h"
#include "Runtime/Function/Framework/Component/Renderer/mesh_renderer.h"
#include "Runtime/Function/Framework/Component/Transform/transform.h"
#include "Runtime/Function/Renderer/material.h"

int main(int argc, char** argv)
{
    // std::filesystem::path executable_path(argv[0]);

    Application application;
    auto application_standalone=new ApplicationStandalone();
    application_standalone->SetDataPath("../Assets/");
    application.Initiliaze(application_standalone);
    application.GetDataPath();

    // ������Ϸ����
    GameObject* o = new GameObject("Person");

    // ����Transform���
    auto transformComp =  o->AddComponent<Transform>();

    // ����Mesh��� ����Mesh
    auto meshFilterComp =o->AddComponent<MeshFilter>();
    meshFilterComp->LoadMesh("model/fishsoup_pot.mesh");

    // ����MeshRenderer��� ����Material
    auto meshRendererComp = o->AddComponent<MeshRenderer>();
    Material* material = new Material();
    material->Parse("material/fishsoup_pot.mat");
    meshRendererComp->SetMaterial(material);

    application.Run();

    delete o;
    delete application_standalone;

    return 0;
}
