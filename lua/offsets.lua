-- sources: Y2JB kernel_offset.js, ps5-payload-dev SDK

fw_offsets = {
    ["4.00"] = { DATA_BASE=0x0C00000, ALLPROC=0x027EDCB8, SECURITY_FLAGS=0x06506474, ROOTVNODE=0x066E74C0, KERNEL_PMAP_STORE=0x03257A78, GVMSPACE=0x064C3F80 },
    ["4.02"] = "4.00", ["4.03"] = "4.00", ["4.50"] = "4.00", ["4.51"] = "4.00",

    ["5.00"] = { DATA_BASE=0x0C40000, ALLPROC=0x0291DD00, SECURITY_FLAGS=0x066466EC, ROOTVNODE=0x06853510, KERNEL_PMAP_STORE=0x03398A88, GVMSPACE=0x06603FB0 },
    ["5.02"] = "5.00", ["5.10"] = "5.00",

    ["5.50"] = { DATA_BASE=0x0C40000, ALLPROC=0x0291DD00, SECURITY_FLAGS=0x066466EC, ROOTVNODE=0x06853510, KERNEL_PMAP_STORE=0x03394A88, GVMSPACE=0x06603FB0 },

    ["6.00"] = { DATA_BASE=0x0C60000, ALLPROC=0x02869D20, SECURITY_FLAGS=0x065968EC, ROOTVNODE=0x0679F510, KERNEL_PMAP_STORE=0x032E4358, GVMSPACE=0x065540F0 },
    ["6.02"] = "6.00", ["6.50"] = "6.00",

    ["7.00"] = { DATA_BASE=0x0C50000, ALLPROC=0x02859D50, SECURITY_FLAGS=0x00AC8064, ROOTVNODE=0x030C7510, KERNEL_PMAP_STORE=0x02E2C848, GVMSPACE=0x02E76090 },
    ["7.01"] = "7.00", ["7.20"] = "7.00", ["7.40"] = "7.00", ["7.60"] = "7.00", ["7.61"] = "7.00",

    ["8.00"] = { DATA_BASE=0x0C70000, ALLPROC=0x02875D50, SECURITY_FLAGS=0x00AC3064, ROOTVNODE=0x030FB510, KERNEL_PMAP_STORE=0x02E48848, GVMSPACE=0x02EAA090 },
    ["8.20"] = "8.00", ["8.40"] = "8.00", ["8.60"] = "8.00",

    ["9.00"] = { DATA_BASE=0x0CA0000, ALLPROC=0x02755D50, SECURITY_FLAGS=0x00D72064, ROOTVNODE=0x02FDB510, KERNEL_PMAP_STORE=0x02D28B78, GVMSPACE=0x02D8A570 },

    ["9.05"] = { DATA_BASE=0x0CA0000, ALLPROC=0x02755D50, SECURITY_FLAGS=0x00D73064, ROOTVNODE=0x02FDB510, KERNEL_PMAP_STORE=0x02D28B78, GVMSPACE=0x02D8A570 },
    ["9.20"] = "9.05", ["9.40"] = "9.05", ["9.60"] = "9.05",

    ["10.00"] = { DATA_BASE=0x0CC0000, ALLPROC=0x02765D70, SECURITY_FLAGS=0x00D79064, ROOTVNODE=0x02FA3510, KERNEL_PMAP_STORE=0x02CF0EF8, GVMSPACE=0x02D52570 },
    ["10.01"] = "10.00", ["10.20"] = "10.00", ["10.40"] = "10.00", ["10.60"] = "10.00",

    ["11.00"] = { DATA_BASE=0x0D30000, ALLPROC=0x02875D70, SECURITY_FLAGS=0x00D8C064, ROOTVNODE=0x030B7510, KERNEL_PMAP_STORE=0x02E04F18, GVMSPACE=0x02E66570 },
    ["11.20"] = "11.00", ["11.40"] = "11.00", ["11.60"] = "11.00",
 
    ["12.00"] = { DATA_BASE=0x0D50000, ALLPROC=0x02885E00, SECURITY_FLAGS=0x00D83064, ROOTVNODE=0x030D7510, KERNEL_PMAP_STORE=0x02E1CFB8, GVMSPACE=0x02E7E570},
}

local struct_offsets = {
    -- proc
    PROC_PID        = 0xBC,
    PROC_UCRED      = 0x40,
    PROC_FD         = 0x48,
    PROC_VM_SPACE   = 0x200,

    -- ucred
    UCRED_CR_UID    = 0x04,
    UCRED_CR_RUID   = 0x08,
    UCRED_CR_SVUID  = 0x0C,
    UCRED_CR_NGROUPS= 0x10,
    UCRED_CR_RGID   = 0x14,
    UCRED_CR_PRISON = 0x30,
    UCRED_CR_SCEAUTHID = 0x58,
    UCRED_CR_SCECAPS0  = 0x60,
    UCRED_CR_SCECAPS1  = 0x68,
    UCRED_CR_SCEATTRS  = 0x83,

    -- filedesc
    FILEDESC_OFILES = 0x00,
    FDESCENTTBL_HDR = 8,
    FILEDESCENT_SIZE= 0x30,

    -- fd
    FD_RDIR         = 0x10,
    FD_JDIR         = 0x18,

    KQ_FDP          = 0xA8,
    KL_LOCK         = 0x68,

    -- net
    INPCB_PKTOPTS   = 0x120,
    IP6PO_RTHDR     = 0x70,

    -- pipe sigio
    PIPE_SIGIO      = 0xD8,

    -- pmap
    PMAP_CR3        = 0x28,
    PMAP_PML4       = 0x20,

    -- gpu vmspace
    SIZEOF_GVMSPACE     = 0x100,
    GVMSPACE_START_VA   = 0x08,
    GVMSPACE_SIZE       = 0x10,
    GVMSPACE_PAGE_DIR   = 0x38,

    TARGET_ID_REL   = 0x09,
    QA_FLAGS_REL    = 0x24,
    UTOKEN_REL      = 0x8C,
}

function get_offsets(fw_version)
    local major, minor = fw_version:match("(%d+)%.(%d+)")
    if not major then return nil end
    local key = tostring(tonumber(major)) .. "." .. minor
    local entry = fw_offsets[key]
    for i = 1, 5 do
        if type(entry) == "string" then entry = fw_offsets[entry]
        else break end
    end

    if type(entry) ~= "table" then return nil end
    local OFF = {}
    for k, v in pairs(entry) do OFF[k] = v end
    for k, v in pairs(struct_offsets) do OFF[k] = v end
    return OFF
end