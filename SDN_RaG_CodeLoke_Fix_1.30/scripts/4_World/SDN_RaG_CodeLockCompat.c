/**
 * SDN RaG CodeLock Fix 1.30
 *
 * Compatibility namespace for RaG BaseBuilding. DayZ 1.30 exposes the
 * vanilla DigitalCodeLock type; the Code Lock mod exposes CodeLock. They
 * are unrelated classes and must never share a variable or method name.
 *
 * The RaG source patch in patches/actiondismantlepart.c renames its helper
 * call to GetRaGCodeLock(). This file intentionally contains no replacement
 * DigitalCodeLock implementation: the game accessor remains untouched.
 */
class SDN_RaG_CodeLockCompat
{
    static bool IsCompatible()
    {
        return true;
    }
};
