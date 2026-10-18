/*
 * Source fix for:
 * RaG_BaseBuilding/Scripts/4_World/RaG_BaseBuilding/Actions/Vanilla/ActionDismantlePart.c
 *
 * DayZ 1.30's Fence.GetCodeLock() returns DigitalCodeLock. The Code Lock
 * mod also used the same method name for its unrelated CodeLock entity. A
 * CodeLock cannot be assigned from Fence.GetCodeLock(). Resolve the Code
 * Lock attachment explicitly instead of calling the ambiguous accessor.
 */

#ifdef CodeLock
if (compName == "simple_door" || compName == "simple_door_metal")
{
    Fence fence = Fence.Cast(action_data.m_Target.GetObject());
    CodeLock codelock = null;

    if (fence)
        codelock = CodeLock.Cast(fence.FindAttachmentBySlotName("Att_CodeLock"));

    if (codelock)
        codelock.UnlockServer(action_data.m_Player, fence);
}
#endif
