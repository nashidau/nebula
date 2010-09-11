
function nebload(data)
    local ch;
    local n = nebula;
    print("neb load called")

    if not data then return nil,"need data" end

    ch = n.character_add()
    if data.name then ch:name_set(data.name) end

    if data.attrs then
	for i,ad in ipairs(data.attrs) do
	    local at = ch:attr_add(ad.name)
	    for k,ed in ipairs(ad.elems) do
		if ed.type == "ref" then
		    local r = {}
		    r.ref = ed.ref
		    r.filter = ed.filter
		    r.check = false
		    at:ref_add(r)
		elseif ed.type == "value" then
		    at:value_add(ed.value)
		else
		    print("Unknwon element:",ed.type)
		end
	    end
	end
    end

    print("...done loading")
    print(ch)

    return ch
end



function harpstatfilter(value)
    return (value - 50) / 5
end

function harpkillranks(ranks)
    return ranks * 5
end




-- vim: set sw=4 sts=4 :
