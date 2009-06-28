-- this scipt (usable in map editor) changes the weather for all fields on the map
-- It also demonstrates how to persist properties for a map.
-- the current weather is stored in the map and each invocation of the script will 
-- change the weather one more step  
--
-- script intended for usage in mapeditor

map = asc.getActiveMap()

weatherString = map:getProperties():getValue('lua.weatherswitcher.value');
-- getValue always returns variables of type String

-- we are now creating setting up an integer with the weather index
if w == '' then
  weatherNumeric = 0;
else
  weatherNumeric = tonumber( weatherString )
end

-- there are 6 weather states defined in ASC
if  weatherNumeric < 5  then
   weatherNumeric = weatherNumeric + 1
else
   -- if we have had the last ('snow and ice'), start from beginning
   weatherNumeric = 0
end


for y = 0,map:height()-1 do
  for x = 0,map:width()-1 do
     fld = map:getField(x,y)
     fld:setWeather( weatherNumeric )
  end
end 

-- while setValue uses a String as value, Lua will do an automatic conversion from int to string
map:getProperties():setValue('lua.weatherswitcher.value', weatherNumeric );
