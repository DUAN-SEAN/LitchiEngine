#pragma once

#include <filesystem>
#include "Buffer.h"

namespace LitchiRuntime {

	class FileSystem
	{
	public:
		// TODO: move to FileSystem class
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	public:
        // Strings
        static bool IsEmptyOrWhitespace(const std::string& var);
        static bool IsAlphanumeric(const std::string& var);
        static std::string RemoveIllegalCharacters(const std::string& text);
        static std::string GetStringBeforeExpression(const std::string& str, const std::string& exp);
        static std::string GetStringAfterExpression(const std::string& str, const std::string& exp);
        static std::string GetStringBetweenExpressions(const std::string& str, const std::string& exp_a, const std::string& exp_b);
        static std::string ConvertToUppercase(const std::string& lower);
        static std::string ReplaceExpression(const std::string& str, const std::string& from, const std::string& to);
        static std::wstring StringToWstring(const std::string& str);

        // Supported files
        static bool IsSupportedAudioFile(const std::string& path);
        static bool IsSupportedImageFile(const std::string& path);
        static bool IsSupportedModelFile(const std::string& path);
        static bool IsSupportedShaderFile(const std::string& path);
        static bool IsSupportedFontFile(const std::string& path);
        static bool IsEnginePrefabFile(const std::string& path);
        static bool IsEngineMaterialFile(const std::string& path);
        static bool IsEngineMeshFile(const std::string& path);
        static bool IsEngineModelFile(const std::string& path);
        static bool IsEngineSceneFile(const std::string& path);
        static bool IsEngineTextureFile(const std::string& path);
        static bool IsEngineAudioFile(const std::string& path);
        static bool IsEngineShaderFile(const std::string& path);
        static bool IsEngineFile(const std::string& path);

        static void SetProjectAssetDirectoryPath(const std::string& workDir);
        static std::string  GetProjectAssetDirectoryPath();

        // Supported files in directory
        static std::vector<std::string> GetSupportedFilesInDirectory(const std::string& path);
        static std::vector<std::string> GetSupportedImageFilesFromPaths(const std::vector<std::string>& paths);
        static std::vector<std::string> GetSupportedAudioFilesFromPaths(const std::vector<std::string>& paths);
        static std::vector<std::string> GetSupportedModelFilesFromPaths(const std::vector<std::string>& paths);
        static std::vector<std::string> GetSupportedModelFilesInDirectory(const std::string& path);
        static std::vector<std::string> GetSupportedSceneFilesInDirectory(const std::string& path);

        // Directories & files
        static std::string GetFileNameFromFilePath(const std::string& path);
        static std::string GetFileNameWithoutExtensionFromFilePath(const std::string& path);
        static std::string GetDirectoryFromFilePath(const std::string& path);
        static std::string GetFilePathWithoutExtension(const std::string& path);
        static std::string ReplaceExtension(const std::string& path, const std::string& extension);
        static std::string GetExtensionFromFilePath(const std::string& path);
        static std::string NativizeFilePath(const std::string& path);
        static std::string GetRelativePath(const std::string& path);
        static std::string GetRelativePathAssetFromNative(const std::string& path);
        static std::string GetWorkingDirectory();
        static std::string GetRootDirectory(const std::string& path);
        static std::string GetParentDirectory(const std::string& path);
        static std::vector<std::string> GetDirectoriesInDirectory(const std::string& path);
        static std::vector<std::string> GetFilesInDirectory(const std::string& path);
        static bool Exists(const std::string& path);
        static bool IsDirectory(const std::string& path);
        static bool IsFile(const std::string& path);
        static void OpenUrl(const std::string& url);
        static bool Delete(const std::string& path);
        static bool CreateDirectory(const std::string& path);
        static bool CopyFileFromTo(const std::string& source, const std::string& destination);

        static std::string ProjectAssetDirectoryPath;
	};
    

    static const char* EXTENSION_WORLD = ".world";
    static const char* EXTENSION_MATERIAL = ".material";
    static const char* EXTENSION_MODEL = ".model";
    static const char* EXTENSION_PREFAB = ".prefab";
    static const char* EXTENSION_SHADER = ".shader";
    static const char* EXTENSION_FONT = ".font";
    static const char* EXTENSION_TEXTURE = ".texture";
    static const char* EXTENSION_MESH = ".mesh";
    static const char* EXTENSION_AUDIO = ".audio";

    static const std::vector<std::string> supported_formats_image
    {
            ".jpg",
            ".png",
            ".bmp",
            ".tga",
            ".dds",
            ".exr",
            ".raw",
            ".gif",
            ".hdr",
            ".ico",
            ".iff",
            ".jng",
            ".jpeg",
            ".koala",
            ".kodak",
            ".mng",
            ".pcx",
            ".pbm",
            ".pgm",
            ".ppm",
            ".pfm",
            ".pict",
            ".psd",
            ".raw",
            ".sgi",
            ".targa",
            ".tiff",
            ".tif", // tiff can also be tif
            ".wbmp",
            ".webp",
            ".xbm",
            ".xpm"
    };

    static const std::vector<std::string> supported_formats_audio
    {
        ".aiff",
        ".asf",
        ".asx",
        ".dls",
        ".flac",
        ".fsb",
        ".it",
        ".m3u",
        ".midi",
        ".mod",
        ".mp2",
        ".mp3",
        ".ogg",
        ".pls",
        ".s3m",
        ".vag", // PS2/PSP
        ".wav",
        ".wax",
        ".wma",
        ".xm",
        ".xma" // XBOX 360
    };

    static const std::vector<std::string> supported_formats_model
    {
        ".3ds",
        ".obj",
        ".fbx",
        ".blend",
        ".dae",
        ".gltf",
        ".lwo",
        ".c4d",
        ".ase",
        ".dxf",
        ".hmp",
        ".md2",
        ".md3",
        ".md5",
        ".mdc",
        ".mdl",
        ".nff",
        ".ply",
        ".stl",
        ".x",
        ".smd",
        ".lxo",
        ".lws",
        ".ter",
        ".ac3d",
        ".ms3d",
        ".cob",
        ".q3bsp",
        ".xgl",
        ".csm",
        ".bvh",
        ".b3d",
        ".ndo"
    };

    static const std::vector<std::string> supported_formats_shader
    {
        ".hlsl"
    };

    static const std::vector<std::string> supported_formats_font
    {
        ".ttf",
        ".ttc",
        ".cff",
        ".woff",
        ".otf",
        ".otc",
        ".pfa",
        ".pfb",
        ".fnt",
        ".bdf",
        ".pfr"
    };
}
