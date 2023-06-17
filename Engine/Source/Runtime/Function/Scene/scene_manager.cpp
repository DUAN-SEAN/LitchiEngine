#include "scene_manager.h"

#include "Runtime/Core/Meta/Serializer/serializer.h"
#include "Runtime/Function/Framework/GameObject/game_object.h"
#include "Runtime/Resource/asset_manager.h"

namespace LitchiRuntime
{
    Scene::Scene():name_("default")
    {
    }
    Scene::Scene(std::string name)
    {
        name_ = name;
    }
    Scene::~Scene()
    {
        // �������е�GO
        for (auto iter = game_object_vec_.begin(); iter != game_object_vec_.end(); iter++) {
            GameObject* game_object = *iter;
			delete game_object;
        }
        game_object_vec_.clear();
    }

    void Scene::AddGameObject(GameObject* game_object)
    {
        // ��go��ӵ�game_object_vec_��
        game_object_vec_.push_back(game_object);
        game_object_tree_.root_node()->AddChild(game_object);
    }

    void Scene::Foreach(std::function<void(GameObject* game_object)> func)
    {
        game_object_tree_.Post(game_object_tree_.root_node(), [&func](Tree::Node* node) {
            auto n = node;
            GameObject* game_object = dynamic_cast<GameObject*>(n);
            func(game_object);
            });
    }
    GameObject* Scene::Find(const char* name) {
        GameObject* game_object_find = nullptr;
        game_object_tree_.Find(game_object_tree_.root_node(), [&name](Tree::Node* node) {
            GameObject* game_object = dynamic_cast<GameObject*>(node);
            if (game_object->name() == name) {
                return true;
            }
            return false;
            }, reinterpret_cast<Tree::Node**>(&game_object_find));
        return game_object_find;
    }

    SceneManager::SceneManager(std::string sceneRootFolderPath)
    {
    }

    SceneManager::~SceneManager()
    {
    }

    bool SceneManager::LoadScene(std::string path)
    {
        std::string completePath = m_sceneRootFolderPath + path;
        Scene* scene = new Scene("Temp");
    	AssetManager::LoadAsset(completePath,*scene);

    	// ��scene��ӵ�map��
        scene_map_[scene->GetName()] = scene;

        // ��ʼ��Scene�����е�GameObject,����GameObject�Ĳ㼶��ϵ
        SetCurrentSceneSourcePath(completePath);
        SetCurrentScene(scene);

        return true;
    }

    void SceneManager::SaveCurrentSceneTo(const std::string& completePath)
    {
        AssetManager::SaveAsset<Scene>(*m_currScene, completePath);

        // ���õ�ǰ�����ı���·��
        SetCurrentSceneSourcePath(completePath);
    }

    Scene* SceneManager::CreateScene(std::string sceneName)
    {
        Scene* scene = new Scene(sceneName);

        // ��scene��ӵ�map��
        scene_map_[sceneName] = scene;

        // ��ʼ��Scene�����е�GameObject,����GameObject�Ĳ㼶��ϵ
        SetCurrentSceneSourcePath("");
        SetCurrentScene(scene);

        return scene;
    }

    bool SceneManager::DestroyScene(std::string sceneName)
    {
        auto scene = scene_map_[sceneName];
        scene_map_.erase(sceneName);
        delete scene;

        return true;
    }

    void SceneManager::Foreach(std::function<void(GameObject* game_object)> func)
    {
        for (auto iter = scene_map_.begin(); iter != scene_map_.end(); iter++) {
            Scene* scene = iter->second;
            scene->Foreach(func);
            //current_camera_->CheckCancelRenderToTexture();
        }
    }


}
