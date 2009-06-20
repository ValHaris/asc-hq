-- This is an example for querying the terrain properties of a field.
-- The script iterates over all fields on the map and will put a bridge
-- object on those fields that have the 'shallow_water' property.
--
-- The names of the properties are the same as used in the .asctxt files
-- A list of all terrain properties (and some other stuff) can be found at:
-- http://terdon.asc-hq.org/asc/srcdoc/html/textfiletags_8cpp-source.html
--
-- Available in: mapeditor

map = asc.getActiveMap();

-- get the bridge object
bridge = asc.getObjectType( 1000 )

for y = 0, map:height()-1 do
  for x = 0, map:width()-1 do 
     fld = map:getField(x,y)
     if  fld:hasProperty ( 'Shallow_water' ) then
        -- here, we are creating an object of type MapCoordinate from the integer coordinate x and y
        asc.placeObject( map, asc.MapCoordinate(x,y), bridge )
    end
  end
end
