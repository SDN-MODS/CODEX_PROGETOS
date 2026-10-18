/*
 * Apply this minimal source patch to:
 * RaG_BaseBuilding/scripts/4_World/rag_basebuilding/actions/vanilla/actiondismantlepart.c
 *
 * DayZ 1.30 introduced/uses DigitalCodeLock in the vanilla action hierarchy.
 * Code Lock's helper returns CodeLock. They are unrelated types. Rename the
 * Code Lock helper at the call site; do not cast either type to the other.
 */

// BEFORE (the conflicting call in the RaG file):
// CodeLock codeLock = GetCodeLock();

// AFTER:
CodeLock codeLock = GetRaGCodeLock();

// If the helper is declared in the same RaG class, rename its declaration too:
// CodeLock GetCodeLock()           -> CodeLock GetRaGCodeLock()
// and update every RaG-only call in this action file.
