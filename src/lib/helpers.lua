
function nebload(data)
    local ch;
    local n = nebula;
    print("neb load called")

    if not data then return nil,"need data" end

    ch = n.character_add()
--   if data.name then ch:name_set(data.name) end
    print(data.name)

    if data.attrs then
	for i,ad in ipairs(data.attrs) do
	    local at = ch:attr_add(ad.name)
	    for k,ed in ipairs(ad.elems) do
		if ed.type == "ref" then
		    at:ref_add(ed.ref, false)
		elseif ed.type == "value" then
		    at:value_add(ed.value)
		else
		    print("Unknwon element:",ed.type)
		end
	    end
	end
    end


    return ch
end


-- vim: set sw=4 sts=4 :
