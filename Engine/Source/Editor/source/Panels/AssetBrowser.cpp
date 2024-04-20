
#include "Runtime/Core/pch.h"

#include "Runtime/Function/UI/Plugins/ContextualMenu.h"
#include "Runtime/Function/UI/Widgets/Visual/Image.h"
#include <Runtime/Function/UI/Widgets/InputFields/InputText.h>

#include "Editor/include/Core/EditorActions.h"
#include "Runtime/Core/Tools/Utils/PathParser.h"
#include "Runtime/Core/Tools/Utils/SystemCalls.h"
#include "Runtime/Function/UI/Widgets/Visual/Separator.h"

#include "Editor/include/Panels/AssetBrowser.h"

#include "Runtime/Core/Global/ServiceLocator.h"
#include "Runtime/Core/Window/Dialogs/MessageBox.h"
#include "Runtime/Function/UI/Widgets/Buttons/Button.h"
#include <Runtime/Function/UI/Plugins/DDSource.h>

#include "Editor/include/ApplicationEditor.h"

#include "Editor/include/Panels/AssetView.h"
#include "Editor/include/Panels/MaterialEditor.h"
#include "Runtime/Core/Tools/Utils/String.h"
#include "Runtime/Function/UI/Plugins/DDTarget.h"
#include "Runtime/Function/UI/Widgets/Texts/TextClickable.h"
#include "Runtime/Resource/AssetManager.h"


#define FILENAMES_CHARS LitchiEditor::AssetBrowser::__FILENAMES_CHARS

const std::string FILENAMES_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-_=+ 0123456789()[]";

std::string GetAssociatedMetaFile(const std::string& p_assetPath)
{
	return p_assetPath + ".meta";
}

void RenameAsset(const std::string& p_prev, const std::string& p_new)
{
	std::filesystem::rename(p_prev, p_new);

	if (const std::string previousMetaPath = GetAssociatedMetaFile(p_prev); std::filesystem::exists(previousMetaPath))
	{
		if (const std::string newMetaPath = GetAssociatedMetaFile(p_new); !std::filesystem::exists(newMetaPath))
		{
			std::filesystem::rename(previousMetaPath, newMetaPath);
		}
		else
		{
			DEBUG_LOG_ERROR(newMetaPath + " is already existing, .meta creation failed");
		}
	}
}

void RemoveAsset(const std::string& p_toDelete)
{
	std::filesystem::remove(p_toDelete);

	if (const std::string metaPath = GetAssociatedMetaFile(p_toDelete); std::filesystem::exists(metaPath))
	{
		std::filesystem::remove(metaPath);
	}
}

class TexturePreview : public LitchiRuntime::IPlugin
{
public:
	TexturePreview() : image((uint32_t)0, { 80, 80 })
	{

	}

	void SetPath(const std::string& p_path)
	{
		// todo:
		texture = LitchiEditor::ApplicationEditor::Instance()->textureManager->GetResource(p_path);
	}

	virtual void Execute() override
	{
		if (ImGui::IsItemHovered())
		{
			if (texture)
			{
				image.renderTarget = texture;
			}

			ImGui::BeginTooltip();
			image.Draw();
			ImGui::EndTooltip();
		}
	}

	LitchiRuntime::RHI_Texture* texture = nullptr;
	LitchiRuntime::Image image;
};

class BrowserItemContextualMenu : public ContextualMenu
{
public:
	BrowserItemContextualMenu(const std::string p_filePath, bool p_protected = false) : m_protected(p_protected), filePath(p_filePath) {}

	virtual void CreateList()
	{
		if (!m_protected)
		{
			auto& renameMenu = CreateWidget<MenuList>("Rename to...");
			auto& deleteAction = CreateWidget<MenuItem>("Delete");

			auto& nameEditor = renameMenu.CreateWidget<InputText>("");
			nameEditor.selectAllOnClick = true;

			renameMenu.ClickedEvent += [this, &nameEditor]
			{
				nameEditor.content = PathParser::GetElementName(filePath);

				if (!std::filesystem::is_directory(filePath))
					if (size_t pos = nameEditor.content.rfind('.'); pos != std::string::npos)
						nameEditor.content = nameEditor.content.substr(0, pos);
			};

			deleteAction.ClickedEvent += [this] { DeleteItem(); };

			nameEditor.EnterPressedEvent += [this](std::string p_newName)
			{
				if (!std::filesystem::is_directory(filePath))
					p_newName += '.' + PathParser::GetExtension(filePath);

				/* Clean the name (Remove special chars) */
				p_newName.erase(std::remove_if(p_newName.begin(), p_newName.end(), [](auto& c)
					{
						return std::find(FILENAMES_CHARS.begin(), FILENAMES_CHARS.end(), c) == FILENAMES_CHARS.end();
					}), p_newName.end());

				std::string containingFolderPath = PathParser::GetContainingFolder(filePath);
				std::string newPath = containingFolderPath + p_newName;
				std::string oldPath = filePath;

				if (filePath != newPath && !std::filesystem::exists(newPath))
					filePath = newPath;

				if (std::filesystem::is_directory(oldPath))
					filePath += '/';

				RenamedEvent.Invoke(oldPath, newPath);
			};
		}
	}

	virtual void Execute() override
	{
		if (m_widgets.size() > 0)
			ContextualMenu::Execute();
	}

	virtual void DeleteItem() = 0;

	std::string GetFileFullPath()
	{
		return filePath;
	}

public:
	bool m_protected;
	std::string filePath;// relative path
	Event<std::string> DestroyedEvent;
	Event<std::string, std::string> RenamedEvent;
};

class FolderContextualMenu : public BrowserItemContextualMenu
{
public:
	FolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		auto& showInExplorer = CreateWidget<MenuItem>("Show in explorer");
		showInExplorer.ClickedEvent += [this]
		{
			SystemCalls::ShowInExplorer(filePath);
		};

		if (!m_protected)
		{
			auto& importAssetHere = CreateWidget<MenuItem>("Import Here...");
			importAssetHere.ClickedEvent += [this]
			{
				if (EDITOR_EXEC(ImportAssetAtLocation(filePath)))
				{
					TreeNode* pluginOwner = reinterpret_cast<TreeNode*>(userData);
					pluginOwner->Close();
					EDITOR_EXEC(DelayAction(std::bind(&TreeNode::Open, pluginOwner)));
				}
			};

			auto& createMenu = CreateWidget<MenuList>("Create..");

			auto& createFolderMenu = createMenu.CreateWidget<MenuList>("Folder");
			auto& createSceneMenu = createMenu.CreateWidget<MenuList>("Scene");
			auto& createShaderMenu = createMenu.CreateWidget<MenuList>("Shader");
			auto& createMaterialMenu = createMenu.CreateWidget<MenuList>("Material");

			auto& createStandardShaderMenu = createShaderMenu.CreateWidget<MenuList>("Standard_Lambert template");

			auto& createStandardMaterialMenu = createMaterialMenu.CreateWidget<MenuList>("Standard_Lambert");
			auto& createStandardSkinMaterialMenu = createMaterialMenu.CreateWidget<MenuList>("Standard_Lambert_Skin");
			auto& createStandardImageMaterialMenu = createMaterialMenu.CreateWidget<MenuList>("Standard_Image");
			auto& createStandardTextMaterialMenu = createMaterialMenu.CreateWidget<MenuList>("Standard_Text");

			auto& createFolder = createFolderMenu.CreateWidget<InputText>("");
			auto& createScene = createSceneMenu.CreateWidget<InputText>("");

			auto& createStandardMaterial = createStandardMaterialMenu.CreateWidget<InputText>("");
			auto& createStandardSkinMaterial = createStandardSkinMaterialMenu.CreateWidget<InputText>("");
			auto& createStandardImageMaterial = createStandardImageMaterialMenu.CreateWidget<InputText>("");
			auto& createStandardTextMaterial = createStandardTextMaterialMenu.CreateWidget<InputText>("");

			auto& createStandardShader = createStandardShaderMenu.CreateWidget<InputText>("");

			createFolderMenu.ClickedEvent += [&createFolder] { createFolder.content = ""; };
			createSceneMenu.ClickedEvent += [&createScene] { createScene.content = ""; };
			createStandardShaderMenu.ClickedEvent += [&createStandardShader] { createStandardShader.content = ""; };
			createStandardMaterialMenu.ClickedEvent += [&createStandardMaterial] { createStandardMaterial.content = ""; };
			createStandardSkinMaterialMenu.ClickedEvent += [&createStandardSkinMaterial] { createStandardSkinMaterial.content = ""; };
			createStandardImageMaterialMenu.ClickedEvent += [&createStandardImageMaterial] { createStandardImageMaterial.content = ""; };
			createStandardTextMaterialMenu.ClickedEvent += [&createStandardTextMaterial] { createStandardTextMaterial.content = ""; };

			createFolder.EnterPressedEvent += [this](std::string newFolderName)
			{
				size_t fails = 0;
				std::string finalPath;

				do
				{
					finalPath = filePath + (!fails ? newFolderName : newFolderName + " (" + std::to_string(fails) + ')');

					++fails;
				} while (std::filesystem::exists(finalPath));

				std::filesystem::create_directory(finalPath);

				ItemAddedEvent.Invoke(finalPath);
				Close();
			};

			createScene.EnterPressedEvent += [this](std::string newSceneName)
			{
				size_t fails = 0;
				std::string finalPath;

				do
				{
					finalPath = filePath + (!fails ? newSceneName : newSceneName + " (" + std::to_string(fails) + ')') + ".scene";

					++fails;
				} while (std::filesystem::exists(finalPath));

				Scene* scene = new Scene(newSceneName);
				scene->CreateGameObject("Light")->AddComponent<Light>();
				scene->CreateGameObject("Camera")->AddComponent<Camera>();
				AssetManager::SaveAsset(*scene, finalPath);

				delete scene;
				ItemAddedEvent.Invoke(finalPath);
				Close();
			};

			createStandardShader.EnterPressedEvent += [this](std::string newShaderName)
			{
				size_t fails = 0;
				std::string finalPath;

				do
				{
					finalPath = filePath + '/' + (!fails ? newShaderName : newShaderName + " (" + std::to_string(fails) + ')') + ".glsl";

					++fails;
				} while (std::filesystem::exists(finalPath));

				std::filesystem::copy_file(LitchiEditor::ApplicationEditor::Instance()->GetEngineAssetsPath() + ":Shaders/Forward/Standard.hlsl", finalPath);
				ItemAddedEvent.Invoke(finalPath);
				Close();
			};

			createStandardMaterial.EnterPressedEvent += [this](std::string materialName)
			{
				size_t fails = 0;
				std::string finalPath;

				do
				{
					auto relativePath= FileSystem::GetRelativePathAssetFromNative(filePath);
					finalPath = relativePath + (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".mat";

					++fails;
				} while (FileSystem::Exists(finalPath));

				auto material = ApplicationBase::Instance()->materialManager->CreateMaterial(finalPath);
				auto shader = ApplicationBase::Instance()->shaderManager->LoadResource(":Shaders/Forward/Standard.hlsl");
				material->SetShader(shader);
				material->PostResourceLoaded();
				material->SaveToFile(material->GetResourceFilePath());

				if (material)
				{
					auto& materialEditor = EDITOR_PANEL(LitchiEditor::MaterialEditor, "Material Editor");
					materialEditor.SetTarget(material);
					materialEditor.Open();
					materialEditor.Focus();
					materialEditor.Preview();
				}
				ItemAddedEvent.Invoke(finalPath);

				Close();
			};

			BrowserItemContextualMenu::CreateList();
		}
	}

	virtual void DeleteItem() override
	{
		MessageBox message("Delete folder", "Deleting a folder (and all its content) is irreversible, are you sure that you want to delete \"" + filePath + "\"?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::YES_NO);

		if (message.GetUserAction() == MessageBox::EUserAction::YES)
		{
			if (std::filesystem::exists(filePath) == true)
			{
				EDITOR_EXEC(PropagateFolderDestruction(filePath));
				std::filesystem::remove_all(filePath);
				DestroyedEvent.Invoke(filePath);
			}
		}
	}

public:
	Event<std::string> ItemAddedEvent;
};

class ScriptFolderContextualMenu : public FolderContextualMenu
{
public:
	ScriptFolderContextualMenu(const std::string& p_filePath, bool p_protected = false) : FolderContextualMenu(p_filePath, p_protected) {}

	void CreateScript(const std::string& p_name, const std::string& p_path)
	{
		std::string fileContent = "local " + p_name + " =\n{\n}\n\nfunction " + p_name + ":OnStart()\nend\n\nfunction " + p_name + ":OnUpdate(deltaTime)\nend\n\nreturn " + p_name;

		std::ofstream outfile(p_path);
		outfile << fileContent << std::endl; // Empty scene content

		ItemAddedEvent.Invoke(p_path);
		Close();
	}

	virtual void CreateList() override
	{
		FolderContextualMenu::CreateList();

		auto& newScriptMenu = CreateWidget<MenuList>("New script...");
		auto& nameEditor = newScriptMenu.CreateWidget<InputText>("");

		newScriptMenu.ClickedEvent += [this, &nameEditor]
		{
			nameEditor.content = PathParser::GetElementName("");
		};

		nameEditor.EnterPressedEvent += [this](std::string p_newName)
		{
			/* Clean the name (Remove special chars) */
			p_newName.erase(std::remove_if(p_newName.begin(), p_newName.end(), [](auto& c)
				{
					return std::find(FILENAMES_CHARS.begin(), FILENAMES_CHARS.end(), c) == FILENAMES_CHARS.end();
				}), p_newName.end());

			std::string newPath = filePath + p_newName + ".lua";

			if (!std::filesystem::exists(newPath))
			{
				CreateScript(p_newName, newPath);
			}
		};
	}
};

class FileContextualMenu : public BrowserItemContextualMenu
{
public:
	FileContextualMenu(const std::string& p_filePath, bool p_protected = false) : BrowserItemContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		auto& editAction = CreateWidget<MenuItem>("Open");

		editAction.ClickedEvent += [this]
		{
			SystemCalls::OpenFile(filePath);
		};

		if (!m_protected)
		{
			auto& duplicateAction = CreateWidget<MenuItem>("Duplicate");

			duplicateAction.ClickedEvent += [this]
			{
				std::string filePathWithoutExtension = filePath;

				if (size_t pos = filePathWithoutExtension.rfind('.'); pos != std::string::npos)
					filePathWithoutExtension = filePathWithoutExtension.substr(0, pos);

				std::string extension = "." + PathParser::GetExtension(filePath);

				auto filenameAvailable = [&extension](const std::string& target)
				{
					return !std::filesystem::exists(target + extension);
				};

				const auto newNameWithoutExtension = String::GenerateUnique(filePathWithoutExtension, filenameAvailable);

				std::string finalPath = newNameWithoutExtension + extension;
				std::filesystem::copy(filePath, finalPath);

				DuplicateEvent.Invoke(finalPath);
			};
		}

		BrowserItemContextualMenu::CreateList();


		auto& editMetadata = CreateWidget<MenuItem>("Properties");

		editMetadata.ClickedEvent += [this]
		{
			/* auto& panel = EDITOR_PANEL(LitchiEditor::AssetProperties, "Asset Properties");
			 std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
			 panel.SetTarget(resourcePath);
			 panel.Open();
			 panel.Focus();*/
		};
	}

	virtual void DeleteItem() override
	{
		MessageBox message("Delete file", "Deleting a file is irreversible, are you sure that you want to delete \"" + filePath + "\"?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::YES_NO);

		if (message.GetUserAction() == MessageBox::EUserAction::YES)
		{
			RemoveAsset(filePath);
			DestroyedEvent.Invoke(filePath);
			EDITOR_EXEC(PropagateFileRename(filePath, "?"));
		}
	}

public:
	Event<std::string> DuplicateEvent;
};

template<typename Resource, typename ResourceLoader>
class PreviewableContextualMenu : public FileContextualMenu
{
public:
	PreviewableContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		// todo:
		auto& previewAction = CreateWidget<MenuItem>("Preview");

		previewAction.ClickedEvent += [this]
		{
			auto relativePath = FileSystem::GetRelativePathAssetFromNative(filePath);
			Resource* resource = LitchiRuntime::ServiceLocator::Get<ResourceLoader>()[EDITOR_EXEC(GetResourcePath(relativePath, m_protected))];
			auto& assetView = EDITOR_PANEL(LitchiEditor::AssetView, "Asset View");
			assetView.SetResource(resource);
			assetView.Open();
			assetView.Focus();
		};

		FileContextualMenu::CreateList();
	}
};

class ShaderContextualMenu : public FileContextualMenu
{
public:
	ShaderContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		FileContextualMenu::CreateList();

		auto& compileAction = CreateWidget<MenuItem>("Compile");

		compileAction.ClickedEvent += [this]
		{
			DEBUG_LOG_ERROR("ShaderContextualMenu Compile No Impl");
		};

		//compileAction.ClickedEvent += [this]
		//{
		//	auto& shaderManager = OVSERVICE(ShaderManager);
		//	std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
		//	if (shaderManager.IsResourceRegistered(resourcePath))
		//	{
		//		/* Trying to recompile */
		//		Loaders::ShaderLoader::Recompile(*shaderManager[resourcePath], filePath);
		//	}
		//	else
		//	{
		//		/* Trying to compile */
		//		MaterialShader* shader = OVSERVICE(ShaderManager)[resourcePath];
		//		if (shader)
		//			DEBUG_LOG_INFO("[COMPILE] \"" + filePath + "\": Success!");
		//	}
		//	
		//};
	}
};

class ModelContextualMenu : public PreviewableContextualMenu<Mesh, ModelManager>
{
public:
	ModelContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		auto& reloadAction = CreateWidget<MenuItem>("Reload");

		reloadAction.ClickedEvent += [this]
		{
			auto& modelManager = OVSERVICE(ModelManager);
			std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
			if (modelManager.IsResourceRegistered(resourcePath))
			{
				modelManager.AResourceManager::ReloadResource(resourcePath);
			}
		};

		if (!m_protected)
		{
			auto& generateMaterialsMenu = CreateWidget<MenuList>("Generate materials...");

			//generateMaterialsMenu.CreateWidget<MenuItem>("Standard").ClickedEvent += [this]
			//{
			//	auto& modelManager = OVSERVICE(ModelManager);
			//	std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
			//	if (auto model = modelManager.GetResource(resourcePath))
			//	{
			//		for (const std::string& materialName : model->GetMaterialNames())
			//		{
			//			size_t fails = 0;
			//			std::string finalPath;

			//			do
			//			{
			//				finalPath = PathParser::GetContainingFolder(filePath) + (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".ovmat";

			//				++fails;
			//			} while (std::filesystem::exists(finalPath));

			//			{
			//				std::ofstream outfile(finalPath);
			//				outfile << "<root><shader>:Shaders/Standard.glsl</shader></root>" << std::endl; // Empty standard material content
			//			}

			//			DuplicateEvent.Invoke(finalPath);
			//		}
			//	}
			//};

			//generateMaterialsMenu.CreateWidget<MenuItem>("StandardPBR").ClickedEvent += [this]
			//{
			//	auto& modelManager = OVSERVICE(ModelManager);
			//	std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
			//	if (auto model = modelManager.GetResource(resourcePath))
			//	{
			//		for (const std::string& materialName : model->GetMaterialNames())
			//		{
			//			size_t fails = 0;
			//			std::string finalPath;

			//			do
			//			{
			//				finalPath = PathParser::GetContainingFolder(filePath) + (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".ovmat";

			//				++fails;
			//			} while (std::filesystem::exists(finalPath));

			//			{
			//				std::ofstream outfile(finalPath);
			//				outfile << "<root><shader>:Shaders/StandardPBR.glsl</shader></root>" << std::endl; // Empty standard material content
			//			}

			//			DuplicateEvent.Invoke(finalPath);
			//		}
			//	}
			//};

			//generateMaterialsMenu.CreateWidget<MenuItem>("Unlit").ClickedEvent += [this]
			//{
			//	auto& modelManager = OVSERVICE(ModelManager);
			//	std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
			//	if (auto model = modelManager.GetResource(resourcePath))
			//	{
			//		for (const std::string& materialName : model->GetMaterialNames())
			//		{
			//			size_t fails = 0;
			//			std::string finalPath;

			//			do
			//			{
			//				finalPath = PathParser::GetContainingFolder(filePath) + (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".ovmat";

			//				++fails;
			//			} while (std::filesystem::exists(finalPath));

			//			{
			//				std::ofstream outfile(finalPath);
			//				outfile << "<root><shader>:Shaders/Unlit.glsl</shader></root>" << std::endl; // Empty standard material content
			//			}

			//			DuplicateEvent.Invoke(finalPath);
			//		}
			//	}
			//};

			//generateMaterialsMenu.CreateWidget<MenuItem>("Lambert").ClickedEvent += [this]
			//{
			//	auto& modelManager = OVSERVICE(ModelManager);
			//	std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
			//	if (auto model = modelManager.GetResource(resourcePath))
			//	{
			//		for (const std::string& materialName : model->GetMaterialNames())
			//		{
			//			size_t fails = 0;
			//			std::string finalPath;

			//			do
			//			{
			//				finalPath = PathParser::GetContainingFolder(filePath) + (!fails ? materialName : materialName + " (" + std::to_string(fails) + ')') + ".ovmat";

			//				++fails;
			//			} while (std::filesystem::exists(finalPath));

			//			{
			//				std::ofstream outfile(finalPath);
			//				outfile << "<root><shader>:Shaders/Lambert.glsl</shader></root>" << std::endl; // Empty standard material content
			//			}

			//			DuplicateEvent.Invoke(finalPath);
			//		}
			//	}
			//};
		}

		PreviewableContextualMenu::CreateList();
	}
};

class TextureContextualMenu : public PreviewableContextualMenu<RHI_Texture2D, TextureManager>
{
public:
	TextureContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		auto& reloadAction = CreateWidget<MenuItem>("Reload");

		reloadAction.ClickedEvent += [this]
		{
			auto& textureManager = OVSERVICE(TextureManager);
			std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
			if (textureManager.IsResourceRegistered(resourcePath))
			{
				///* Trying to recompile */
				//textureManager.AResourceManager::ReloadResource(resourcePath);
				//EDITOR_PANEL(LitchiEditor::MaterialEditor, "Material Editor").Refresh();
			}
		};

		PreviewableContextualMenu::CreateList();
	}
};

class SceneContextualMenu : public FileContextualMenu
{
public:
	SceneContextualMenu(const std::string& p_filePath, bool p_protected = false) : FileContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		auto& editAction = CreateWidget<MenuItem>("Edit");

		editAction.ClickedEvent += [this]
		{
			std::string resourcePath = EDITOR_EXEC(GetResourcePath(filePath, m_protected));
			EDITOR_EXEC(LoadSceneFromDisk(resourcePath,true));
		};

		FileContextualMenu::CreateList();
	}
};

class MaterialContextualMenu : public PreviewableContextualMenu<Material, MaterialManager>
{
public:
	MaterialContextualMenu(const std::string& p_filePath, bool p_protected = false) : PreviewableContextualMenu(p_filePath, p_protected) {}

	virtual void CreateList() override
	{
		auto& editAction = CreateWidget<MenuItem>("Edit");

		editAction.ClickedEvent += [this]
		{
			auto relativePath = FileSystem::GetRelativePathAssetFromNative(filePath);

			Material* material = OVSERVICE(MaterialManager)[EDITOR_EXEC(GetResourcePath(relativePath, m_protected))];
			if (material)
			{
				auto& materialEditor = EDITOR_PANEL(LitchiEditor::MaterialEditor, "Material Editor");
				materialEditor.SetTarget(material);
				materialEditor.Open();
				materialEditor.Focus();

				Material* resource = LitchiRuntime::ServiceLocator::Get<MaterialManager>()[EDITOR_EXEC(GetResourcePath(relativePath, m_protected))];
				auto& assetView = EDITOR_PANEL(LitchiEditor::AssetView, "Asset View");
				assetView.SetResource(resource);
				assetView.Open();
				assetView.Focus();
			}
		};

		auto& reload = CreateWidget<MenuItem>("Reload");
		reload.ClickedEvent += [this]
		{
			auto relativePath = FileSystem::GetRelativePathAssetFromNative(filePath);

			auto& materialManager = OVSERVICE(MaterialManager);
			auto resourcePath = EDITOR_EXEC(GetResourcePath(relativePath, m_protected));
			Material* material = materialManager[resourcePath];
			if (material)
			{
				materialManager.AResourceManager::ReloadResource(resourcePath);
				EDITOR_PANEL(LitchiEditor::MaterialEditor, "Material Editor").Refresh();
			}
		};

		PreviewableContextualMenu::CreateList();
	}
};

LitchiEditor::AssetBrowser::AssetBrowser
(
	const std::string& p_title,
	bool p_opened,
	const LitchiRuntime::PanelWindowSettings& p_windowSettings,
	const std::string& p_engineAssetFolder,
	const std::string& p_projectAssetFolder,
	const std::string& p_projectScriptFolder
) :
	PanelWindow(p_title, p_opened, p_windowSettings),
	m_engineAssetFolder(p_engineAssetFolder),
	m_projectAssetFolder(p_projectAssetFolder),
	m_projectScriptFolder(p_projectScriptFolder)
{
	if (!std::filesystem::exists(m_projectAssetFolder))
	{
		std::filesystem::create_directories(m_projectAssetFolder);

		MessageBox message
		(
			"Assets folder not found",
			"The \"Assets/\" folders hasn't been found in your project directory.\nIt has been automatically generated",
			MessageBox::EMessageType::WARNING,
			MessageBox::EButtonLayout::OK
		);
	}

	if (!std::filesystem::exists(m_projectScriptFolder))
	{
		std::filesystem::create_directories(m_projectScriptFolder);

		MessageBox message
		(
			"Scripts folder not found",
			"The \"Scripts/\" folders hasn't been found in your project directory.\nIt has been automatically generated",
			MessageBox::EMessageType::WARNING,
			MessageBox::EButtonLayout::OK
		);
	}

	auto& refreshButton = CreateWidget<Button>("Rescan assets");
	refreshButton.ClickedEvent += std::bind(&AssetBrowser::Refresh, this);
	refreshButton.lineBreak = false;
	refreshButton.idleBackgroundColor = { 0.f, 0.5f, 0.0f };

	auto& importButton = CreateWidget<Button>("Import asset");
	importButton.ClickedEvent += EDITOR_BIND(ImportAsset, m_projectAssetFolder);
	importButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };

	m_assetList = &CreateWidget<Group>();

	Fill();
}

void LitchiEditor::AssetBrowser::Fill()
{
	m_assetList->CreateWidget<Separator>();
	ConsiderItem(nullptr, std::filesystem::directory_entry(m_engineAssetFolder), true);
	m_assetList->CreateWidget<Separator>();
	ConsiderItem(nullptr, std::filesystem::directory_entry(m_projectAssetFolder), false);
	m_assetList->CreateWidget<Separator>();
	ConsiderItem(nullptr, std::filesystem::directory_entry(m_projectScriptFolder), false, false, true);
}

void LitchiEditor::AssetBrowser::Clear()
{
	m_assetList->RemoveAllWidgets();
}

void LitchiEditor::AssetBrowser::Refresh()
{
	Clear();
	Fill();
}

void LitchiEditor::AssetBrowser::ParseFolder(TreeNode& p_root, const std::filesystem::directory_entry& p_directory, bool p_isEngineItem, bool p_scriptFolder)
{
	/* Iterates another time to display list files */
	for (auto& item : std::filesystem::directory_iterator(p_directory))
		if (item.is_directory())
			ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);

	/* Iterates another time to display list files */
	for (auto& item : std::filesystem::directory_iterator(p_directory))
		if (!item.is_directory())
			ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);
}

void LitchiEditor::AssetBrowser::ConsiderItem(TreeNode* p_root, const std::filesystem::directory_entry& p_entry, bool p_isEngineItem, bool p_autoOpen, bool p_scriptFolder)
{
	bool isDirectory = p_entry.is_directory();
	std::string itemname = PathParser::GetElementName(p_entry.path().string());
	std::string path = PathParser::MakeNonWindowsStyle(p_entry.path().string());
	if (isDirectory && path.back() != '/' && path.back() != '/') // Add '/' if is directory and backslash is missing
		path += '/';
	std::string resourceFormatPath = EDITOR_EXEC(GetResourcePath(path, p_isEngineItem));
	bool protectedItem = !p_root || p_isEngineItem;

	PathParser::EFileType fileType = PathParser::GetFileType(itemname);

	// Unknown file, so we skip it
	if (fileType == PathParser::EFileType::UNKNOWN && !isDirectory)
	{
		return;
	}

	/* If there is a given treenode (p_root) we attach the new widget to it */
	auto& itemGroup = p_root ? p_root->CreateWidget<Group>() : m_assetList->CreateWidget<Group>();


	// TODO:Í¼±ê
	///* Find the icon to apply to the item */
	//uint32_t iconTextureID = isDirectory ? ApplicationEditor::Instance()->editorResources->GetTexture("Icon_Folder")->id : ApplicationEditor::Instance()->editorResources->GetFileIcon(itemname)->id;

	//itemGroup.CreateWidget<Image>(iconTextureID, Vector2{ 16, 16 }).lineBreak = false;

	/* If the entry is a directory, the content must be a tree node, otherwise (= is a file), a text will suffice */
	if (isDirectory)
	{
		auto& treeNode = itemGroup.CreateWidget<TreeNode>(itemname);

		if (p_autoOpen)
			treeNode.Open();

		auto& ddSource = treeNode.AddPlugin<DDSource<std::pair<std::string, Group*>>>("Folder", resourceFormatPath, std::make_pair(resourceFormatPath, &itemGroup));

		if (!p_root || p_scriptFolder)
			treeNode.RemoveAllPlugins();

		auto& contextMenu = !p_scriptFolder ? treeNode.AddPlugin<FolderContextualMenu>(path, protectedItem && resourceFormatPath != "") : treeNode.AddPlugin<ScriptFolderContextualMenu>(path, protectedItem && resourceFormatPath != "");
		contextMenu.userData = static_cast<void*>(&treeNode);

		contextMenu.ItemAddedEvent += [this, &treeNode, path, p_isEngineItem, p_scriptFolder](std::string p_string)
		{
			treeNode.Open();
			treeNode.RemoveAllWidgets();
			ParseFolder(treeNode, std::filesystem::directory_entry(PathParser::GetContainingFolder(p_string)), p_isEngineItem, p_scriptFolder);
		};

		if (!p_scriptFolder)
		{
			if (!p_isEngineItem) /* Prevent engine item from being DDTarget (Can't Drag and drop to engine folder) */
			{
				treeNode.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("Folder").DataReceivedEvent += [this, &treeNode, path, p_isEngineItem](std::pair<std::string, Group*> p_data)
				{
					if (!p_data.first.empty())
					{
						std::string folderReceivedPath = EDITOR_EXEC(GetRealPath(p_data.first));

						std::string folderName = PathParser::GetElementName(folderReceivedPath) + '/';
						std::string prevPath = folderReceivedPath;
						std::string correctPath = m_pathUpdate.find(&treeNode) != m_pathUpdate.end() ? m_pathUpdate.at(&treeNode) : path;
						std::string newPath = correctPath + folderName;

						if (!(newPath.find(prevPath) != std::string::npos) || prevPath == newPath)
						{
							if (!std::filesystem::exists(newPath))
							{
								bool isEngineFolder = p_data.first.at(0) == ':';

								if (isEngineFolder) /* Copy dd folder from Engine resources */
									std::filesystem::copy(prevPath, newPath, std::filesystem::copy_options::recursive);
								else
								{
									RenameAsset(prevPath, newPath);
									EDITOR_EXEC(PropagateFolderRename(prevPath, newPath));
								}

								treeNode.Open();
								treeNode.RemoveAllWidgets();
								ParseFolder(treeNode, std::filesystem::directory_entry(correctPath), p_isEngineItem);

								if (!isEngineFolder)
									p_data.second->Destroy();
							}
							else if (prevPath == newPath)
							{
								// Ignore
							}
							else
							{
								MessageBox errorMessage("Folder already exists", "You can't move this folder to this location because the name is already taken", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
							}
						}
						else
						{
							MessageBox errorMessage("Wow!", "Crazy boy!", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
						}
					}
				};

				treeNode.AddPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [this, &treeNode, path, p_isEngineItem](std::pair<std::string, Group*> p_data)
				{
					if (!p_data.first.empty())
					{
						std::string fileReceivedPath = EDITOR_EXEC(GetRealPath(p_data.first));

						std::string fileName = PathParser::GetElementName(fileReceivedPath);
						std::string prevPath = fileReceivedPath;
						std::string correctPath = m_pathUpdate.find(&treeNode) != m_pathUpdate.end() ? m_pathUpdate.at(&treeNode) : path;
						std::string newPath = correctPath + fileName;

						if (!std::filesystem::exists(newPath))
						{
							bool isEngineFile = p_data.first.at(0) == ':';

							if (isEngineFile) /* Copy dd file from Engine resources */
								std::filesystem::copy_file(prevPath, newPath);
							else
							{
								RenameAsset(prevPath, newPath);
								EDITOR_EXEC(PropagateFileRename(prevPath, newPath));
							}

							treeNode.Open();
							treeNode.RemoveAllWidgets();
							ParseFolder(treeNode, std::filesystem::directory_entry(correctPath), p_isEngineItem);

							if (!isEngineFile)
								p_data.second->Destroy();
						}
						else if (prevPath == newPath)
						{
							// Ignore
						}
						else
						{
							MessageBox errorMessage("File already exists", "You can't move this file to this location because the name is already taken", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
						}
					}
				};

				treeNode.AddPlugin<DDTarget<std::pair<GameObject*, TreeNode*>>>("Actor").DataReceivedEvent += [this, &treeNode, resourceFormatPath, p_isEngineItem](std::pair<GameObject*, TreeNode*> p_data)
				{
					auto rootGameObject = p_data.first;
					auto scene = rootGameObject->GetScene();

					EDITOR_EXEC(CreatePrefab(scene, rootGameObject, resourceFormatPath + rootGameObject->GetName() + ".prefab"));
				};

			}

			contextMenu.DestroyedEvent += [&itemGroup](std::string p_deletedPath) { itemGroup.Destroy(); };

			contextMenu.RenamedEvent += [this, &treeNode, path, &ddSource, p_isEngineItem](std::string p_prev, std::string p_newPath)
			{
				p_newPath += '/';

				if (!std::filesystem::exists(p_newPath)) // Do not rename a folder if it already exists
				{
					RenameAsset(p_prev, p_newPath);
					EDITOR_EXEC(PropagateFolderRename(p_prev, p_newPath));
					std::string elementName = PathParser::GetElementName(p_newPath);
					std::string data = PathParser::GetContainingFolder(ddSource.data.first) + elementName + "/";
					ddSource.data.first = data;
					ddSource.tooltip = data;
					treeNode.name = elementName;
					treeNode.Open();
					treeNode.RemoveAllWidgets();
					ParseFolder(treeNode, std::filesystem::directory_entry(p_newPath), p_isEngineItem);
					m_pathUpdate[&treeNode] = p_newPath;
				}
				else
				{
					MessageBox errorMessage("Folder already exists", "You can't rename this folder because the given name is already taken", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
				}
			};

			contextMenu.ItemAddedEvent += [this, &treeNode, p_isEngineItem](std::string p_path)
			{
				treeNode.RemoveAllWidgets();
				ParseFolder(treeNode, std::filesystem::directory_entry(PathParser::GetContainingFolder(p_path)), p_isEngineItem);
			};

		}

		contextMenu.CreateList();

		treeNode.OpenedEvent += [this, &treeNode, path, p_isEngineItem, p_scriptFolder]
		{
			treeNode.RemoveAllWidgets();
			std::string updatedPath = PathParser::GetContainingFolder(path) + treeNode.name;
			ParseFolder(treeNode, std::filesystem::directory_entry(updatedPath), p_isEngineItem, p_scriptFolder);
		};

		treeNode.ClosedEvent += [this, &treeNode]
		{
			treeNode.RemoveAllWidgets();
		};
	}
	else
	{
		auto& clickableText = itemGroup.CreateWidget<TextClickable>(itemname);

		FileContextualMenu* contextMenu = nullptr;

		switch (fileType)
		{
		case PathParser::EFileType::MODEL:		contextMenu = &clickableText.AddPlugin<ModelContextualMenu>(path, protectedItem);		break;
		case PathParser::EFileType::TEXTURE:	contextMenu = &clickableText.AddPlugin<TextureContextualMenu>(path, protectedItem); 	break; // todo: 
		case PathParser::EFileType::SHADER:		contextMenu = &clickableText.AddPlugin<ShaderContextualMenu>(path, protectedItem);		break;
		case PathParser::EFileType::MATERIAL:	contextMenu = &clickableText.AddPlugin<MaterialContextualMenu>(path, protectedItem);	break;
		case PathParser::EFileType::SCENE:		contextMenu = &clickableText.AddPlugin<SceneContextualMenu>(path, protectedItem);		break;
		default: contextMenu = &clickableText.AddPlugin<FileContextualMenu>(path, protectedItem); break;
        // todo: prefab
		}

		contextMenu->CreateList();

		contextMenu->DestroyedEvent += [&itemGroup](std::string p_deletedPath)
		{
			itemGroup.Destroy();

			//if (EDITOR_CONTEXT(sceneManager).GetCurrentSceneSourcePath() == p_deletedPath) // Modify current scene source path if the renamed file is the current scene
			//	EDITOR_CONTEXT(sceneManager).ForgetCurrentSceneSourcePath();
		};

		auto& ddSource = clickableText.AddPlugin<DDSource<std::pair<std::string, Group*>>>
			(
				"File",
				resourceFormatPath,
				std::make_pair(resourceFormatPath, &itemGroup)
			);

		contextMenu->RenamedEvent += [&ddSource, &clickableText, p_scriptFolder](std::string p_prev, std::string p_newPath)
		{
			if (p_newPath != p_prev)
			{
				if (!std::filesystem::exists(p_newPath))
				{
					RenameAsset(p_prev, p_newPath);
					std::string elementName = PathParser::GetElementName(p_newPath);
					ddSource.data.first = PathParser::GetContainingFolder(ddSource.data.first) + elementName;

					if (!p_scriptFolder)
					{
						EDITOR_EXEC(PropagateFileRename(p_prev, p_newPath));
						//if (EDITOR_CONTEXT(sceneManager).GetCurrentSceneSourcePath() == p_prev) // Modify current scene source path if the renamed file is the current scene
						//	EDITOR_CONTEXT(sceneManager).StoreCurrentSceneSourcePath(p_newPath);
					}
					else
					{
						EDITOR_EXEC(PropagateScriptRename(p_prev, p_newPath));
					}

					clickableText.content = elementName;
				}
				else
				{
					MessageBox errorMessage("File already exists", "You can't rename this file because the given name is already taken", MessageBox::EMessageType::ERROR, MessageBox::EButtonLayout::OK);
				}
			}
		};

		contextMenu->DuplicateEvent += [this, &clickableText, p_root, path, p_isEngineItem](std::string newItem)
		{
			EDITOR_EXEC(DelayAction(std::bind(&AssetBrowser::ConsiderItem, this, p_root, std::filesystem::directory_entry{ newItem }, p_isEngineItem, false, false), 0));
		};

		if (fileType == PathParser::EFileType::TEXTURE)
		{
			auto& texturePreview = clickableText.AddPlugin<TexturePreview>();
			texturePreview.SetPath(resourceFormatPath);
		}

		/*if(fileType == PathParser::EFileType::PREFAB)
		{
			auto prefab = ApplicationBase::Instance()->prefabManager->LoadResource(resourceFormatPath);
			prefab->PostResourceLoaded();
			clickableText.AddPlugin<DDSource<Prefab*>>("LoadPrefab", resourceFormatPath,prefab);
		}

		if(fileType == PathParser::EFileType::MODEL)
		{
			auto model = ApplicationBase::Instance()->modelManager->LoadResource(resourceFormatPath);
			auto prefab = model->GetModelPrefab();
			clickableText.AddPlugin<DDSource<Prefab*>>("LoadPrefab", resourceFormatPath, prefab);
		}*/
	}
}
