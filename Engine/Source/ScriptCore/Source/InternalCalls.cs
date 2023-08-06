using System;
using System.Runtime.CompilerServices;

// ReSharper disable once CheckNamespace
namespace LitchiEngine
{
    /// <summary>
    /// 脚本对象基类
    /// </summary>
    public abstract class ScriptObject
    {
        /// <summary>
        /// 设置非托管对象的Id
        /// </summary>
        /// <param name="unmanagedId"></param>
        internal void SetUnmanagedIdFromEngine(ulong unmanagedId)
        {
            m_umanagedId = unmanagedId;
        }

        /// <summary>
        /// 非托管层定义的id
        /// </summary>
        protected internal ulong UnmanagedId => m_umanagedId;
        private ulong m_umanagedId;
    }

    #region Scene & GameObject
    
    /// <summary>
    /// 场景管理器
    /// </summary>
    public static class SceneManager
    {
        /// <summary>
        /// 从引擎加载的场景
        /// </summary>
        /// <param name="scene"></param>
        public static void LoadSceneFromEngine(Scene scene)
        {
            if (scene != null)
            {
                Console.WriteLine("LoadSceneFromEngine Complete!");
            }
            else
            {

                Console.WriteLine("LoadSceneFromEngine Fail!");
            }
            Scene.OnLoadScene(scene);

            m_currScene = scene;

        }


        public static Scene m_currScene;
    }

    /// <summary>
    /// 场景实例
    /// </summary>
    public class Scene : ScriptObject
    {
        public GameObject FindGameObject(string name)
        {
            return null;
        }

        public static void OnLoadScene(Scene scene)
        {
            // 从C++层加载场景

            // 缓存GO?

        }
    }

    /// <summary>
    /// 游戏对象
    /// </summary>
    public class GameObject : ScriptObject
    {
        /// <summary>
        /// 添加组件
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public T AddComponent<T>() where T : Component
        {
            return GetOrCreateComponent<T>();
        }

        /// <summary>
        /// 获取组件
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public T GetComponent<T>() where T : Component
        {
            return GetOrCreateComponent<T>();
        }

        /// <summary>
        /// 获取或则创建Component
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        private T GetOrCreateComponent<T>() where T : Component
        {
            var componentType = typeof(T);

            // 内部调用到C++层创建对象
            if (componentType.BaseType == typeof(Component))
            {
                return InternalCalls.Internal_GetOrCreateComponent(Scene.UnmanagedId, UnmanagedId, componentType.Name) as T;
            }
            else if (typeof(T).BaseType == typeof(ScriptComponent))
            {
                return InternalCalls.Internal_GetOrCreateScriptComponent(Scene.UnmanagedId, UnmanagedId, $"{componentType.Namespace}.{componentType.Name}") as T;
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// 设置Scene非托管对象的Id
        /// </summary>
        /// <param name="unmanagedId"></param>
        internal void SetSceneUnmanagedIdFromEngine(ulong unmanagedId)
        {
            m_sceneUnmanageId = unmanagedId;
        }

        /// <summary>
        /// GameObject的名字
        /// </summary>
        public string Name => InternalCalls.Internal_GetGameObjectName(UnmanagedId);

        /// <summary>
        /// 当前场景
        /// </summary>
        public Scene Scene => InternalCalls.Internal_GetScriptInstance(m_sceneUnmanageId) as Scene;
        private ulong m_sceneUnmanageId;

    }

    #endregion

    #region Component

    /// <summary>
    /// 脚本基类
    /// </summary>
    public abstract class Component : ScriptObject
    {
        #region 生命周期函数

        /// <summary>
        /// 脚本实例化时
        /// </summary>
        protected abstract void OnAwake();

        /// <summary>
        /// 脚本启动时
        /// </summary>
        protected abstract void OnStart();

        /// <summary>
        /// 脚本刷新
        /// </summary>
        protected abstract void OnUpdate();

        #endregion

        /// <summary>
        /// 设置GameObject非托管对象的Id
        /// </summary>
        /// <param name="unmanagedId"></param>
        internal void SetGameObjectUnmanagedIdFromEngine(ulong unmanagedId)
        {
            m_gameObjectUnmanageId = unmanagedId;
        }

        /// <summary>
        /// 获取当前游戏对象
        /// </summary>
        public GameObject Scene => InternalCalls.Internal_GetScriptInstance(m_gameObjectUnmanageId) as GameObject;
        private ulong m_gameObjectUnmanageId;
    }

    /// <summary>
    /// 脚本组件基类
    /// </summary>
    public abstract class ScriptComponent : Component
    {

    }

    /// <summary>
    /// Transform组件的托管对象
    /// </summary>
    public class Transform : Component
    {
        protected sealed override void OnAwake()
        {
        }

        protected sealed override void OnStart()
        {
        }

        protected sealed override void OnUpdate()
        {
        }
    }

    /// <summary>
    /// 测试脚本组件
    /// </summary>
    public class TestScriptComponent : ScriptComponent
    {
        protected override void OnAwake()
        {
            Console.WriteLine("TestScriptComponent::OnAwake");
        }

        protected override void OnStart()
        {
            Console.WriteLine("TestScriptComponent::OnStart");
        }

        protected override void OnUpdate()
        {
            Console.WriteLine("TestScriptComponent::OnUpdate");
        }
    }

    #endregion

    /// <summary>
    /// 内部调用
    /// </summary>
    internal static class InternalCalls
    {
        #region Common

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ScriptObject Internal_GetScriptInstance(ulong scriptObjectUnmanagedId);

        #endregion

        #region Scene

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Scene Internal_GetCurrentScene();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Scene Internal_CreateEmptyScene();

        #endregion

        #region GameObject

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static GameObject Internal_GetOrCreateGameObject(ulong sceneUnmanagedId, string gameObjectName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string Internal_GetGameObjectName(ulong gameObjectUnmanagedId);

        #endregion

        #region Component

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Component Internal_GetOrCreateComponent(ulong sceneUnmanagedId, ulong gameObjectUnmanagedId, string componentName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ScriptComponent Internal_GetOrCreateScriptComponent(ulong sceneUnmanagedId, ulong gameObjectUnmanagedId, string className);

        #endregion
    }
}
