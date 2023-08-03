﻿using System;
using System.Runtime.CompilerServices;

namespace LitchiEngine
{
    public class Object
    {
        /// <summary>
		/// 非托管层定义的id
		/// </summary>
        protected UInt64 m_umanagedId;
    }

    public class GameObject : Object
    {
        public string Name
        {
            get
            {
                // 通过InternalCall获取
                return "";
            }
        }

        public T AddComponent<T>() where T:ScriptComponent ,new()
        {
            // 内部调用到C++层创建对象

            return new T();
        }


        #region Internal Calls

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Object Internal_Create1(Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Object Internal_Create2(string typeName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_ManagedInstanceCreated(Object managedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_ManagedInstanceDeleted(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_Destroy(IntPtr obj, float timeLeft);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Internal_GetTypeName(IntPtr obj);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Object Internal_FindObject(ref Guid id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Object Internal_TryFindObject(ref Guid id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_ChangeID(IntPtr obj, ref Guid id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern IntPtr Internal_GetUnmanagedInterface(IntPtr obj, Type type);

        #endregion
    }

    /// <summary>
    /// 脚本组件
    /// </summary>
    public abstract class ScriptComponent : Object
    {

    }

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
