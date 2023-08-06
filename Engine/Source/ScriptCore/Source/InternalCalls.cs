using System;
using System.Runtime.CompilerServices;

// ReSharper disable once CheckNamespace
namespace LitchiEngine
{
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

    public class ScriptObject
    {
        /// <summary>
        /// 非托管层定义的id
        /// </summary>
        protected UInt64 m_umanagedId;
    }

    public class GameObject : ScriptObject
    {
        public string Name
        {
            get
            {
                // 通过InternalCall获取
                return "";
            }
        }

        public T AddComponent<T>() where T : ScriptComponent, new()
        {
            // 内部调用到C++层创建对象
            if (typeof(T).BaseType == typeof(Component))
            {

            }
            else
            {

            }

            return new T();
        }


        #region Internal Calls

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ScriptObject Internal_Create1(Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ScriptObject Internal_Create2(string typeName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_ManagedInstanceCreated(ScriptObject managedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_ManagedInstanceDeleted(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_Destroy(IntPtr obj, float timeLeft);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Internal_GetTypeName(IntPtr obj);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ScriptObject Internal_FindObject(ref Guid id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ScriptObject Internal_TryFindObject(ref Guid id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_ChangeID(IntPtr obj, ref Guid id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern IntPtr Internal_GetUnmanagedInterface(IntPtr obj, Type type);

        #endregion
    }

    #region 组件

    /// <summary>
    /// 脚本基类
    /// </summary>
    public abstract class Component : ScriptObject
    {
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
    }

    public class Transform : Component
    {
        protected override void OnAwake()
        {
            throw new NotImplementedException();
        }

        protected override void OnStart()
        {
            throw new NotImplementedException();
        }

        protected override void OnUpdate()
        {
            throw new NotImplementedException();
        }
    }

    /// <summary>
    /// 脚本组件基类
    /// </summary>
    public abstract class ScriptComponent : Component
    {

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

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_FindEntityByName(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object GetScriptInstance(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_GetLinearVelocity(ulong entityID, out Vector2 linearVelocity);
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static Rigidbody2DComponent.BodyType Rigidbody2DComponent_GetType(ulong entityID);
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static void Rigidbody2DComponent_SetType(ulong entityID, Rigidbody2DComponent.BodyType type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);
    }
}
