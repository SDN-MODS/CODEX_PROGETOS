class CfgPatches
{
    class SDN_RaG_CodeLoke_Fix_1_30
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 1.30;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgMods
{
    class SDN_RaG_CodeLoke_Fix_1_30
    {
        dir = "SDN_RaG_CodeLoke_Fix_1.30";
        name = "SDN RaG CodeLock Fix 1.30";
        picture = "";
        action = "";
        type = "mod";
        dependencies[] = {"World"};
        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"SDN_RaG_CodeLoke_Fix_1.30/scripts/4_World"};
            };
        };
    };
};
