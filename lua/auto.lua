run_lua_file("/savedata0/lua/setlogserver.lua")
if PLATFORM == "PS5" then
--[[  if tonumber(FW_VERSION) <= 7.61 then
    run_lua_file("/savedata0/lua/umtx.lua")
  elseif tonumber(FW_VERSION) <= 10.01 then
    run_lua_file("/savedata0/lua/lapse.lua")
  elseif tonumber(FW_VERSION) <= 12.00 then
    run_lua_file("/savedata0/lua/poops.lua") ]]--
  if tonumber(FW_VERSION) >= 4.00 and tonumber(FW_VERSION) <= 12.00 then
    run_lua_file("/savedata0/lua/poops_ps5.lua")
    run_lua_file("/mnt/sandbox/" .. get_title_id() .. "_000/savedata0/lua/autoload.lua")
  else
    error("Not supported firmware: " .. tostring(FW_VERSION))
  end
elseif PLATFORM == "PS4" then
--[[  if tonumber(FW_VERSION) >= 9.00 and tonumber(FW_VERSION) <= 12.02 then
    run_lua_file("lapse.lua")
  elseif tonumber(FW_VERSION) <= 13.00 then
    run_lua_buffer("poops.lua")
  else
    error("Unknown firmware: " .. tostring(FW_VERSION))
  end ]]--
  error("Not supported platform: " .. PLATFORM)
else
  error("Unknown platform: " .. tostring(PLATFORM))
end