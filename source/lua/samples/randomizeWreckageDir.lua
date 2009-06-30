-- This script iteraterates over the whole map, locates all
-- wreckage objects and randomizes their direction

-- useable for map editor

map = asc.getActiveMap()

for y = 0,map:height()-1 do
  for x = 0,map:width()-1 do
    fld = map:getField(x,y)
    for o = 0,fld:getObjectCount()-1 do
       obj = fld:getObject(o)
       if  string.find( obj:getType():getName(), "Wreckage") then
          obj:setDir( math.random(6))
       end
    end
  end
end
