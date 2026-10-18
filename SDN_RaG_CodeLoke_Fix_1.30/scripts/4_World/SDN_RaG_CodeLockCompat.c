/**
 * Independent compatibility API for servers that load RaG BaseBuilding and
 * Code Lock together on DayZ 1.30.
 *
 * This does not override Fence.GetCodeLock(): in 1.30 that vanilla accessor
 * returns DigitalCodeLock. CodeLock is retrieved from its attachment slot
 * through a separate name, so no unrelated types are mixed.
 */
modded class Fence
{
    CodeLock SDN_GetCodeLockAttachment()
    {
        return CodeLock.Cast(FindAttachmentBySlotName("Att_CodeLock"));
    }
};
