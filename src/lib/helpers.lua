
function nebload(data)
    local ch;
    local n = nebula;
    print("neb load called")

    if not data then return nil,"need data" end

    ch = n.character_add()
    if data.name then ch:name_set(data.name) end

    if data.notes then
	for i,note in ipairs(data.notes) do
	    local no = ch:note_add(note.name, note.note);
	end
    end

    if data.attrs then
	for i,ad in ipairs(data.attrs) do
	    local at = ch:attr_add(ad)
	    for k,ed in ipairs(ad.elems) do
		if ed.type == "ref" then
		    local r = {}
		    r.ref = ed.ref
		    r.transform = ed.transform
		    r.check = false
		    r.note = ed.note
		    at:ref_add(r)
		elseif ed.type == "value" then
		    at:value_add(ed.value, ed.note)
		else
		    print("Unknown element:",ed.type)
		end
	    end
	end
    end

    print("...done loading")
    print(ch)

    return ch
end

local harpbonustable = {
    -18, -18, -18, -18, -18, -16, -16, -16, -16, -16,
    -14, -14, -14, -14, -14, -12, -12, -12, -12, -12,
    -10, -10, -10, -10, -10,  -8,  -8,  -8,  -8,  -8,
     -6,  -6,  -6,  -6,  -6,  -4,  -4,  -4,  -4,  -4,
     -2,  -2,  -2,  -2,  -2,   0,   0,   0,   0,   0,
      1,   1,   1,   1,   1,   2,   2,   2,   2,   2,
      3,   3,   3,   3,   3,   4,   4,   4,   4,   4,
      5,   5,   5,   5,   5,   6,   6,   6,   6,   6,
      7,   7,   7,   7,   7,   8,   8,   8,   8,   8,
      9,   9,   9,   9,   9,  10,  10,  10,  10,  10,
}


function harpstattransform(value)
    if value > 100 then return value - 90 end
    if value < 1 then return -18 end
    return harpbonustable[value];
end

function harpkillranks(ranks)
    if ranks < 1 then return -25 end
    if ranks < 11 then return ranks * 5 end
    if ranks < 21 then return ranks * 2 + 30 end
    return ranks + 50
end

function harpdptransform(value)
    local v = harpstattransform(value)
    if v < 1 then
	return 0
    else
	return v
    end
end

-- Used to cost starting stats.  No real value after start of the game
function harpstatpointtransform(value)
    if value < 90 then return value end

    local del = value - 90
    return 90 + del * del
end

function dnd35skill(value)
    return value
end

-- Immortal
function immortalrankbonus(value)
    if value == 0 then return -5 end
    return value
end

-- Convert a stat value to it's bonus
function dnd35stattransform(value)
    return math.floor((value - 10) / 2)
end

-- vim: set sw=4 sts=4 :
