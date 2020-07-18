

-- according to this http://www.lua.org/manual/5.3/manual.html#pdf-package.config
-- the most reliable way to detect if we are on windows, or other OS
function is_windows()
	return package.config:sub(1,1) == '\\'
end


-- Returns true if `file` exists
function exists(file)
   local ok, err, code = os.rename(file, file)
   if not ok then
      if code == 13 then
         -- Permission denied, but it exists
         return true
      end
   end
   return ok, err
end

--- Check if a directory exists in this path
function isdir(path)
   return exists(path.."/")
end

-- Changes first letter of string to uppercase
-- e.g. "test" -> "Test"
function title_case(s)
	return s:sub(1,1):upper()..s:sub(2)
end

-- premake5 cli options are dumped to a global _OPTIONS table

-- this function returns the option as a string or a default value
function get_option_str(name, default)
	local opt = _OPTIONS[name]
	if opt ~= nil then 
		return tostring(opt)
	else 
		return default 
	end
end

-- this function returns true if _OPTIONS[name] exists
function has_option(name, default)
	local opt = _OPTIONS[name]
	return opt ~= nil or default
end

function rmdir(file)
	if not isdir(file) then
		return
	end
	print ("remove " .. file)
	if is_windows() then
		os.execute("rmdir \"" .. file .. "\" /s /q")
	else
		os.execute("rm -rf \"" .. file .. "\"")
	end
end

function rm(file)
	if not exists(file) then
		return
	end
	print ("remove " .. file)
	if is_windows() then
		os.execute("del \"" .. file .. "\"")
	else
		os.execute("rm \"" .. file .. "\"")
	end
end